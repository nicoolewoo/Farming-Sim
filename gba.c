#include "gba.h"
//executes graphics

volatile unsigned short *videoBuffer = (volatile unsigned short *) 0x6000000;
u32 vBlankCounter = 0;

void waitForVBlank(void) {
  while (SCANLINECOUNTER > 160);
  while (SCANLINECOUNTER < 160);
  vBlankCounter +=1;
}

static int __qran_seed = 42;
static int qran(void) {
  __qran_seed = 1664525 * __qran_seed + 1013904223;
  return (__qran_seed >> 16) & 0x7FFF;
}

int randint(int min, int max) { return (qran() * (max - min) >> 15) + min; }

void setPixel(int row, int col, u16 color) {

  UNUSED(row);
  UNUSED(col);
  UNUSED(color);
  videoBuffer[OFFSET(row, col, WIDTH)] = color;
}


void drawRectDMA(int row, int col, int width, int height, volatile u16 color) {
  UNUSED(row);
  UNUSED(col);
  UNUSED(width);
  UNUSED(height);
  UNUSED(color);

  volatile u16 color1;
  color1 = color;
  for(int i = 0; i < height; i++){
    DMA[3].src = &color1;
    DMA[3].dst = &videoBuffer[OFFSET(row+i , col, WIDTH)];
    DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
  }

}

void drawFullScreenImageDMA(const u16 *image) {
  UNUSED(image);
  DMA[3].src = image;
  DMA[3].dst = videoBuffer;
  DMA[3].cnt = DMA_DESTINATION_INCREMENT | DMA_SOURCE_INCREMENT | DMA_ON | WIDTH * HEIGHT;
}


void drawImageDMA(int row, int col, int width, int height, const u16 *image) {
  UNUSED(row);
  UNUSED(col);
  UNUSED(width);
  UNUSED(height);
  UNUSED(image);
  for(int i = 0; i < height; i++){   
    DMA[3].src = &image[OFFSET(i, 0, width)];
    DMA[3].dst = &videoBuffer[OFFSET(row + i, col, 240)];
    DMA[3].cnt = DMA_DESTINATION_INCREMENT | DMA_SOURCE_INCREMENT | DMA_ON | width;
  }
}

 // Draws a rectangular chunk of a fullscreen image to the video buffer.
void undrawImageDMA(int row, int col, int width, int height, const u16 *image) {
  UNUSED(row);
  UNUSED(col);
  UNUSED(width);
  UNUSED(height);
  UNUSED(image);
}


  //Fills the video buffer with a given color.

void fillScreenDMA(volatile u16 color) {
  UNUSED(color);
  u16 color1 = color;
  for (int i = 0; i < HEIGHT; i++) {
    DMA[3].src = &color1;
    DMA[3].dst = &videoBuffer[OFFSET(i, 0, WIDTH)];
    DMA[3].cnt = WIDTH | DMA_ON | DMA_SOURCE_FIXED;
  }
}

void drawChar(int row, int col, char ch, u16 color) {
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 8; j++) {
      if (fontdata_6x8[OFFSET(j, i, 6) + ch * 48]) {
        setPixel(row + j, col + i, color);
      }
    }
  }
}

void drawString(int row, int col, char *str, u16 color) {
  while (*str) {
    drawChar(row, col, *str++, color);
    col += 6;
  }
}

void drawCenteredString(int row, int col, int width, int height, char *str, u16 color) {
  u32 len = 0;
  char *strCpy = str;
  while (*strCpy) {
    len++;
    strCpy++;
  }

  u32 strWidth = 6 * len;
  u32 strHeight = 8;

  int new_row = row + ((height - strHeight) >> 1);
  int new_col = col + ((width - strWidth) >> 1);
  drawString(new_row, new_col, str, color);
}
