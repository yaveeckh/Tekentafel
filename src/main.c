
#include "constants.h"
//Initialiseren van de pwm pulse variabelen voor motor 0 en 1
float pulse0 = 0;
float pulse1 = 0;
float pen_up_pulse = 62;


//OXO
unsigned char initialized = 0;
unsigned char waiting_for_input = 0;
unsigned char player_turn = 0;
unsigned char run_game = 1;
unsigned char end_game;

char winner; //0 = player 0, 1 = player 1, -1 = draw

unsigned char position_x = 0;
unsigned char position_y = 0;

//Input
unsigned char down_pressed = 0;
unsigned char up_pressed = 0;
unsigned char left_pressed = 0;
unsigned char right_pressed = 0;
unsigned char center_pressed = 0;

//Struct die een punt defineert
typedef struct Punt {
    float x;
    float y;
} Punt;

Punt curr_pos = {COORDINATE_OFFSET_X, COORDINATE_OFFSET_Y};


//Initialiseet de controle flag voor de pwm signalen, 0 = servo 0 pulse, 1 = servo 1 pulse, 2 = wait period
char flag0 = 0;

ISR(TIMER1_COMPA_vect){
    cli();
    if (flag0 == 0){
        flag0 = 1;
        PORTC |= _BV(PC0);
        PORTC &= ~_BV(PC1);

        OCR1A = pulse0;
    } else if (flag0 == 1) {
        flag0 = 2;
        PORTC &= ~_BV(PC0);
        PORTC |= _BV(PC1);
        
        OCR1A = pulse1;
    } else if (flag0 == 2) {
        flag0 = 3;
        PORTC &= ~_BV(PC0);
        PORTC &= ~_BV(PC1);
        PORTB |= _BV(PB7);
        OCR1A = pen_up_pulse;
    } else {
        flag0 = 0;
        PORTC &= ~_BV(PC0);
        PORTC &= ~_BV(PC1);
        PORTB &= ~_BV(PB7);
        OCR1A = 1250 - pulse1 - pen_up_pulse;
    }
    sei();
}


//Berekent pulse uit hoek voor servo 0 en 1
int pulsecalc0 (double angle) {
    return round(-105/PI * angle + 142); 
}
int pulsecalc1 (double angle) {
    return  round(105/PI * angle + 37);
}


//Inverse kinematica
Punt calculateAngles(Punt coord){
    float x = coord.x;
    float y = coord.y;
    float q1 = acos((x*x+y*y-A1*A1-A2*A2)/(2*A1*A2));
    float q0 = (atan2(y,x) + atan2(A2*sin(q1),(A1+A2*cos(q1))));
    Punt angles = {q0, q1};
    return angles;
}

//Move to function, combineert inverse kinematica en hoek-> pulse om servos aan te sturen
void moveTo(Punt positie) {
    Punt angles = calculateAngles(positie);
    pulse0 = pulsecalc0(angles.x);
    pulse1 = pulsecalc1(angles.y);
}

void pen_up(unsigned char pen_state) {
    if (pen_state == 1) {
        pen_up_pulse = 62;
    } else {
        pen_up_pulse = 112;
    }
}

//Functie die cirkel tekent
void drawCircle(Punt middel, float r) {
    float theta = 0;
    float x = middel.x;
    float y = middel.y;
    Punt position;
    Punt p = {x+r , y};
    moveTo(p);
    _delay_ms(1000);
    pen_up(0);
    while (theta<=2*PI)
    {
        position.x = x+r*cos(theta);
        position.y = y+r*sin(theta);
        moveTo(position);
        theta+=0.0314;
        _delay_ms(20);
    }
    pen_up(1);
    _delay_ms(1000);
}

//Functie die lijn tekent
void drawLine(Punt begin, Punt eind) {
    Punt position;
    float x0 = begin.x;
    float x1 = eind.x;
    float y0 = begin.y;
    float y1 = eind.y;
    Punt p = {x0, y0};
    float t = 0;
    moveTo(p);
    _delay_ms(1000);
    pen_up(0);
    while(t <= 1) {
        position.x = x0+t*(x1-x0);
        position.y = y0+t*(y1-y0);
        moveTo(position);
        t += 0.01;
        _delay_ms(25);
    }
    pen_up(1);
    _delay_ms(1000);
}

void drawCross(Punt midden) {
    Punt rechter_bovenhoek = {midden.x + CELL_SPACING/2, midden.y + CELL_SPACING/2};
    Punt linker_bovenhoek = {midden.x - CELL_SPACING/2, midden.y + CELL_SPACING/2};
    Punt rechter_onderhoek = {midden.x + CELL_SPACING/2, midden.y - CELL_SPACING/2};
    Punt linker_onderhoek = {midden.x - CELL_SPACING/2, midden.y - CELL_SPACING/2};
    drawLine(rechter_onderhoek, linker_bovenhoek);
    drawLine(rechter_bovenhoek, linker_onderhoek);
}

void CheckWin() {
    //Check win
    //Check board

    //if win||board full -> go to end and declare winner
}


