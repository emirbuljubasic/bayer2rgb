#include "bayer.h"
#include "delay.h"
#include "stm32f4xx.h"
#include "string.h"
#include "usart.h"
#include <stdio.h>

typedef uint8_t pix;

void calculateRow(pix *row1, pix *row2, pix *row3, pixel *row4, uint8_t i);
void convertPixelToRow(uint8_t *data, pixel *row);

pix row1[width];
pix row2[width];
pix row3[width];
// pixel row4[width * 2];
pixel row4[width];
uint8_t row5[width * 3 + 1];
// uint8_t row5[width * 3 + 1];

int main(int argc, char *argv[]) {

  initUSART2(USART2_BAUDRATE_115200);
  init_DMA1();
  // initSTOPWATCH();

  getrowUSART2_DMA(row1);
  getrowUSART2_DMA(row2);
  getrowUSART2_DMA(row3);

  int i = 0;
  // int time = 0;
  for (; i < height - 3; i++) {
    calculateRow(row1, row2, row3, row4, i);

    convertPixelToRow(row5, row4);
    sendrowUSART2_DMA(row5);

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

void convertPixelToRow(uint8_t *data, pixel *row) {
  int i = 0, n = 0;
  for (; i < width; i++) {
    n = i * 3;
    data[n] = (row[i].rgb.red * 31) / 255;
    data[n + 1] = (row[i].rgb.green * 63) / 255;
    data[n + 2] = (row[i].rgb.blue * 31) / 255;
  }
  data[width * 3] = 0xFF;
}
