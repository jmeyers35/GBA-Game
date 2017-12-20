#include <stdio.h>
#include <stdlib.h>
#include "myLib.h"
#include "myman.h"
#include "startscreen.h"
#include "cloutpants.h"
#include "hater.h"
#include "gameover.h"
int main() {
    // Enable mode 3 and bitmap mode
    REG_DISPCTL = MODE3 | BG2_ENABLE;
    // Game will start at the start screen
    enum gameState state = START;

    // Create Luciano so he can do it to em
    object luciano;
    luciano.row = rand() % 100;
    luciano.col = rand() % 200;
    luciano.rdel = 1;
    luciano.cdel = 1;
    // Create the clout pants for Luciano to collect
    object pants;
    pants.row = rand() % 100;
    pants.col = rand() % 200;
    pants.rdel = 0;
    pants.cdel = 0;


    // Create our worst nemesis: Yung Cash Register AKA Lil Broomstick, who is
    //the enemy in our game
    object broomstick;
    broomstick.row = 0;
    broomstick.col = 0;
    broomstick.rdel = 1;
    broomstick.cdel = 1;
    static int score = 0;
    static int highscore = 0;
    // String buffer used for printString()
    char buffer[100];
    // Game Loop
    while(1) {
        // call waitForVblank first
        waitForVblank();
        sprintf(buffer, "CLOUT: %d", score);
        // Determine what key is being pressed at a given frame
        key_poll();
        switch(state) {
            // Draws Start screen, then moves to idle state
            case START:
                drawimage3(0,0,STARTSCREEN_WIDTH, STARTSCREEN_HEIGHT, startscreen);
                drawString(120, 30, "Press Start to do it to em!", RED);
                state = START_NODRAW;
                break;

            // Idle state that waits for the user to start the game by pressing
            // the Start button
            case START_NODRAW:
            if (key_is_down(BUTTON_START) & key_was_up(BUTTON_START)) {
                state = GAME_DRAW;
            }
                break;
            // Draws the background as well as the initial locations of the
            // in-game objects/characters, then moves to the game state
            case GAME_DRAW:
            fillScreen(BLACK);
            drawimage3(luciano.row, luciano.col, MYMAN_WIDTH, MYMAN_HEIGHT, myman);
            drawimage3(pants.row, pants.col, CLOUTPANTS_WIDTH, CLOUTPANTS_HEIGHT, cloutpants);

            drawimage3(broomstick.row, broomstick.col, HATER_WIDTH, HATER_HEIGHT, hater);

            drawString(150,5, buffer, WHITE);

            state = GAME;

            break;
            // Main game state. Handles movement and collision detection of all
            // objects in the game, as well as updating score
            case GAME:
                if (key_is_down(BUTTON_SELECT) & key_was_up(BUTTON_SELECT)) {
                    state = START;
                }
                // Draw updated positions of luciano and the pants
                drawimage3(luciano.row, luciano.col, MYMAN_WIDTH, MYMAN_HEIGHT, myman);
                drawimage3(pants.row, pants.col, CLOUTPANTS_WIDTH, CLOUTPANTS_HEIGHT, cloutpants);

                // if Lil Broomstick is about to go off the screen,
                // turn him around
                if ((broomstick.row > 140 && broomstick.rdel > 0) || (broomstick.row < 20 && broomstick.rdel < 0)) {
                    broomstick.rdel = -(broomstick.rdel);
                }
                if ((broomstick.col > 220 && broomstick.cdel > 0) || (broomstick.col < 20 && broomstick.cdel < 0)) {
                    broomstick.cdel = -(broomstick.cdel);
                }
                // Draw black rectangle to cover old image of Lil Broomstick,
                // move him, then draw Lil Broomstick at his new location
                drawRect(broomstick.row, broomstick.col, HATER_HEIGHT, HATER_WIDTH, BLACK);
                broomstick.row += broomstick.rdel;
                broomstick.col += broomstick.cdel;
                drawimage3(broomstick.row, broomstick.col, HATER_WIDTH, HATER_HEIGHT, hater);


                // These four conditionals handle the movement of Luciano based
                // on key input. Draws black rectangles to cover Luciano's old
                // position, updates his position, then draws him at
                // his new location.
                if (key_is_down(BUTTON_UP) && luciano.row > 0) {
                    drawRect(luciano.row, luciano.col, MYMAN_HEIGHT, MYMAN_WIDTH, BLACK);
                    luciano.row-= luciano.rdel;
                    drawimage3(luciano.row, luciano.col, MYMAN_WIDTH, MYMAN_HEIGHT, myman);
                }
                if (key_is_down(BUTTON_DOWN) && luciano.row + MYMAN_HEIGHT < 160) {
                    drawRect(luciano.row, luciano.col, MYMAN_HEIGHT, MYMAN_WIDTH, BLACK);
                    luciano.row+= luciano.rdel;
                    drawimage3(luciano.row, luciano.col, MYMAN_WIDTH, MYMAN_HEIGHT, myman);
                }
                if (key_is_down(BUTTON_RIGHT) && luciano.col + MYMAN_WIDTH < 240) {
                    drawRect(luciano.row, luciano.col, MYMAN_HEIGHT, MYMAN_WIDTH, BLACK);
                    luciano.col+= luciano.cdel;
                    drawimage3(luciano.row, luciano.col, MYMAN_WIDTH, MYMAN_HEIGHT, myman);
                }
                if (key_is_down(BUTTON_LEFT) && luciano.col > 0) {
                    drawRect(luciano.row, luciano.col, MYMAN_HEIGHT, MYMAN_WIDTH, BLACK);
                    luciano.col-= luciano.cdel;
                    drawimage3(luciano.row, luciano.col, MYMAN_WIDTH, MYMAN_HEIGHT, myman);
                }

                // This conditional checks if a collision has occured between
                // Luciano and the pants. If that's the case, cover up the
                // old location of the pants, randomly generate a new location,
                // redraw the pants, increment the score, then update the
                // score displayed on the screen.
                if (collisionCheck(luciano.row, luciano.col, MYMAN_HEIGHT, MYMAN_WIDTH, pants.row, pants.col, CLOUTPANTS_HEIGHT, CLOUTPANTS_WIDTH)) {
                    drawRect(pants.row, pants.col, CLOUTPANTS_HEIGHT, CLOUTPANTS_WIDTH, BLACK);
                    pants.row = rand() % 120;
                    pants.col = rand() % 200;
                    drawimage3(pants.row, pants.col, CLOUTPANTS_WIDTH, CLOUTPANTS_HEIGHT, myman);
                    score++;
                    sprintf(buffer, "CLOUT: %d", score);
                    drawRect(150, 5, 30, 80, BLACK);
                    drawString(150,5, buffer, WHITE);
                }
                // Checks if a collision has occured between Lil Broomstick
                // and Luciano. If that's the case, GAME OVER
                if (collisionCheck(luciano.row, luciano.col, MYMAN_HEIGHT, MYMAN_WIDTH, broomstick.row, broomstick.col, HATER_HEIGHT, HATER_WIDTH)) {
                    state = GAME_OVER_DRAW;
                }

                break;
            // Draws the game over screen, then moves to the idle state.
            // Prints and updates the high score if need be.
            case GAME_OVER_DRAW:
                drawimage3(0,0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT, gameover);
                drawString(40, 60, "GAME OVER", BLUE);
                sprintf(buffer, "CLOUT COLLECTED: %d", score);
                drawString(60, 60, buffer, BLUE);
                if (score > highscore) {
                    highscore = score;
                }
                sprintf(buffer, "HIGH SCORE: %d", highscore);
                drawString(80, 60, buffer, BLUE);
                drawString(100, 20, "PRESS START TO DO IT TO EM AGAIN", BLUE);
                state = GAME_OVER_NODRAW;
                break;
            // Waits for the user to restart the game with the Start button

            case GAME_OVER_NODRAW:
                if (key_is_down(BUTTON_START) && key_was_up(BUTTON_START)) {
                    state = RESET;
                }
                break;

            // Randomises the starting positions of the objects for the next
            // attempt
            case RESET:
            luciano.row = rand() % 100;
            luciano.col = rand() % 200;
            pants.row = rand() % 100;
            pants.col = rand() % 200;
            broomstick.row = rand() % 100;
            broomstick.col = rand() % 200;
            score = 0;
            state = GAME_DRAW;
            break;
            }
        }
    }
