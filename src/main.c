 
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

ISR(TIMER1_COMPA_vect){
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
    } else {
        flag0 = 0;
        PORTC &= ~_BV(PC0);
        PORTC &= ~_BV(PC1);
        OCR1A = 1250;
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

Punt calculateCircle(float x, float y, float theta, float r) {
    Punt postions = {x+r*cos(theta), y+r*sin(theta)};
    return postions;
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

    //POTENTIAL FASTER SOLUTIONS FOR ANGLE CALCULATIONS
    //q2 = atan2(sqrt(1-(x*x+y*y-A1*A1-A2*A2)/(2*A1*A2)),(x*x+y*y-A1*A1-A2*A2)/(2*A1*A2));
    //q1 = atan2(y,x)+atan2+-A2*sin(q2),A1+A2*cos(q2));


    Punt testPunt = {5,0};
    Punt testAngles = calculateAngles(testPunt);
    q0 = testAngles.x;
    q1 = testAngles.y;

    pulse0 = pulsecalc0(q0);
    pulse1 = pulsecalc1(q1);
    


    float angle = 0;
    float r = 2;
    float x = 5;
    float y = 5;

    while(angle <= 2*PI) {
        Punt postities = calculateCircle(x,y, angle, 4);
        Punt angles = calculateAngles(postities);
        q0 = angles.x;
        q1 = angles.y;
        pulse0 = pulsecalc0(q0);
        pulse1 = pulsecalc1(q1);
        angle += 0.005;
        _delay_ms(5);
    }
    
    while (1)
    {
    }

    return 0;
}