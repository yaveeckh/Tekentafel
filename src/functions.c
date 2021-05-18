#include "functions.h"

//Initialize initial pulses for servos
int pulse0 = 0;
int pulse1 = 0;
int pen_up_pulse = PEN_UP_PULSE;
//Interrupt state for servo control
char flag0 = 0;


//OXO
//Initialize game states
//Checks if game should be running
unsigned char run_game = 1;
//Initialize board stage
unsigned char game_state = 0;
//Winner, 0 = X, 1 = O,  -1 = draw
char winner;
unsigned char player_turn = 0;
//Game board
int board [3][3];

//X and  Y positions on the board
unsigned char position_x = 0;
unsigned char position_y = 0;

//Input
unsigned char down_pressed = 0;
unsigned char up_pressed = 0;
unsigned char left_pressed = 0;
unsigned char right_pressed = 0;
unsigned char center_pressed = 0;

//Joystick analog values converted by ADC
int j_x;
int j_y;

//Current position IRL
Punt curr_pos = {COORDINATE_OFFSET_X, COORDINATE_OFFSET_Y};

//Initialize ADC for joystick
void InitJoystick()
{
    //Set VREF to VCC
    ADMUX|=(1<<REFS0);
    //Enable ADC, set prescaler for internal clock ADC to 128
    ADCSRA|=(1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);
    //Power for the joystick
    DDRB |= _BV(PB2);
    PORTB |= _BV(PB2);
}

//Initialize interrupt timer for servos
void InitServo() {

    //See output pins PWM Signal
    DDRC |= _BV(PC0)|_BV(PC1);
    DDRB |= _BV(PB7);
    
    //Set prescaler and cc mode
    TCCR1A |= _BV(COM1A1);
    TCCR1A &= ~_BV(COM1A0);

    TCCR1B |= _BV(WGM12);
    TCCR1B &= ~_BV(WGM13);

    TCCR1B  |= _BV(CS12);
    TCCR1B &= ~(_BV(CS11)|_BV(CS10));

    //Set match value
    OCR1A = O_50;

    //Interrupt on compare match
    TIMSK1 |= _BV(OCIE1A);
}

//Initialize button input
void InitButtonInput() {
    // If button input is used intializes them as input and activate internal pull up resistor
    // DDRE &= ~(_BV(PE7)|_BV(PE6)|_BV(PE5)|_BV(PE4));
    // PORTE |= _BV(PE7)|_BV(PE6)|_BV(PE5)|_BV(PE4);

    //Initialize center button as input and activate internal pull up resistor
    DDRC &= ~(_BV(PC7));
    PORTC |= _BV(PC7);
}

//Check input buttons and joystick
void CheckInput() {
    //Input via buttons, handeled by joystick in this project
    // if(~PINE & _BV(PE4)) {
    //     if (!left_pressed) {
    //         left_pressed = 1;
    //     }
    // } else {
    //     left_pressed = 0;
    // }

    // if(~PINE & _BV(PE6)) {
    //     if (!right_pressed) {
    //         right_pressed = 1;
    //     }
    // } else {
    //     right_pressed = 0;
    // }

    // if(~PINE & _BV(PE5)) {
    //     if (!down_pressed) {
    //         down_pressed = 1;                       
    //     }
    // } else {
    //     down_pressed = 0;
    // }

    // if(~PINE & _BV(PE7)) {
    //     if (!up_pressed) {
    //         up_pressed = 1;
    //     }
    // } else {
    //     up_pressed = 0;
    // }

    //Check if button has been pressed, modify button state variables accordingly
    if(~PINC & _BV(PC7)) {
        if (!center_pressed) {
            center_pressed = 1;
        }
    } else {
        center_pressed = 0;
    }
    
    //Read the analog values of the joystick input, convert them to digital and store them. Subtract/Add 512 for easier input check
    j_x = -ReadJoystick(X_AXIS) + 512;
    j_y = ReadJoystick(Y_AXIS) - 512;

    
    //Joystick controls
    //Check if x-axis reaches edge values, modify button state variable accordingly
    if (j_x > 256) {
        right_pressed = 1;
    } else {
        right_pressed = 0;
    }

    if (j_x < -256) {
        left_pressed = 1;
    } else {
        left_pressed = 0;
    }

    //Check if y-axis reaches edge values, modify button state variable accordingly
    if (j_y < -256) {
        down_pressed = 1;
    } else {
        down_pressed = 0;
    }

    if (j_y > 256) {
        up_pressed = 1;
    } else {
        up_pressed = 0;
    }
}

