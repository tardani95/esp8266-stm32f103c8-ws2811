/*
 * periph.h
 *
 *  Created on: Apr 25, 2018
 *      Author: tar
 */

#ifndef PERIPH_H_
#define PERIPH_H_

/*========================================================================*/
/*                             INCLUDES									  */
/*========================================================================*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"  /* general purpose input output */
#include "stm32f10x_rcc.h"   /* reset and clock control */

//#include "stm32f10x_tim.h"   /* timer header */

#include "stm32f10x_exti.h"  /* external interrupt */
#include "stm32f1xx_it.h" 	 /* interrupt handler  */
//#include "stm32f10x_usart.h" /* universal synchronous asynchronous receiver transmitter */

//#include "stm32f10x_dma.h"   /* direct memory access */

/* High level functions for NVIC and SysTick (add-on to CMSIS functions)
 * NVIC - Nested Vector Interrupt Controller
 */
#include "misc.h"
/*========================================================================*/

/* EEPROM DATA */
/* Variables' number */
/* 3x offset,length*/
#define NumbOfVar               ((uint8_t)0x06)

/* standard mode to color palettes - change to 1 for rgb color mode*/
#define STD_MODE 0

extern uint16_t led_pin; 			/* PC13 */
extern uint16_t button_pin; 		/* PB14 */

void Init_EEPROM();
void InitGPIO_LED(GPIO_InitTypeDef*);
void InitGPIO_BTN(GPIO_InitTypeDef*);
void InitEXTI_BTN(EXTI_InitTypeDef*, NVIC_InitTypeDef*);
void EXTI15_10_IRQHandler(void);

#endif /* PERIPH_H_ */
