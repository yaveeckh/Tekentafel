#ifndef FUNCTIONS
#define FUNCTIONS

#include "constants.h"
#include "dwenguino/dwenguino_board.h"
#include "dwenguino/dwenguino_lcd.h"
#include <stdlib.h>
#include <string.h>

 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

//Declare variables used to store the OCR1A VALUES FOR THE TIMER INTERRUPT
extern int pulse0;
extern int pulse1;
extern int pen_up_pulse;

//Declare flag for interrupt states (which servo/wait)
extern char flag0;

//Declare board array
extern int board [3][3];

//Declare game states
extern unsigned char initialize;
extern unsigned char waiting_for_input;
extern unsigned char player_turn;
extern unsigned char run_game;
extern unsigned char end_game;
extern unsigned char game_state;
extern unsigned char mainmenu;

//Declare winner
extern char winner; //0 = player X, 1 = player O, -1 = draw

//Declare board coordinates
extern unsigned char position_x;
extern unsigned char position_y;

//Input
extern unsigned char down_pressed;
extern unsigned char up_pressed;
extern unsigned char left_pressed;
extern unsigned char right_pressed;
extern unsigned char center_pressed;



typedef struct Punt {
    float x;
    float y;
} Punt;

Punt curr_pos;

//Declare ADC registers for joystick
void InitJoystick();
//Declare analog values x and y axis joystick
extern int j_x;
extern int j_y;

//Declare registers for servo interrupts
void InitServo();

//Declare buttons input
void InitButtonInput();

//Checks input
void CheckInput();

//Initialise game board
void InitGameBoard();

//Check if someone has won or board is full
void CheckWin();


//Declare function that converts OCR1A value to 
int pulsecalc0 (double angle);
int pulsecalc1 (double angle);

//Declare inverse kinematics function
Punt CalculateAngles(Punt coord);

//Declare move function
void MoveTo(Punt postion);

//Declare pen_up function
void pen_up(unsigned char pen_state);

//Declare draw circle
void DrawCircle(Punt center, float r);

//Declare draw line
void DrawLine(Punt p0, Punt p1, unsigned char rect);

//Declare draw cross
void DrawCross(Punt center);

//Declare draw bezier curve
void DrawBezier2(Punt p0, Punt p1, Punt p2);

//Declare draw rectangle
void DrawRectangle(Punt p0, Punt p2);

//Reads joystick values, ch 7 is x-axis, ch 8 is y-axis
unsigned long int ReadJoystick(unsigned int port);

#endif
