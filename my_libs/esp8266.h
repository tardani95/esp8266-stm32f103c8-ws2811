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

#ifndef _ESP8266_H_
#define _ESP8266_H_

/*========================================================================*/
/*                             INCLUDES									  */
/*========================================================================*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"  /* general purpose input output */
#include "stm32f10x_rcc.h"   /* reset and clock control */
#include "stm32f10x_usart.h" /* universal synchronous asynchronous receiver transmitter */
#include "stm32f10x_dma.h"   /* direct memory access */

/* High level functions for NVIC and SysTick (add-on to CMSIS functions)
 * NVIC - Nested Vector Interrupt Controller
 */
#include "misc.h"

#include "util.h" /* my lib */
/*========================================================================*/

/* Private define  */
#define UART_BUFFER_SIZE 20

void InitGPIO_UART1(GPIO_InitTypeDef*);
void InitNVIC_UART1(NVIC_InitTypeDef*);
void InitDMA_CH4_UART1_TX(DMA_InitTypeDef*,uint8_t*);
void InitDMA_CH5_UART1_RX(DMA_InitTypeDef*,uint8_t*);
void InitUART1(USART_InitTypeDef*);

void InitESP8266(uint8_t* uart_receive_array);
void ClearRX_DMA_Buffer(void);
void SendDataToESP8266(uint8_t /*, callback*/);
void StartUDPReceiving(uint8_t);
void StartUDPReceivingWithCallback(uint8_t, callback);


/* DMA request handler for USART1_TX */
void DMA1_Channel4_IRQHandler(void);
/* DMA request handler for USART1_RX */
void DMA1_Channel5_IRQHandler(void);

#endif /* _ESP8266_H_ */
