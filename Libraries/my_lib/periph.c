/*
 * periph.c
 *
 *  Created on: Apr 25, 2018
 *      Author: tar
 */

/******************************************************************************/
/*                               Includes									  */
/******************************************************************************/
#include "periph.h"

/******************************************************************************/
/*                               Variables									  */
/******************************************************************************/
uint16_t led_pin	= GPIO_Pin_13; 		/* PC13 */
uint16_t button_pin = GPIO_Pin_14; 		/* PB14 */
uint16_t ledstrip_signal1 = GPIO_Pin_0; 	/* PB0  */
uint16_t ledstrip_signal2 = GPIO_Pin_1; 	/* PB1  */
uint16_t pwm_exti_pin 	= GPIO_Pin_1;	/* PA1  */
uint8_t  look_up_table_2[LOOK_UP_TABLE_SIZE];

/******************************************************************************/
/*            			         Functions                                    */
/******************************************************************************/

/**
  * @brief  This function freeze the cpu for x [us]
  * @param  time in [us]
  * @retval None
  */
void delayMicroSec(uint32_t us){
	us *= 5.15;
	for(uint32_t i = 0; i<us;i++){
	}
}

/**
  * @brief  This function initialize the lookup table - it is in BRG dimension
  * @param  None
  * @retval None
  */
void InitLookUpTable(void){
	uint8_t look_up_table_1[4][3]={{0,200,0},{125,0,125},{0,0,255},{125,125,0}};
	for(uint16_t i=0;i<4;++i){
		for(uint16_t j=0;j<3;++j){
			for(uint16_t k = 0; k<8; ++k){
				look_up_table_2[i*24 + j*8 + k] = look_up_table_1[i][j] & (0x80 >> k);
			}
		}
	}
}

/**
  * @brief  This function updates the lookup table - it is in BRG dimension
  * @param  R: Red value in range 0-255
  * @param  G: Green value in range 0-255
  * @param  B: Blue value in range 0-255
  * @retval None
  */
void RefreshLookUpTable(uint8_t R, uint8_t G, uint8_t B){
	uint8_t look_up_table_1[4][3]={{B,R,G},{B,R,G},{B,R,G},{B,R,G}};
	for(uint16_t i=0;i<4;++i){
		for(uint16_t j=0;j<3;++j){
			for(uint16_t k = 0; k<8; ++k){
				look_up_table_2[i*24 + j*8 + k] = look_up_table_1[i][j] & (0x80 >> k);
			}
		}
	}
}

/**
  * @brief  This function modifies the lookup table so, that it makes a fade in fade out animation
  * @param  fade_in_time: it will fade in in 255*fade_in_time [us]
  * @param  hold_time: hold the color at the same value for hold_time [us]
  * @param  fade_out_time:it will fade out in 255*fade_out_time [us]
  * @retval None
  */
void AnimFadeInFadeOut(uint16_t fade_in_time, uint16_t hold_time, uint16_t fade_out_time){
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
			delayMicroSec(fade_in_time);

		}
		delayMicroSec(hold_time);
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
			delayMicroSec(fade_out_time);

		}
	}
}

/**
  * @brief  This function initialize the LED on PC13 pin
  * @param  None
  * @retval None
  */
void InitGPIO_LED(GPIO_InitTypeDef* GPIO_InitStructure){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_StructInit(GPIO_InitStructure);

	GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure->GPIO_Pin   = led_pin;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, GPIO_InitStructure);
}

/**
  * @brief  This function initialize the button on PB14 pin
  * @param  None
  * @retval None
  */
void InitGPIO_BTN(GPIO_InitTypeDef* GPIO_InitStructure){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_StructInit(GPIO_InitStructure);

	GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure->GPIO_Pin   = button_pin;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, GPIO_InitStructure);
}

/**
  * @brief  This function initialize the led strip signal 1 on PB0
  * @param  None
  * @retval None
  */
void InitGPIO_LSS1(GPIO_InitTypeDef* GPIO_InitStructure){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_StructInit(GPIO_InitStructure);

	GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure->GPIO_Pin   = ledstrip_signal1;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, GPIO_InitStructure);
}

/**
  * @brief  This function initialize the led strip signal 2 on PB1
  * @param  None
  * @retval None
  */
void InitGPIO_LSS2(GPIO_InitTypeDef* GPIO_InitStructure){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_StructInit(GPIO_InitStructure);

	GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure->GPIO_Pin   = ledstrip_signal2;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, GPIO_InitStructure);
}

/**
  * @brief  This function initialize the gpio pin on PA1 for pwm function for the external interrupt
  * @param  None
  * @retval None
  */
void InitGPIO_PWM_EXTI(GPIO_InitTypeDef* GPIO_InitStructure){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_StructInit(GPIO_InitStructure);

	GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure->GPIO_Pin   = pwm_exti_pin;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, GPIO_InitStructure);
}

/**
  * @brief  This function initialize the gpio pin on PA1 for pwm function for the external interrupt
  * @param  None
  * @retval None
  */
void InitGPIO_UART1(GPIO_InitTypeDef* GPIO_InitStructure){

}

/**
  * @brief  This function initialize timer3 clock to a 400kHz frequency - 2.5us time constant
  * @param  None
  * @retval None
  */
