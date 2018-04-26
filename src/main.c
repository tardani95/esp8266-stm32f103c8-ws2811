/*
******************************************************************************
File        : main.c
Author      : Daniel Tar
Version     : 1.0
Copyright   :
Description : WS2811
Info        : 2018-04-09
******************************************************************************
*/

/* Includes */
#include <stddef.h>
#include <stdio.h>
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

#include "periph.h"


/* Private typedef */
/* Private define  */


/* Private macro */
/* Private variables */
//uint16_t led_pin = GPIO_Pin_13; /* on port C*/
//uint16_t button_pin = GPIO_Pin_14; /* on port B*/
//uint16_t pwm_pin = GPIO_Pin_0;
//extern uint16_t repetition_counter = 0;

/* Private function prototypes */
/* Private functions */
void delayMicroSec(uint32_t us){
	us *= 5.15;
	for(uint32_t i = 0; i<us;i++){
	}
}


/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/

/**
 * Hinweise:
 * Bei den Timern 1, 8, 15, 16 und 17 müssen die PWM Ausgänge zusätzlich mit der Funktion TIM_CtrlPWMOutputs(...) aktiviert werden.
 * Nur, falls der Timer aktiviert, aber die PWM-Outputs deaktiviert sind kommen die Einstellungen zum Idle State zum Tragen.
 * Die OCN Ausgänge gibt es allgemein nur bei den Timern 1, 8, 15, 16 und 17.
 */

int main(void)
{

	SystemInit();

	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure; /* external interrupt init */
	NVIC_InitTypeDef NVIC_InitStructure; /* nested vector interrupt controller init */
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure; // timer init
	TIM_OCInitTypeDef TIM_OC_InitStructure; // output compare init

	InitLookUpTable();


	/* GPIO INIT */

	/* built-in led init */
	InitGPIO_LED(&GPIO_InitStructure);
	/*switch off the led by default*/
	GPIO_WriteBit(GPIOC,led_pin,Bit_SET);

	/* button init */
	InitGPIO_BTN(&GPIO_InitStructure);

	/* ledstrip signal pin init*/
	InitGPIO_LSS(&GPIO_InitStructure);

	/* pwm for external interrupt register init*/
	InitGPIO_PWM_EXTI(&GPIO_InitStructure);



	/* TIMER INIT */

	/* tim2 clock init */
	InitTIM2_CLK(&TIM_TimeBase_InitStructure);

	/* tim2 ch2 pwm inti */
	InitTIM2_CH2_PWM(&TIM_OC_InitStructure);

	/* tim3 clock init */
	InitTIM3_CLK(&TIM_TimeBase_InitStructure);

	/* tim3 ch3 pwm init */
	InitTIM3_CH3_PWM(&TIM_OC_InitStructure);



	/* INTERRUPT INIT */
	/* the interrupt handlers found in stm32f1xx_it.c */

	/* button interrupt init */
	InitEXTI_BTN(&EXTI_InitStructure, &NVIC_InitStructure);

	/* pwm interrupt capture */
	InitEXTI_TIM3_PWM(&EXTI_InitStructure, &NVIC_InitStructure);
	InitEXTI_TIM2_PWM(&EXTI_InitStructure, &NVIC_InitStructure);




	delayMicroSec(500);
	TIM3->CCR3=look_up_table_2[0] ? 43 : 18;
	while(1){
		for(uint8_t i=0;i<7;++i){
			for(uint16_t temp = 0; temp<256; temp+=1){
				switch(i){
					case 0: RefreshLookUpTable(temp,0,0); break;
					case 1: RefreshLookUpTable(0,temp,0); break;
					case 2: RefreshLookUpTable(0,0,temp); break;
					case 3: RefreshLookUpTable(temp,temp,0); break;
					case 4: RefreshLookUpTable(0,temp,temp); break;
					case 5: RefreshLookUpTable(temp,0,temp); break;
					case 6: RefreshLookUpTable(temp,temp,temp); break;
					default: break;
				}
				delayMicroSec(2000);
			}
			delayMicroSec(1000);
			/* watch out! negative overflow! --> int16_t */
			for(int16_t temp = 255; temp>=0; temp-=1){
				switch(i){
					case 0: RefreshLookUpTable(temp,0,0); break;
					case 1: RefreshLookUpTable(0,temp,0); break;
					case 2: RefreshLookUpTable(0,0,temp); break;
					case 3: RefreshLookUpTable(temp,temp,0); break;
					case 4: RefreshLookUpTable(0,temp,temp); break;
					case 5: RefreshLookUpTable(temp,0,temp); break;
					case 6: RefreshLookUpTable(temp,temp,temp); break;
					default: break;
				}
				delayMicroSec(2000);
			}
		}
	}
}