//Draw the game board, initialize the board array
void InitGameBoard(){
    //Enable interrupts
    sei();
    //Calculate edge points of line and draw them
    Punt pb1 = {COORDINATE_OFFSET_X + CELL_SPACING, COORDINATE_OFFSET_Y};
    Punt pe1 = {COORDINATE_OFFSET_X + CELL_SPACING, 3 * CELL_SPACING + COORDINATE_OFFSET_Y};
    Punt pb2 = {COORDINATE_OFFSET_X + 2 * CELL_SPACING, COORDINATE_OFFSET_Y};
    Punt pe2 = {COORDINATE_OFFSET_X + 2 * CELL_SPACING, 3 * CELL_SPACING + COORDINATE_OFFSET_Y};
    Punt pb3 = {COORDINATE_OFFSET_X, COORDINATE_OFFSET_Y + CELL_SPACING,};
    Punt pe3 = {COORDINATE_OFFSET_X + 3 * CELL_SPACING, COORDINATE_OFFSET_Y + CELL_SPACING};
    Punt pb4 = {COORDINATE_OFFSET_X, 2 * CELL_SPACING + COORDINATE_OFFSET_Y};
    Punt pe4 = {COORDINATE_OFFSET_X + 3 *CELL_SPACING, 2 * CELL_SPACING + COORDINATE_OFFSET_Y};
    
    DrawLine(pb1, pe1, 0);
    DrawLine(pb2, pe2, 0);
    DrawLine(pb3, pe3, 0);
    DrawLine(pb4, pe4, 0);
    
    Punt lb = {COORDINATE_OFFSET_X, 3 * CELL_SPACING + COORDINATE_OFFSET_Y};
    Punt ro = {COORDINATE_OFFSET_X + 3 * CELL_SPACING, COORDINATE_OFFSET_Y};
    DrawRectangle(lb, ro);

    //Initialize all values in array as -1 corresponding whith an empty cell
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            board[y][x] = -1;
        }
    }
    
    //St current position as most left and down cell
    curr_pos.x = COORDINATE_OFFSET_X + 0.5 * CELL_SPACING;
    curr_pos.y = COORDINATE_OFFSET_Y + 0.5 * CELL_SPACING;

    //Set game state as 2 to go to the next stage
    game_state = 2;
}

//Check if someone has won or board is full
void CheckWin() {
    //Check if oxo has been formed in the rows of the board, go to next stage
    for(int y = 0; y < 3; y++){
        if (board[y][0] == 1 && board[y][1] == 0 && board[y][2] == 1) {
        game_state = 3;
        return;
        }
        
    }
    
    //Check if oxo has been formed in the columns of the board, go to next stage
    for(int x = 0; x < 3; x++){
        if (board[0][x] == 1 && board[1][x] == 0 && board[2][x] == 1) {
        game_state = 3;
        return;
        }
    }
    
    //Check if oxo has been formed on the diagonals of the board, go to next stage
    if (board[0][0] == 1 && board[1][1] == 0 && board[2][2] == 1) {
        game_state = 3;
        return;
    }

    if (board[2][0] == 1 && board[1][1] == 0 && board[0][2] == 1) {
        game_state = 3;
        return;
    }

    //Loop over the board, if no free cells are available go to the next stage and set winner to draw
    unsigned char board_free_space = 0;
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            if (board[y][x] == -1) {
                board_free_space = 1;
            }
        }
    }

    if (board_free_space == 0) {
        game_state = 3;
        winner = -1;
        return;
    }
    
}


//Convert angles to OCR1A values for the two arm servos
int pulsecalc0 (double angle) {
    return round(-SLOPE_LIN/PI * angle + CON0_LIN); 
}
int pulsecalc1 (double angle) {
    return  round(SLOPE_LIN/PI * angle + CON1_LIN);
}


//Convert position to angles with invers kinematics, return point with both angles
Punt CalculateAngles(Punt coord){
    float x = coord.x;
    float y = coord.y;
    float q1 = acos((x*x+y*y-A1*A1-A2*A2)/(2*A1*A2));
    float q0 = (atan2(y,x) + atan2(A2*sin(q1),(A1+A2*cos(q1))));
    Punt angles = {q0, q1};
    return angles;
}

//Converts angles too OCR1A values and assigns them to the pulse variables
void MoveTo(Punt position) {
    Punt angles = CalculateAngles(position);
    pulse0 = pulsecalc0(angles.x);
    pulse1 = pulsecalc1(angles.y);
}

//Sets OCR1A values for the pen servo, 1 = up, 0 = down
void pen_up(unsigned char pen_state) {
    if (pen_state == 1) {
        pen_up_pulse = PEN_UP_PULSE;
    } else {
        pen_up_pulse = PEN_DOWN_PULSE;
    }
}

