#include "usart.h"
#include "stm32f4xx.h"

void initUSART2(uint32_t baudrate) {
  // wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
  //  USART2: PA2 -> TX & PA3 -> RX
  //------------------------------------------------------------------

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
  GPIOA->MODER |= (GPIO_MODER_MODER2_1) | (GPIO_MODER_MODER3_1);
  GPIOA->AFR[0] |= 0x00007700;

  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2_1;
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3_1;

  USART2->BRR = baudrate;
  // USART2->CR1 = USART_CR1_UE | USART_CR1_TE;
}

void putcharUSART2(uint8_t data) { /// print one character to USART2
  while (!(USART2->SR & USART_SR_TC))
    ;

  USART2->DR = data;
}

void printUSART2(char *str, ...) { /// print text and up to 10 arguments!
  uint8_t rstr[40];                // 33 max -> 32 ASCII for 32 bits and NULL
  uint16_t k = 0;
  uint16_t arg_type;
  uint32_t utmp32;
  uint32_t *p_uint32;
  char *p_char;
  va_list vl;

  // va_start(vl, 10);
  // // always pass the last named parameter to va_start, for compatibility with
  // older compilers
  va_start(vl, str); // always pass the last named parameter to va_start, for
                     // compatibility with older compilers
  while (str[k] != 0x00) {
    if (str[k] == '%') {
      if (str[k + 1] != 0x00) {
        switch (str[k + 1]) {
        case ('b'): {              // binary
          if (str[k + 2] == 'b') { // byte
            utmp32 = va_arg(vl, int);
            arg_type = (PRINT_ARG_TYPE_BINARY_BYTE);
          } else if (str[k + 2] == 'h') { // half word
            utmp32 = va_arg(vl, int);
            arg_type = (PRINT_ARG_TYPE_BINARY_HALFWORD);
          } else if (str[k + 2] == 'w') { // word
            utmp32 = va_arg(vl, uint32_t);
            arg_type = (PRINT_ARG_TYPE_BINARY_WORD);
          } else { // default word
            utmp32 = va_arg(vl, uint32_t);
            arg_type = (PRINT_ARG_TYPE_BINARY_WORD);
            k--;
          }

          k++;
          p_uint32 = &utmp32;
          break;
        }
        case ('d'): {              // decimal
          if (str[k + 2] == 'b') { // byte
            utmp32 = va_arg(vl, int);
            arg_type = (PRINT_ARG_TYPE_DECIMAL_BYTE);
          } else if (str[k + 2] == 'h') { // half word
            utmp32 = va_arg(vl, int);
            arg_type = (PRINT_ARG_TYPE_DECIMAL_HALFWORD);
          } else if (str[k + 2] == 'w') { // word
            utmp32 = va_arg(vl, uint32_t);
            arg_type = (PRINT_ARG_TYPE_DECIMAL_WORD);
          } else { // default word
            utmp32 = va_arg(vl, uint32_t);
            arg_type = (PRINT_ARG_TYPE_DECIMAL_WORD);
            k--;
          }

          k++;
          p_uint32 = &utmp32;
          break;
        }
        case ('c'): { // character
          char tchar = va_arg(vl, int);
          putcharUSART2(tchar);
          arg_type = (PRINT_ARG_TYPE_CHARACTER);
          break;
        }
        case ('s'): { // string
          p_char = va_arg(vl, char *);
          sprintUSART2((uint8_t *)p_char);
          arg_type = (PRINT_ARG_TYPE_STRING);
          break;
        }
        case ('f'): { // float
          uint64_t utmp64 = va_arg(
              vl, uint64_t); // convert double to float representation IEEE 754
          uint32_t tmp1 = utmp64 & 0x00000000FFFFFFFF;
          tmp1 = tmp1 >> 29;
          utmp32 = utmp64 >> 32;
          utmp32 &= 0x07FFFFFF;
          utmp32 = utmp32 << 3;
          utmp32 |= tmp1;
          if (utmp64 & 0x8000000000000000)
            utmp32 |= 0x80000000;

          if (utmp64 & 0x4000000000000000)
            utmp32 |= 0x40000000;

          p_uint32 = &utmp32;

          arg_type = (PRINT_ARG_TYPE_FLOAT);
          // arg_type = (PRINT_ARG_TYPE_HEXADECIMAL_WORD);
          // arg_type = (PRINT_ARG_TYPE_BINARY_WORD);
          break;
        }
        case ('x'): {              // hexadecimal
          if (str[k + 2] == 'b') { // byte
            utmp32 = (uint32_t)va_arg(vl, int);
            arg_type = (PRINT_ARG_TYPE_HEXADECIMAL_BYTE);
          } else if (str[k + 2] == 'h') { // half word
            utmp32 = (uint32_t)va_arg(vl, int);
            arg_type = (PRINT_ARG_TYPE_HEXADECIMAL_HALFWORD);
          } else if (str[k + 2] == 'w') { // word
            utmp32 = va_arg(vl, uint32_t);
            arg_type = (PRINT_ARG_TYPE_HEXADECIMAL_WORD);
          } else {
            utmp32 = va_arg(vl, uint32_t);
            arg_type = (PRINT_ARG_TYPE_HEXADECIMAL_WORD);
            k--;
          }

          k++;
          p_uint32 = &utmp32;
          break;
        }
        default: {
          utmp32 = 0;
          p_uint32 = &utmp32;
          arg_type = (PRINT_ARG_TYPE_UNKNOWN);
          break;
        }
        }

        if (arg_type & (PRINT_ARG_TYPE_MASK_CHAR_STRING)) {
          getStr4NumMISC(arg_type, p_uint32, rstr);
          sprintUSART2(rstr);
        }
        k++;
      }
    } else { // not a '%' char -> print the char
      putcharUSART2(str[k]);
      if (str[k] == '\n')
        putcharUSART2('\r');
    }
    k++;
  }

  va_end(vl);
  return;
}