void Initialize() {
    Punt beginLijn1 = {COORDINATE_OFFSET_X + CELL_SPACING, COORDINATE_OFFSET_Y};
    Punt eindLijn1 = {COORDINATE_OFFSET_X + CELL_SPACING, 3 * CELL_SPACING + COORDINATE_OFFSET_Y};
    Punt beginLijn2 = {COORDINATE_OFFSET_X + 2 * CELL_SPACING, COORDINATE_OFFSET_Y};
    Punt eindLijn2 = {COORDINATE_OFFSET_X + 2 * CELL_SPACING, 3 * CELL_SPACING + COORDINATE_OFFSET_Y};
    Punt beginLijn3 = {COORDINATE_OFFSET_X, COORDINATE_OFFSET_Y + CELL_SPACING,};
    Punt eindLijn3 = {COORDINATE_OFFSET_X + 3 * CELL_SPACING, COORDINATE_OFFSET_Y + CELL_SPACING};
    Punt beginLijn4 = {COORDINATE_OFFSET_X, 2 * CELL_SPACING + COORDINATE_OFFSET_Y};
    Punt eindLijn4 = {COORDINATE_OFFSET_X + 3 *CELL_SPACING, 2 * CELL_SPACING + COORDINATE_OFFSET_Y};
    
    drawLine(beginLijn1, eindLijn1);
    drawLine(beginLijn2, eindLijn2);
    drawLine(beginLijn3, eindLijn3);
    drawLine(beginLijn4, eindLijn4);

    curr_pos.x = COORDINATE_OFFSET_X + 0.5 * CELL_SPACING;
    curr_pos.y = COORDINATE_OFFSET_Y + 0.5 * CELL_SPACING;
    initialized = 1;
}

void CheckInput() {
    if(~PINE & _BV(PE4)) {                  //If west button is pressed
        if (!left_pressed) {                     //Check if west button has already been pressed
            left_pressed = 1;                       //Set west button state to pressed
        }
    } else {
        left_pressed = 0;
    }

    if(~PINE & _BV(PE6)) {                  //If west button is pressed
        if (!right_pressed) {                     //Check if west button has already been pressed
            right_pressed = 1;                       //Set west button state to pressed
        }
    } else {
        right_pressed = 0;
    }

    if(~PINE & _BV(PE7)) {                  //If west button is pressed
        if (!up_pressed) {                     //Check if west button has already been pressed
            up_pressed = 1;                       //Set west button state to pressed
        }
    } else {
        up_pressed = 0;
    }

    if(~PINC & _BV(PC7)) {                  //If west button is pressed
        if (!center_pressed) {                     //Check if west button has already been pressed
            center_pressed = 1;                       //Set west button state to pressed
        }
    } else {
        center_pressed = 0;
    }
}


int main(void)
{
    //SET C PINS AS OUTPUT
    DDRC |= _BV(PC0)|_BV(PC1);
    DDRB |= _BV(PB7);

    //SET UP INTERRUPT
    TCCR1A |= _BV(COM1A1);
    TCCR1A &= ~_BV(COM1A0);

    TCCR1B |= _BV(WGM12);
    TCCR1B &= ~_BV(WGM13);

    TCCR1B  |= _BV(CS12);
    TCCR1B &= ~(_BV(CS11)|_BV(CS10));

    OCR1A = 1250;   //SET MATCH ON 1250 (FOR 50 HZ PULSES)

    sei();
    TIMSK1 |= _BV(OCIE1A);

    //Input via board
    DDRE &= ~(_BV(PE7)|_BV(PE6)|_BV(PE5)|_BV(PE4));    //Set pin 4 and 6 as input
    PORTE |= _BV(PE7)|_BV(PE6)|_BV(PE5)|_BV(PE4);    //Set pin 4 and 6 as pull up
    DDRC &= ~(_BV(PC7));
    PORTC |= _BV(PC7);
    //POTENTIAL FASTER SOLUTIONS FOR ANGLE CALCULATIONS
    //q2 = atan2(sqrt(1-(x*x+y*y-A1*A1-A2*A2)/(2*A1*A2)),(x*x+y*y-A1*A1-A2*A2)/(2*A1*A2));
    //q1 = atan2(y,x)+atan2+-A2*sin(q2),A1+A2*cos(q2));
    

    while (1)
    {

        //Print on screen "Press joystick to start"
        //Check for input


        while(run_game) {
            //Game_loop

            if (!initialized) {
                //Initialized
                Initialize();
                waiting_for_input = 1;
            }

            while (waiting_for_input) {
                //Move position when buttons pressed
                CheckInput();
                if (left_pressed) {
                    position_x = (position_x - 1)%3;
                    _delay_ms(500);
                }

                if (right_pressed){
                    position_x = (position_x + 1) % 3;
                    _delay_ms(500);
                }

                if (up_pressed) {
                    position_y = (position_y + 1) % 3;
                    _delay_ms(500);
                }

                if (down_pressed){
                    position_y = (position_y - 1) % 3;
                    _delay_ms(500);
                }
                curr_pos.x = COORDINATE_OFFSET_X + (position_x + 0.5) * CELL_SPACING;
                curr_pos.y = COORDINATE_OFFSET_Y + (position_y + 0.5) * CELL_SPACING;
                

                moveTo(curr_pos);

                //When move confirmed Draw
                //Check which player -> Draw cross/Draw Circle
                if (center_pressed){
                    
                    if (player_turn) {
                        //Draw circle
                        drawCircle(curr_pos, CELL_SPACING/2);
                    } else {
                        //Draw cross
                        drawCross(curr_pos);
                    }
                    player_turn ^= 1;
                    _delay_ms(500);
                }
                
            }

            //Check for win
            CheckWin();

        }

        while(end_game) {

        }
        

    }

    return 0;
}
