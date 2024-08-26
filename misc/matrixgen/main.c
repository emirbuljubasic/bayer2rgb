#include <stdio.h>

int width = 180;
int height = 180;

void genblue();
void gengreen();
void genred();

int main() {

  printf("uint8_t bitmap[%d][%d] = {", width, height);

  genblue();
  gengreen();
  genred();

  printf("};\n");

  return 0;
}

void genblue() {
  for (int j = 0; j < 59; j++) {
    printf("{");
    for (int i = 0; i < width; i++) {
      if (j % 2 == 0) {
        if (i % 2 == 0) {
          printf("255");
        } else {
          printf("0");
        }
      } else {
        printf("0");
      }

      if (i != width - 1) {
        printf(", ");
      }
    }

    printf("}");
    if (j != height - 1) {
      printf(",\n");
    }
  }
}

void gengreen() {
  for (int j = 0; j < 62; j++) {
    printf("{");
    for (int i = 0; i < width; i++) {
      if (j % 2 == 0) {
        if (i % 2 == 0) {
          printf("255");
        } else {
          printf("0");
        }
      } else {
        if (i % 2 == 0) {
          printf("0");
        } else {
          printf("255");
        }
      }

      if (i != width - 1) {
        printf(", ");
      }
    }

    printf("}");
    if (j != height - 1) {
      printf(",\n");
    }
  }
}
void genred() {
  for (int j = 0; j < 59; j++) {
    printf("{");
    for (int i = 0; i < width; i++) {
      if (j % 2 == 0) {
        if (i % 2 == 0) {
          printf("0");
        } else {
          printf("255");
        }
      } else {
        printf("0");
      }

      if (i != width - 1) {
        printf(", ");
      }
    }

    printf("}");
    if (j != height - 1) {
      printf(",\n");
    }
  }
}
