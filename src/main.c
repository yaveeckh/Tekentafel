#include "functions.h"

//Timer interrupt handler, uses a flag to control the 3 servos by enabling/disabling the right pins in each fase
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
        OCR1A = O_50 - pulse0 - pulse1 - pen_up_pulse;
    }
    sei();
}


int main(void)
{
    //Initialize all registers and lcd screen
    InitServo();
    InitButtonInput();
    InitJoystick();
    initBoard();
    initLCD();
    clearLCD();

    //Tunr backlight on lcd screen
    backlightOn();

    while (1)
    {

        //Check if the game should be running, used for future expansion.
        //Game loop
        while(run_game) {
            //Checks player input
            CheckInput();

            //gamestate 0 = mainmenu, render and wait for input
            if (game_state == 0) {
                printStringToLCD("Press center to start", 0, 0);
                if (center_pressed) {
                    game_state = 1;
                    _delay_ms(500);
                    clearLCD();
                }
            }

            //gamestate 1 = initialize board fase
            else if (game_state == 1) {
                printStringToLCD("Initializing ...", 0, 0);
                //Draw gameboard, initialize array
                InitGameBoard();
                clearLCD();
            }

            else if (game_state == 2) {
                //Print x and y position to lcd
                printStringToLCD("X= ", 0, 0);
                printIntToLCD(position_x, 0, 3);
                printStringToLCD("Y= ", 1, 0);
                printIntToLCD(position_y, 1, 3);
                //If button has been pressed, change discrete board coordinates
                if (left_pressed) {
                    if (position_x == 0) {
                        position_x = 2;
                    } else {
                        position_x = (position_x -1)%3;
                    }
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
                    if (position_y == 0) {
                        position_y = 2;
                    } else {
                        position_y = (position_y -1)%3;
                    }
                    _delay_ms(500);
                }


                //Calculate the real position of the arm with the discrete board coordinates
                curr_pos.x = COORDINATE_OFFSET_X + (position_x + 0.5) * CELL_SPACING;
                curr_pos.y = COORDINATE_OFFSET_Y + (position_y + 0.5) * CELL_SPACING;
                
                //Move to chosen position
                MoveTo(curr_pos);

                //Confirm move by pressing button, if space is free draw the right figure and modify board array, else reject move
                if (center_pressed){
                    if (board[position_y][position_x] == -1) {
                         if (player_turn) {
                            //Draw circle
                            DrawCircle(curr_pos, CELL_SPACING/2);
                            //Modify array value to 1 = O
                            board[position_y][position_x] = 1;
                        } else {
                            //Draw cross
                            DrawCross(curr_pos);
                            //Modify array value to 0 = X
                            board[position_y][position_x] = 0;
                        }
                        //Set winner to player in case player wins
                        winner = player_turn;
                        //Switch player for next move
                        player_turn ^= 1;    
                        //Check if player has won or a draw has occured
                        CheckWin();
                    } else {
                        //Reject turn
                        printStringToLCD("Cell Full", 0, 0);
                        _delay_ms(500);
                        clearLCD();
                    }

                }
                
            }
            //gamestate 2 = Announce winner stage
            else if (game_state == 3) {
                //Announce winner
                if (winner == 0) {
                    printStringToLCD("X won!", 0, 0);
                } else if (winner == 1) {
                    printStringToLCD("O won!", 0, 0);
                } else {
                    printStringToLCD("Draw!", 0, 0);
                }
                
                printStringToLCD("C: Restart!", 1, 0);
                if (center_pressed) {
                    game_state = 0;
                    _delay_ms(500);
                    clearLCD();
                }
            }
        }

    }

    return 0;
}