//Draws a circle in small increments using parametric equation of the circle
void DrawCircle(Punt center, float r) {
    //Variable which holds current theta angle
    float theta = 0;
    //Variable which holds current position
    Punt position;
    //Set x and y as coordinates middlepoint
    float x = center.x;
    float y = center.y;
    //Move to first drawing point on circle
    Punt p = {x+r , y};
    MoveTo(p);
    //Delay while moving
    _delay_ms(1000);
    //Move pen down
    pen_up(0);
    //While t <= 2*PI, calculate next point, moe to that point and increment parameter
    while (theta<=2*PI)
    {
        position.x = x+r*cos(theta);
        position.y = y+r*sin(theta);
        MoveTo(position);
        theta+=INC_C;
        _delay_ms(20);
    }
    //Delay while pen goes up
    pen_up(1);
    _delay_ms(1000);
}

//Draws a line in small increments using first degree bezier curve, p0 = first point, p1 = second point, rect = tekenen van rechthoek -> pen niet omhoog
void DrawLine(Punt p0, Punt p1, unsigned char rect) {
    //Variable whic holds current position
    Punt position;

    //Variable which holds all the edge coordinates of the line
    float x0 = p0.x;
    float x1 = p1.x;
    float y0 = p0.y;
    float y1 = p1.y;

    //Move to first point on line
    Punt p = {x0, y0};
    float t = 0;
    MoveTo(p);

    //Delay while moving
    _delay_ms(1000);
    //Move pen down
    pen_up(0);

   
    //While t <= 1, calculate next point, moe to that point and increment parameter
    while(t <= 1) {
        position.x = x0+t*(x1-x0);
        position.y = y0+t*(y1-y0);
        MoveTo(position);
        t += INC_B;
        _delay_ms(25);
    }
    //If not in rectangle mode, pen up
    if(!rect) {
        //Move pen up
        pen_up(1);
        //Delay while pen goes up
        _delay_ms(1000);
    }
}

//Draws quardratic bezier curve in small increments
void DrawBezier2(Punt p0, Punt p1, Punt p2) {
    //Variable whic holds current position
    Punt position;

    //Variable which holds all the edge coordinates of the line
    float x0 = p0.x;
    float x1 = p1.x;
    float x2 = p2.x;
    float y0 = p0.y;
    float y1 = p1.y;
    float y2 = p2.y;

    //Move to first point on line
    Punt p = {x0, y0};
    float t = 0;
    MoveTo(p);
    
    //Delay while moving
    _delay_ms(1000);
    //Move pen down
    pen_up(0);
    //While t <= 1, calculate next point, moe to that point and increment parameter
    while(t <= 1) {
        position.x = (1-t)*(1-t)*x0+2*t*(1-t)*x1+t*t*x2;
        position.y = (1-t)*(1-t)*y0+2*t*(1-t)*y1+t*t*y2;
        MoveTo(position);
        t += INC_B;
        _delay_ms(25);
    }
    //Move pen up
    pen_up(1);
    //Delay while pen goes up
    _delay_ms(1000);
}

//Draws 2 lines forming a cross
void DrawCross(Punt center) {
    Punt rechter_bovenhoek = {center.x + CELL_SPACING/2, center.y + CELL_SPACING/2};
    Punt linker_bovenhoek = {center.x - CELL_SPACING/2, center.y + CELL_SPACING/2};
    Punt rechter_onderhoek = {center.x + CELL_SPACING/2, center.y - CELL_SPACING/2};
    Punt linker_onderhoek = {center.x - CELL_SPACING/2, center.y - CELL_SPACING/2};
    //Not in rectangle mode, pen should be upbetween lines
    DrawLine(rechter_onderhoek, linker_bovenhoek, 0);
    DrawLine(rechter_bovenhoek, linker_onderhoek, 0);
}

//Draws 4 lines forming a rectangle
void DrawRectangle(Punt linker_bovenhoek, Punt rechter_onderhoek){
    Punt linker_onderhoek = {linker_bovenhoek.x, rechter_onderhoek.y};
    Punt rechter_bovenhoek = {rechter_onderhoek.x, linker_bovenhoek.y};
    DrawLine(rechter_bovenhoek, rechter_onderhoek, 1);
    DrawLine(rechter_onderhoek, linker_onderhoek, 1);
    DrawLine(linker_onderhoek, linker_bovenhoek, 1);
    DrawLine(linker_bovenhoek, rechter_bovenhoek, 0);
}

//Reads analog values of joystick and converts them, port represents the analog pin used for input
unsigned long int ReadJoystick(unsigned int pin)
{
    //Clear the last 5 bits of ADMUX and fill them with port which will configure the used pin in the reister correctly
    ADMUX = (ADMUX & 0b11100000)|pin; 
    //Start the conversion
    ADCSRA|=_BV(ADSC);
    //Wait until the conversion is over
    while((ADCSRA)&_BV(ADSC));
    //Return ADC
    return(ADC);
}