void InitTIM3_CLK(TIM_TimeBaseInitTypeDef* TIM_TimeBase_InitStructure){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructInit(TIM_TimeBase_InitStructure);

	TIM_TimeBase_InitStructure->TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure->TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure->TIM_Period = TIM_PERIOD; 	// period = period + 1
	TIM_TimeBase_InitStructure->TIM_Prescaler = TIM_PRESCALER; 	// divider = prescaler + 1
	TIM_TimeBaseInit(TIM3, TIM_TimeBase_InitStructure);
}

/**
  * @brief  This function initialize timer3 output compare mode to pwm1 on ch3
  * @param  None
  * @retval None
  */
void InitTIM3_CH3_CH4_PWM(TIM_OCInitTypeDef* TIM_OC_InitStructure){
	TIM_OCStructInit(TIM_OC_InitStructure);

	TIM_OC_InitStructure->TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OC_InitStructure->TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC_InitStructure->TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC_InitStructure->TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC_InitStructure->TIM_Pulse = 0x0000;

	TIM_OC3Init(TIM3, TIM_OC_InitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_OC4Init(TIM3, TIM_OC_InitStructure);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* Enable the TIM Counter */
	TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  This function initialize timer2 clock to a 400kHz frequency - 2.5us time constant
  * @param  None
  * @retval None
  */
void InitTIM2_CLK(TIM_TimeBaseInitTypeDef* TIM_TimeBase_InitStructure){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseStructInit(TIM_TimeBase_InitStructure);

	TIM_TimeBase_InitStructure->TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure->TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure->TIM_Period = TIM_PERIOD; 	// period = period + 1
	TIM_TimeBase_InitStructure->TIM_Prescaler = TIM_PRESCALER; 	// divider = prescaler + 1
	TIM_TimeBaseInit(TIM2, TIM_TimeBase_InitStructure);
}

/**
  * @brief  This function initialize timer2 output compare mode to pwm1 on ch2
  * @param  None
  * @retval None
  */
void InitTIM2_CH2_PWM(TIM_OCInitTypeDef* TIM_OC_InitStructure){
	TIM_OCStructInit(TIM_OC_InitStructure);

	TIM_OC_InitStructure->TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OC_InitStructure->TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC_InitStructure->TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC_InitStructure->TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC_InitStructure->TIM_Pulse = 0x0000;

	TIM_OC2Init(TIM2, TIM_OC_InitStructure);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

	/* Enable the TIM Counter */
	TIM_Cmd(TIM2, ENABLE);
}

/**
  * @brief  This function initialize the external interrupt on PB14 and sets the nested vectored interrupt controller
  * @param  EXTI_InitStructure: pointer to a EXTI_InitTypeDef structure which will
  *         be initialized.
  *         NVIC_InitStructure: pointer to a NVIC_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void InitEXTI_BTN(EXTI_InitTypeDef* EXTI_InitStructure, NVIC_InitTypeDef* NVIC_InitStructure){
	EXTI_StructInit(EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);

	EXTI_InitStructure->EXTI_Line = EXTI_Line14;
	EXTI_InitStructure->EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure->EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure->EXTI_LineCmd = ENABLE;
	EXTI_Init(EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	/* NVIC_PriorityGroup_4: 4 bits for pre-emption priority
	 *                       0 bits for subpriority*
	 */

	NVIC_InitStructure->NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure->NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure->NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure->NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(NVIC_InitStructure);
}

/**
  * @brief  This function initialize the external interrupt on PB0 (ledstrip signal pin) and sets the nested vectored interrupt controller
  * @param  EXTI_InitStructure: pointer to a EXTI_InitTypeDef structure which will
  *         be initialized.
  *         NVIC_InitStructure: pointer to a NVIC_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void InitEXTI_TIM3_PWM(EXTI_InitTypeDef* EXTI_InitStructure, NVIC_InitTypeDef* NVIC_InitStructure){
	EXTI_StructInit(EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);

	EXTI_InitStructure->EXTI_Line = EXTI_Line0;
	EXTI_InitStructure->EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure->EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure->EXTI_LineCmd = ENABLE;
	EXTI_Init(EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	NVIC_InitStructure->NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure->NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure->NVIC_IRQChannelPreemptionPriority = 0x06;
	NVIC_InitStructure->NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(NVIC_InitStructure);
}

/**
  * @brief  This function initialize the external interrupt on PA1 (tim2 ch2 pwm) and sets the nested vectored interrupt controller
  * @param  EXTI_InitStructure: pointer to a EXTI_InitTypeDef structure which will
  *         be initialized.
  *         NVIC_InitStructure: pointer to a NVIC_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void InitEXTI_TIM2_PWM(EXTI_InitTypeDef* EXTI_InitStructure, NVIC_InitTypeDef* NVIC_InitStructure){
	EXTI_StructInit(EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);

	EXTI_InitStructure->EXTI_Line = EXTI_Line1;
	EXTI_InitStructure->EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure->EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure->EXTI_LineCmd = ENABLE;
	EXTI_Init(EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	NVIC_InitStructure->NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure->NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure->NVIC_IRQChannelPreemptionPriority = 0x07;
	NVIC_InitStructure->NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(NVIC_InitStructure);
}
