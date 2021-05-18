#ifndef CONSTANTS
#define CONSTANTS

//Length arms and defining PI
#define A1 12.5
#define A2 14.5
#define PI 3.1415920

//OXO offset values of the board and cell spacing
#define COORDINATE_OFFSET_X 5
#define COORDINATE_OFFSET_Y 3
#define CELL_SPACING 4

//OCR1A values for pen servo
#define PEN_UP_PULSE 62
#define PEN_DOWN_PULSE 112

//OCR1A value for 50Hz
#define O_50 1250

//Slope angle to OCR1A values conversion
#define SLOPE_LIN 105
//Constant 1 angle to OCR1A values conversion
#define CON0_LIN 142
//Constant 2 angle to OCR1A values conversion
#define CON1_LIN 37


//Increment for drawing bezier curves
#define INC_B 0.01
//Increment for drawing circle
#define INC_C 0.0314

//Pins in F port for x-axis
#define X_AXIS 7
//Pins in F port for y-axis
#define Y_AXIS 6
#endif