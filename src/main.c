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
#include "stm32f10x_rcc.h" //reset and clock control

/* Private typedef */
/* Private define  */



/* Private macro */
/* Private variables */



/* Private function prototypes */
/* Private functions */



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
//	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure; // timer init
//	TIM_OCInitTypeDef TIM_OC_InitStructure; // output compare init


	/* enable the clock on the advanced peripheral buses - see the datasheet on page 11 */
	/* pc13 - led output
	 * pb0  - pwm output
	 * pb13 - button input
	 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);

	/* led gpio init */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* pwm gpio init*/
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* button init */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*timer init*/
	//	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//	TIM_TimeBase_InitStructure.TIM_Period = 999;
	//	TIM_TimeBase_InitStructure.TIM_Prescaler = 71;
	//	TIM_TimeBaseInit(TIM1, &TIM_TimeBase_InitStructure);
	//
	//	TIM_OC_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//	TIM_OC_InitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	//	TIM_OC_InitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	//	TIM_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//	TIM_OC_InitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	//	TIM_OC_InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//	TIM_OC_InitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	//	TIM_OC_InitStructure.TIM_Pulse = 100;
	//
	//	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	//	TIM_OC2Init(TIM1, &TIM_OC_InitStructure);
	//	TIM_Cmd(TIM1, ENABLE);
	//
	//
	//	//timer2 ch2
	//	TIM_OC_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//	TIM_OC_InitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	//	TIM_OC_InitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	//	TIM_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//	TIM_OC_InitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	//	TIM_OC_InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//	TIM_OC_InitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	//	TIM_OC_InitStructure.TIM_Pulse = 500;
	//
	//	TIM_OC2Init(TIM2, &TIM_OC_InitStructure);
	//
	//	TIM_Cmd(TIM2, ENABLE);
	//
	//	//timer3 ch1
	//	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//	TIM_TimeBase_InitStructure.TIM_Period = 1499;
	//	TIM_TimeBase_InitStructure.TIM_Prescaler = 71;
	//	TIM_TimeBaseInit(TIM3, &TIM_TimeBase_InitStructure);
	//
	//	TIM_OC_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//	TIM_OC_InitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	//	TIM_OC_InitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	//	TIM_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//	TIM_OC_InitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	//	TIM_OC_InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//	TIM_OC_InitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	//	TIM_OC_InitStructure.TIM_Pulse = 100;
	//	TIM_OC1Init(TIM3, &TIM_OC_InitStructure);
	//
	//	//timer3 ch2
	//	TIM_OC_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//	TIM_OC_InitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	//	TIM_OC_InitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	//	TIM_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//	TIM_OC_InitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	//	TIM_OC_InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//	TIM_OC_InitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	//	TIM_OC_InitStructure.TIM_Pulse = 500;
	//	TIM_OC2Init(TIM3, &TIM_OC_InitStructure);
	//
	//	TIM_Cmd(TIM3, ENABLE);
	//
	//	//timer4 ch1
	//	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//	TIM_TimeBase_InitStructure.TIM_Period = 4999;
	//	TIM_TimeBase_InitStructure.TIM_Prescaler = 71;
	//	TIM_TimeBaseInit(TIM4, &TIM_TimeBase_InitStructure);
	//
	//	TIM_OC_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//	TIM_OC_InitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	//	TIM_OC_InitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	//	TIM_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//	TIM_OC_InitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	//	TIM_OC_InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//	TIM_OC_InitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	//	TIM_OC_InitStructure.TIM_Pulse = 100;
	//	TIM_OC1Init(TIM4, &TIM_OC_InitStructure);
	//
	//	//timer4 ch2
	//	TIM_OC_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//	TIM_OC_InitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	//	TIM_OC_InitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	//	TIM_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//	TIM_OC_InitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	//	TIM_OC_InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//	TIM_OC_InitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	//	TIM_OC_InitStructure.TIM_Pulse = 500;
	//	TIM_OC2Init(TIM4, &TIM_OC_InitStructure);
	//
	//	TIM_Cmd(TIM4, ENABLE);

	uint8_t readValue = 1;

	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);

	while(1)
	{
		/*
		 * if you write 0 to the ledPin the it lights up
		 * the button is wired with input pull up, so if you press the button the you read the value 0
		 */
		readValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,readValue);

	}
  return 0;
}

