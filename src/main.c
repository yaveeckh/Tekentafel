 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#define A1 12.5
#define A2 14.5
#define PI 3.1415920


//Struct die een punt defineert
typedef struct Punt {
    float x;
    float y;
} Punt;

//Initialiseren van de pwm pulse variabelen voor motor 0 en 1
float pulse0 = 0;
float pulse1 = 0;
float pen_up_pulse = 0;

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
    while (theta<=2*PI)
    {
        position.x = x+r*cos(theta);
        position.y = y+r*sin(theta);
        moveTo(position);
        theta+=0.0314;
        _delay_ms(20);
    }
}

//Functie die lijn tekent
void drawLine(Punt begin, Punt eind) {
    Punt position;

    float x0 = begin.x;
    float x1 = eind.x;
    float y0 = begin.y;
    float y1 = eind.y;

    float t = 0;
    while(t <= 1) {
        position.x = x0+t*(x1-x0);
        position.y = y0+t*(y1-y0);
        moveTo(position);
        t += 0.01;
        _delay_ms(20);
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

    //POTENTIAL FASTER SOLUTIONS FOR ANGLE CALCULATIONS
    //q2 = atan2(sqrt(1-(x*x+y*y-A1*A1-A2*A2)/(2*A1*A2)),(x*x+y*y-A1*A1-A2*A2)/(2*A1*A2));
    //q1 = atan2(y,x)+atan2+-A2*sin(q2),A1+A2*cos(q2));

    // Punt beginLijn1 = {4, 5};
    // Punt eindLijn1 = {4, 17};
    // Punt beginLijn2 = {8, 5};
    // Punt eindLijn2 = {8, 17};
    // Punt beginLijn3 = {0, 9};
    // Punt eindLijn3 = {12, 9};
    // Punt beginLijn4 = {0, 13};
    // Punt eindLijn4 = {12, 13};

    // Punt middenCirkel = {6,11};

    // drawLine(beginLijn1, eindLijn1);
    // drawLine(beginLijn2, eindLijn2);
    // drawLine(beginLijn3, eindLijn3);
    // drawLine(beginLijn4, eindLijn4);
    
    // drawCircle(middenCirkel, 1.5);


    
    
    while (1)
    {
        pen_up(1);
        _delay_ms(1000);
        pen_up(0);
        _delay_ms(1000);
    }

    return 0;
}