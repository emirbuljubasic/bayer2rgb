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

uint8_t data[192];

void initUSART2(uint32_t baudrate);
void putcharUSART2(uint8_t data);
void printUSART2(char *str, ...);
void sprintUSART2(uint8_t *str);
uint8_t getcharUSART2(void);
void getrowUSART2(uint8_t *row);

// void initUSART2_DMA(uint8_t *row, uint16_t size);
// void getrowUSART2_DMA(uint8_t *row, uint16_t size);

void initUSART2_DMA(uint8_t *row);
void USART2_IRQHandler(void);
void getrowUSART2_DMA(uint8_t *row);

void init_DMA1(void);

void sendrowUSART2_DMA(pixel *row);

void init_DMA1_RX(void);
void init_DMA1_TX(void);

#endif
