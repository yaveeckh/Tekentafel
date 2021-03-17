 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#define A1 12.5
#define A2 14.5
#define PI 3.141592

volatile char move0 = 0;
volatile char move1 = 0;
float q1;
float q2;
float x = 5;
float y = 5;
int pulse = 142;

ISR(TIMER1_COMPA_vect) {  
    move0 = 1;
    move1 = 1;
    PORTC &= ~_BV(PC0);
    PORTC &= ~_BV(PC1);
}
int pulsecalc (double angle) {
    return round(105/PI * angle + 37);
}

double deg_to_ms0 (double angle) {
    return -1.6/PI * angle + 2.3;
}
double deg_to_ms1 (double angle) {
    return 1.6/PI * angle + 0.7;
}

int main(void)
{
    DDRA |= 0xff;
    PORTA = 0x00;
    DDRC |= _BV(PC0)|_BV(PC1);
    // make the LED pin an output for PORTA

    TCCR1A |= _BV(COM1A1);
    TCCR1A &= ~_BV(COM1A0);

    TCCR1B |= _BV(WGM12);
    TCCR1B &= ~_BV(WGM13);

    TCCR1B  |= _BV(CS12);
    TCCR1B &= ~(_BV(CS11)|_BV(CS10));

    OCR1A = 1250;
    

    sei();
    TIMSK1 |= _BV(OCIE1A);

    q2 = acos((x*x+y*y-A1*A1-A2*A2)/(2*A1*A2));
    q1 = PI/2 + (atan(y/x) - atan(A2*sin(q2)/(A1+A2*cos(q2))));
    int test = 1250-pulsecalc(q1);
    int test1 = 1250-pulsecalc(q2);
    while (1)
    {
        // if(q1 != 0 && q2 != 0){
        //     move_arm_0(q1);
        //     move_arm_1(q2);
        // }
        if(TCNT1 == test) {
            PORTC |= _BV(PC0);
        } if(TCNT1 == test1) {
            PORTC |= _BV(PC1);
        }
        
        
    }

    return 0;
}