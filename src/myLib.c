#include "myLib.h"

unsigned short *videoBuffer = (unsigned short *)0x6000000;
extern const unsigned char fontdata_6x8[12288];
u16 key_curr = 0;
u16 key_prev = 0;
//Sets a single pixel to the passed in color
void setPixel(int row, int col, unsigned short color) {
    videoBuffer[OFFSET(row, col, 240)] = color;
}
// Draws a rectangle starting at the passed-in row and column
void drawRect(int row, int col, int height, int width, u16 color) {
    for (int r = 0; r < height; r++) {
        DMA[3].src = &color;
        DMA[3].dst = videoBuffer + OFFSET(row + r, col, 240);
        DMA[3].cnt = (width) | DMA_ON | DMA_DESTINATION_INCREMENT | DMA_SOURCE_FIXED;
        }
}
// Draws a char from fontdata onto the screen
void drawChar(int row, int col, char ch, u16 color) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 6; c++) {
            if (fontdata_6x8[OFFSET(r, c, 6) + ch * 48] == 1) {
                setPixel(row + r, col + c, color);
            }
        }
    }
}
// Draws a string onto the screen
void drawString(int row, int col, char str[], unsigned short color) {
    while(*str) {
        drawChar(row, col, *str++, color);
        col+=6;
    }
}
// Draws a passed-in image onto the screen
void drawimage3(int r, int c, int width, int height, const u16* image) {
    for (int row = 0; row < height; row++) {
        DMA[3].src = image + OFFSET(row, 0, width);
        DMA[3].dst = videoBuffer + OFFSET(row + r, c, 240);
        DMA[3].cnt = width | DMA_ON;
    }
}

// waitForVblank implementation using SCANLINECOUNTER
void waitForVblank() {
    while(SCANLINECOUNTER > 160);
    while(SCANLINECOUNTER < 160);
}
// Fills the entire screen the passed in color
void fillScreen(volatile u16 color) {
    DMA[3].src = &color;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = (240*160) | DMA_ON | DMA_SOURCE_FIXED;
}
// Finds the previously pressed key as well as the currently pressed key
void key_poll() {
    key_prev = key_curr;
    key_curr = ~BUTTONS & KEY_MASK;
}

// Returns whether the key is currently pressed down or not
 u32 key_is_down(u32 key) {
    return key_curr & key;
}
// Returns whether the key is currently up or not
 u32 key_is_up(u32 key) {
    return ~key_curr & key;
}
// Returns if the key was pressed down the previous frame
 u32 key_was_down(u32 key) {
    return key_prev & key;
}
// Returns if the key was up the previous frame
 u32 key_was_up(u32 key) {
    return ~key_prev & key;
}
// Determines whether a collision has taken place between an object at the
// coordinates r1, c1 with height h1 and width w1 with another object of
// coordinates r2, c2 with height h2 and width w2
int collisionCheck(int r1, int c1, int h1, int w1, int r2, int c2, int h2, int w2) {
    if ((r1 < (r2 + h2)) && ((r1 + h1) > r2) && (c1 < (w2 + c2)) && ((c1 + w1) > c2)) {
        return 1;
    }
    return 0;
}
