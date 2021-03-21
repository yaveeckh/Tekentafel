 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#define A1 12.5
#define A2 14.5
#define PI 3.141592

float q0;
float q1;
float x = 5;
float y = 10;

ISR(TIMER1_COMPA_vect) {
    PORTC &= ~_BV(PC0);
    PORTC &= ~_BV(PC1);
}

//CALCULATE PULSELENGTHS FROM ANGLE
int pulsecalc0 (double angle) {
    return 1250 - round(-105/PI * angle + 142);
}
int pulsecalc1 (double angle) {
    return 1250 - round(105/PI * angle + 37);
}

int main(void)
{
    //SET C PINS AS OUTPUT
    DDRC |= _BV(PC0)|_BV(PC1);

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

    //CALCULATIONS OF THE ANGLES (TO DO: MAKE FUNCTION)
    q1 = acos((x*x+y*y-A1*A1-A2*A2)/(2*A1*A2));
    q0 = (atan2(y,x) + atan2(A2*sin(q1),(A1+A2*cos(q1))));

    //POTENTIAL FASTER SOLUTIONS FOR ANGLE CALCULATIONS
    //q2 = atan2(sqrt(1-(x*x+y*y-A1*A1-A2*A2)/(2*A1*A2)),(x*x+y*y-A1*A1-A2*A2)/(2*A1*A2));
    //q1 = atan2(y,x)+atan2+-A2*sin(q2),A1+A2*cos(q2));

    int test_pulse1 = pulsecalc1(q1);
    int test_pulse0 = pulsecalc0(q0);
    
    while (1)
    {
        if(TCNT1 == test_pulse0) {
            PORTC |= _BV(PC0);
        } if(TCNT1 == test_pulse1) {
            PORTC |= _BV(PC1);
        }
    }

    return 0;
}