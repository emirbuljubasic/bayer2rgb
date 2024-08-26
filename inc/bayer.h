#ifndef __BAYER_H
#define __BAYER_H

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define width 64
#define height 64

typedef union {
  struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
  } rgb;
  uint32_t color;
} pixel;

void convert(pixel rgbmap[width][height]);
pixel calculatePixel(int x, int y);
pixel calcRed(int i, int j);
pixel calcGreen(int i, int j);
pixel calcBlue(int i, int j);
void writeRgb(pixel rgbmap[width][height]);

#endif // !BAYER_H
