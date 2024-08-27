#include "bayer.h"
// #include "matrix.h"
//
extern uint8_t row1[width];
extern uint8_t row2[width];
extern uint8_t row3[width];
extern uint8_t row4[width];

pixel calculatePixel(int x, int y) {
  // red
  if (x % 2 != 0 && y % 2 != 0) {
    return calcRed(x, y);
  }

  // green
  // if ((x % 2 == 0 && y % 2 == 1) || (x % 2 == 1 && y % 2 == 0)) {
  //   return calcGreen(x, y);
  // }

  // blue
  if (x % 2 == 0 && y % 2 == 0) {
    return calcBlue(x, y);
  }

  // green
  return calcGreen(x, y);
}

pixel calcRed(int i, int j) {
  uint8_t red, green, blue;

  red = row2[j];
  green = (row1[j] + row3[j] + row2[j - 1] + row2[j + 1]) / 4;
  blue = (row1[j - 1] + row1[j + 1] + row3[j - 1] + row3[j + 1]) / 4;

  pixel res;
  res.rgb.red = red;
  res.rgb.green = green;
  res.rgb.blue = blue;
  return res;
}

pixel calcGreen(int i, int j) {
  uint8_t red, green, blue;

  green = row2[j];
  if (i % 2 == 0) {
    blue = (row2[j - 1] + row2[j + 1]) / 2;
    red = (row1[j] + row3[j]) / 2;
  } else {
    red = (row2[j - 1] + row2[j + 1]) / 2;
    blue = (row1[j] + row3[j]) / 2;
  }

  pixel res;
  res.rgb.red = red;
  res.rgb.green = green;
  res.rgb.blue = blue;
  return res;
}

pixel calcBlue(int i, int j) {
  uint8_t red, green, blue;

  red = (row1[j - 1] + row1[j + 1] + row3[j - 1] + row3[j + 1]) / 4;

  green = (row1[j] + row3[j] + row2[j - 1] + row2[j + 1]) / 4;

  blue = row2[j];

  pixel res;
  res.rgb.red = red;
  res.rgb.green = green;
  res.rgb.blue = blue;
  return res;
}