void sprintUSART2(uint8_t *str) {
  uint16_t k = 0;

  while (str[k] != '\0') {
    putcharUSART2(str[k]);
    if (str[k] == '\n')
      putcharUSART2('\r');
    k++;

    if (k == MAX_PRINT_STRING_SIZE)
      break;
  }
}

uint8_t getcharUSART2(void) { /// get one character from USART1
  uint8_t data;
  USART2->CR1 |= USART_CR1_UE | USART_CR1_RE; // enable usart	and Rx
  while ((USART2->SR & USART_SR_RXNE) != USART_SR_RXNE)
    ; // wait until data ready

  data = USART2->DR;              // send data
  USART2->SR &= ~(USART_SR_RXNE); // clear Rx data ready flag
  USART2->CR1 &= ~(USART_CR1_RE);
  return data;
}

void getrowUSART2(uint8_t *row) {
  uint8_t data = 0;
  int i = 0;
  USART2->CR1 |= USART_CR1_UE | USART_CR1_RE; // enable usart	and Rx
  while (i < 64) {
    while ((USART2->SR & USART_SR_RXNE) != USART_SR_RXNE)
      ; // wait until data ready

    data = USART2->DR;              // send data
    USART2->SR &= ~(USART_SR_RXNE); // clear Rx data ready flag

    row[i] = data;
    i++;
  }
  USART2->CR1 &= ~(USART_CR1_RE);
}

void init_DMA1(void) {
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
  init_DMA1_RX();
  init_DMA1_TX();
}

