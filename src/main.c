 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#define A1 12.5
#define A2 14.5
#define PI 3.141592

typedef struct Punt {
    float x;
    float y;
} Punt;

float q0;
float q1;

float x;
float y;

float pulse0 = 100;
float pulse1 = 100;

char flag0 = 0;
char flag1 = 0;

ISR(TIMER1_COMPA_vect){
    if (flag0){
        flag0 = 0;
        PORTC &= ~_BV(PC0);
        OCR1A = 1250;
    } else {
        flag0 = 1;
        PORTC |= _BV(PC0);
        OCR1A = pulse0;
    }
}

ISR(TIMER3_COMPA_vect) {
    if (flag1){
        flag1 = 0;
        PORTC &= ~_BV(PC1);
        OCR3A = 1250;
    } else {
        flag1 = 1;
        PORTC |= _BV(PC1);
        OCR3A = pulse1;
    }
}

//CALCULATE PULSELENGTHS FROM ANGLE
int pulsecalc0 (double angle) {
    return round(-105/PI * angle + 142); 
}
int pulsecalc1 (double angle) {
    return  round(105/PI * angle + 37);
}

Punt calculateAngles(Punt coord){
    float x = coord.x;
    float y = coord.y;
    q1 = acos((x*x+y*y-A1*A1-A2*A2)/(2*A1*A2));
    q0 = (atan2(y,x) + atan2(A2*sin(q1),(A1+A2*cos(q1))));
    Punt angles = {q0, q1};
    return angles;
}

int main(void)
{

    //SET C PINS AS OUTPUT
    DDRC |= _BV(PC0)|_BV(PC1);

    //SET UP INTERRUPT
    TCCR1A |= _BV(COM1A1);
    TCCR1A &= ~_BV(COM1A0);
    TCCR3A |= _BV(COM3A1);
    TCCR3A &= ~_BV(COM3A0);

    TCCR1B |= _BV(WGM12);
    TCCR1B &= ~_BV(WGM13);
    TCCR3B |= _BV(WGM32);
    TCCR3B &= ~_BV(WGM33);

    TCCR1B  |= _BV(CS12);
    TCCR1B &= ~(_BV(CS11)|_BV(CS10));
    TCCR3B |= _BV(CS32);
    TCCR3B &= ~(_BV(CS31)|_BV(CS30));

    OCR1A = 1250;   //SET MATCH ON 1250 (FOR 50 HZ PULSES)
    OCR3A = 0;

    sei();
    TIMSK1 |= _BV(OCIE1A);
    TIMSK3 |= _BV(OCIE3A);

    //POTENTIAL FASTER SOLUTIONS FOR ANGLE CALCULATIONS
    //q2 = atan2(sqrt(1-(x*x+y*y-A1*A1-A2*A2)/(2*A1*A2)),(x*x+y*y-A1*A1-A2*A2)/(2*A1*A2));
    //q1 = atan2(y,x)+atan2+-A2*sin(q2),A1+A2*cos(q2));


    Punt testPunt = {5,0};
    Punt testAngles = calculateAngles(testPunt);
    q0 = testAngles.x;
    q1 = testAngles.y;

    pulse0 = pulsecalc0(q0);
    pulse1 = pulsecalc1(q1);


    
    while (1)
    {
    }

    return 0;
}