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

extern uint16_t led_pin; 			/* PC13 */
extern uint16_t button_pin; 		/* PB14 */


void InitGPIO_LED(GPIO_InitTypeDef*);
void InitGPIO_BTN(GPIO_InitTypeDef*);
void InitEXTI_BTN(EXTI_InitTypeDef*, NVIC_InitTypeDef*);

#endif /* PERIPH_H_ */