void init_DMA1_RX(void) {
  DMA1_Stream5->CR &= ~DMA_SxCR_EN;
  while (DMA1_Stream5->CR & DMA_SxCR_EN)
    ;

  DMA1_Stream5->CR &= 0xF0100000;

  DMA1_Stream5->PAR = (uint32_t) & (USART2->DR);
  // DMA1_Stream5->M0AR = (uint32_t)row;

  DMA1_Stream5->NDTR = 64;

  DMA1_Stream5->CR |= DMA_SxCR_CHSEL_2;
  DMA1_Stream5->CR |= DMA_SxCR_PL_1;

  // DMA1_Stream5->CR |= DMA_SxCR_DIR_0;
  DMA1_Stream6->CR |= DMA_SxCR_TCIE;
  DMA1_Stream5->CR &= ~DMA_SxCR_DIR;
  DMA1_Stream5->CR |= DMA_SxCR_MINC;

  DMA1_Stream5->CR &= ~DMA_SxCR_MSIZE;
  DMA1_Stream5->CR &= ~DMA_SxCR_PSIZE;

  NVIC_SetPriority(DMA1_Stream5_IRQn, 0); // mozda?
  NVIC_EnableIRQ(DMA1_Stream5_IRQn);      // mozda?
}

void init_DMA1_TX(void) {
  DMA1_Stream6->CR &= ~DMA_SxCR_EN;
  while (DMA1_Stream6->CR & DMA_SxCR_EN)
    ;

  DMA1_Stream6->CR &= 0xF0100000;

  DMA1_Stream6->PAR = (uint32_t) & (USART2->DR);
  DMA1_Stream5->M0AR = (uint32_t)data;

  DMA1_Stream6->NDTR = 192;

  DMA1_Stream6->CR |= DMA_SxCR_TCIE;
  DMA1_Stream6->CR |= DMA_SxCR_CHSEL_2;
  DMA1_Stream6->CR |= DMA_SxCR_PL_1;

  DMA1_Stream6->CR |= DMA_SxCR_DIR_0;
  DMA1_Stream6->CR |= DMA_SxCR_MINC;

  DMA1_Stream6->CR &= ~DMA_SxCR_MSIZE;
  DMA1_Stream6->CR &= ~DMA_SxCR_PSIZE;

  NVIC_SetPriority(DMA1_Stream6_IRQn, 0); // mozda?
  NVIC_EnableIRQ(DMA1_Stream6_IRQn);      // mozda?
}

void sendrowUSART2_DMA(pixel *row) {
  int i = 0, n;
  for (; i < 64; i++) {
    n = i * 3;
    data[n] = row[i].rgb.red;
    data[n + 1] = row[i].rgb.green;
    data[n + 2] = row[i].rgb.blue;
  }

  DMA1_Stream6->CR |= DMA_SxCR_EN;

  USART2->CR3 |= USART_CR3_DMAT;
  USART2->CR1 |= USART_CR1_UE | USART_CR1_TE;

  while (!(DMA1->HISR & DMA_HISR_TCIF6))
    ;

  // while (!(USART2->SR & USART_SR_TC))
  // ;

  DMA1->HIFCR |= DMA_HIFCR_CTCIF6;
  USART2->CR3 &= ~USART_CR3_DMAT;
  DMA1_Stream6->CR &= ~DMA_SxCR_EN;
}

void getrowUSART2_DMA(uint8_t *row) {
  DMA1_Stream5->M0AR = (uint32_t)row;
  DMA1_Stream5->CR |= DMA_SxCR_EN;

  USART2->CR3 |= USART_CR3_DMAR;
  USART2->CR1 |= USART_CR1_UE | USART_CR1_RE;

  while (!(DMA1->HISR & DMA_HISR_TCIF5))
    ;

  DMA1->HIFCR |= DMA_HIFCR_CTCIF5;
  USART2->CR3 &= ~USART_CR3_DMAR;
  DMA1_Stream5->CR &= ~DMA_SxCR_EN;
}

void DMA1_Stream5_IRQHandler(void) { DMA1->HIFCR |= DMA_HIFCR_CTCIF5; }
void DMA1_Stream6_IRQHandler(void) { DMA1->HIFCR |= DMA_HIFCR_CTCIF6; }
