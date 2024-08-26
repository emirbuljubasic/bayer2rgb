#include "bayer.h"
#include "delay.h"
#include "stm32f4xx.h"
#include "string.h"
#include "usart.h"
#include <stdio.h>

typedef uint8_t pix;

void writeRgbUSART(pixel rgbmap[width][height]);
void grabRow(pix *row);

void calculateRow(pix *row1, pix *row2, pix *row3, pixel *row4, uint8_t i);
void sendRow(pixel *row);

pix row1[width];
pix row2[width];
pix row3[width];
// pixel row4[width * 2];
pixel row4[width];

int main(int argc, char *argv[]) {

  initUSART2(USART2_BAUDRATE_115200);
  init_DMA1();
  initSTOPWATCH();

  getrowUSART2_DMA(row1);
  getrowUSART2_DMA(row2);
  getrowUSART2_DMA(row3);

  int i = 0;
  int time = 0;
  for (; i < height - 3; i++) {
    calculateRow(row1, row2, row3, row4, i);
    // sendRow(row4);
    sendrowUSART2_DMA(row4);

    memcpy(row1, row2, width);
    memcpy(row2, row3, width);

    getrowUSART2_DMA(row3);
  }

  return 0;
}

void calculateRow(pix *row1, pix *row2, pix *row3, pixel *row4, uint8_t i) {
  int j = 0;
  for (; j < width; j++) {
    row4[j] = calculatePixel(i + 1, j);
  }
}

void grabRow(pix *row) {
  int i;
  uint8_t val, num;
  for (i = 0; i < width; i++) {
    num = 0;
    while ((val = getcharUSART2()) != ',') {
      num *= 10;
      num += val - '0';
    }

    row[i] = num;
  }
}

void sendRow(pixel *row) {
  int i;
  for (i = 0; i < width; i++) {
    printUSART2("%d:%d:%d,", row[i].rgb.red, row[i].rgb.green, row[i].rgb.blue);
  }
  printUSART2("\n");
}
