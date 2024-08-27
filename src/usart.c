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
  USART2->CR1 = USART_CR1_UE | USART_CR1_TE;
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

  DMA1_Stream5->NDTR = RX_BUFF_SIZE;

  DMA1_Stream5->CR |= DMA_SxCR_CHSEL_2;
  DMA1_Stream5->CR |= DMA_SxCR_PL_1;

  // DMA1_Stream5->CR |= DMA_SxCR_DIR_0;
  DMA1_Stream5->CR |= DMA_SxCR_TCIE;
  DMA1_Stream5->CR &= ~DMA_SxCR_DIR;
  DMA1_Stream5->CR |= DMA_SxCR_MINC;

  DMA1_Stream5->CR &= ~DMA_SxCR_MSIZE;
  DMA1_Stream5->CR &= ~DMA_SxCR_PSIZE;

  // NVIC_SetPriority(DMA1_Stream5_IRQn, 0); // mozda?
  // NVIC_EnableIRQ(DMA1_Stream5_IRQn);      // mozda?
}

void init_DMA1_TX(void) {
  DMA1_Stream6->CR &= ~DMA_SxCR_EN;
  while (DMA1_Stream6->CR & DMA_SxCR_EN)
    ;

  DMA1_Stream6->CR &= 0xF0100000;

  DMA1_Stream6->PAR = (uint32_t) & (USART2->DR);
  // DMA1_Stream5->M0AR = (uint32_t)data;

  DMA1_Stream6->NDTR = TX_BUFF_SIZE;

  DMA1_Stream6->CR |= DMA_SxCR_CHSEL_2;
  DMA1_Stream6->CR |= DMA_SxCR_PL_1;

  DMA1_Stream6->CR |= DMA_SxCR_TCIE;
  DMA1_Stream6->CR |= DMA_SxCR_DIR_0;
  DMA1_Stream6->CR |= DMA_SxCR_MINC;

  DMA1_Stream6->CR &= ~DMA_SxCR_MSIZE;
  DMA1_Stream6->CR &= ~DMA_SxCR_PSIZE;

  // NVIC_SetPriority(DMA1_Stream6_IRQn, 0); // mozda?
  // NVIC_EnableIRQ(DMA1_Stream6_IRQn);      // mozda?
}

void sendrowUSART2_DMA(uint8_t *data) {
  DMA1_Stream6->M0AR = (uint32_t)data;
  DMA1_Stream6->CR |= DMA_SxCR_EN;

  USART2->CR3 |= USART_CR3_DMAT;
  USART2->CR1 |= USART_CR1_UE | USART_CR1_TE;

  while (!(DMA1->HISR & DMA_HISR_TCIF6))
    ;

  while (!(USART2->SR & USART_SR_TC))
    ;

  DMA1->HIFCR |= DMA_HIFCR_CTCIF6;
  USART2->CR3 &= ~USART_CR3_DMAT;
  USART2->CR1 &= ~USART_CR1_TE;
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
  USART2->CR1 &= ~USART_CR1_RE;
  DMA1_Stream5->CR &= ~DMA_SxCR_EN;
}

// void DMA1_Stream5_IRQHandler(void) { DMA1->HIFCR |= DMA_HIFCR_CTCIF5; }
// void DMA1_Stream6_IRQHandler(void) { DMA1->HIFCR |= DMA_HIFCR_CTCIF6; }
