/*
******************************************************************************
File        : esp8266.c
Author      : Daniel Tar
Version     :
Copyright   :
Description : esp8266 (ESP-01) library for STM32F10x
Info        : 31.05.2018
*******************************************************************************
*/

#ifndef LIBRARIES_MY_LIB_ESP8266_H_
#define LIBRARIES_MY_LIB_ESP8266_H_

/*========================================================================*/
/*                             INCLUDES									  */
/*========================================================================*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"  /* general purpose input output */
#include "stm32f10x_rcc.h"   /* reset and clock control */
#include "stm32f1xx_it.h" 	 /* interrupt handler  */
#include "stm32f10x_usart.h" /* universal synchronous asynchronous receiver transmitter */
#include "stm32f10x_dma.h"   /* direct memory access */

/* High level functions for NVIC and SysTick (add-on to CMSIS functions)
 * NVIC - Nested Vector Interrupt Controller
 */
#include "misc.h"
/*========================================================================*/

void InitGPIO_UART1(GPIO_InitTypeDef*);
void InitNVIC_UART1(NVIC_InitTypeDef*);
void InitDMA_CH4_UART1_TX(DMA_InitTypeDef*,uint8_t*);
void InitDMA_CH5_UART1_RX(DMA_InitTypeDef*,uint8_t*);
void InitUART1(USART_InitTypeDef*);

void InitESP8266(GPIO_InitTypeDef*,NVIC_InitTypeDef*,DMA_InitTypeDef*,uint8_t*,DMA_InitTypeDef*,uint8_t*,USART_InitTypeDef*);

#endif /* LIBRARIES_MY_LIB_ESP8266_H_ */
