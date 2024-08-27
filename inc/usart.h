#ifndef __USART2_H_
#define __USART2_H_

#include "bayer.h"
#include "misc.h"
#include "stm32f4xx.h"
#include <stdarg.h>
#include <stdio.h>

#define USART2_BAUDRATE_921600 0x0000002D
#define USART2_BAUDRATE_460800 0x0000005B
#define USART2_BAUDRATE_115200 0x0000016C
#define USART2_BAUDRATE_9600 0x00001117

#define TX_BUFF_SIZE width * 3 + 1
#define RX_BUFF_SIZE width

// uint8_t data[width * 3 + 1];

void initUSART2(uint32_t baudrate);

void USART2_IRQHandler(void);

void init_DMA1(void);
void init_DMA1_RX(void);
void init_DMA1_TX(void);

void getrowUSART2_DMA(uint8_t *row);
void sendrowUSART2_DMA(uint8_t *row);

#endif
