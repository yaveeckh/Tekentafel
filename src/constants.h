#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#define A1 12.5
#define A2 14.5
#define PI 3.1415920

//OXO
#define COORDINATE_OFFSET_X 5
#define COORDINATE_OFFSET_Y 3
#define CELL_SPACING 4