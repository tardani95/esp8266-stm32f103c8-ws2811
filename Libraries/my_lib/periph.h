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
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"  /* reset and clock control */

#include "stm32f10x_tim.h"  /* timer header */

#include "stm32f10x_exti.h" /* external interrupt*/
#include "stm32f1xx_it.h" 	/* interrupt handler */

/* High level functions for NVIC and SysTick (add-on to CMSIS functions)
 * NVIC - Nested Vector Interrupt Controller
 */
#include "misc.h"
/*========================================================================*/
extern uint16_t led_pin; 			/* PC13 */
extern uint16_t button_pin; 		/* PB14 */
extern uint16_t ledstrip_signal; 	/* PB0  */
extern uint16_t pwm_exti_pin;		/* PA1  */

#define FAST_MODE 1
#define SYS_CLK 72 /* in MHz */
#define PWM_PERIOD 1.25 /* in usec */

#define TIM_PERIOD 89 /* 89 = (sys_clk * pwm_period)-1 */
#define TIM_PRESCALER 0
#define LED_NUMBER 50

void InitGPIO_LED(GPIO_InitTypeDef*);
void InitGPIO_BTN(GPIO_InitTypeDef*);
void InitGPIO_LSS(GPIO_InitTypeDef*);
void InitGPIO_PWM_EXTI(GPIO_InitTypeDef*);

void InitTIM3_CLK(TIM_TimeBaseInitTypeDef*);
void InitTIM3_CH3_PWM(TIM_OCInitTypeDef*);
void InitTIM2_CLK(TIM_TimeBaseInitTypeDef*);
void InitTIM2_CH2_PWM(TIM_OCInitTypeDef*);

void InitEXTI_BTN(EXTI_InitTypeDef*, NVIC_InitTypeDef*);
void InitEXTI_TIM3_PWM(EXTI_InitTypeDef*, NVIC_InitTypeDef*);
void InitEXTI_TIM2_PWM(EXTI_InitTypeDef*, NVIC_InitTypeDef*);

#endif /* PERIPH_H_ */
