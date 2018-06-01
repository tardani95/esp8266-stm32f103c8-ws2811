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
  * @brief  This function freeze the cpu for x [sec]
  * @param  time in [sec]
  * @retval None
  */
void delaySec(uint32_t sec){
	sec *= 5.15*1000000;
	for(uint32_t i = 0; i<sec;i++){
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
  * @brief  This function updates the lookup table
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
  * @param  GPIO_InitTypeDef variable address
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
  * @param  GPIO_InitTypeDef variable address
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
  * @param  GPIO_InitTypeDef variable address
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
  * @param  GPIO_InitTypeDef variable address
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
  * @param  GPIO_InitTypeDef variable address
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
  * @brief  This function initialize the gpio pins for the UART1 (PA9 - TX, PA10 - RX)
  * @param  GPIO_InitTypeDef variable
  * @retval None
  */
void InitGPIO_UART1(GPIO_InitTypeDef* GPIO_InitStructure){
	/* TX on PA9 */
	GPIO_StructInit(GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure->GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, GPIO_InitStructure);

	/* RX on PA10 */
	GPIO_StructInit(GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure->GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, GPIO_InitStructure);
}

/**
  * @brief  This function initialize the nested vectored interrupt controller for the DMA CH2 - TIM3_CH3
  * @param  NVIC_InitTypeDef variable
  * @retval None
  */
void InitNVIC_LSS1(NVIC_InitTypeDef* NVIC_InitStructure){
	/* LSS1 PB0 - TIM3 CH3 - DMA1 CH2*/
	NVIC_InitStructure->NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_InitStructure->NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure->NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure->NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(NVIC_InitStructure);
}

/**
  * @brief  This function initialize the nested vectored interrupt controller for the DMA CH3 - TIM3_CH4
  * @param  NVIC_InitTypeDef variable
  * @retval None
  */
void InitNVIC_LSS2(NVIC_InitTypeDef* NVIC_InitStructure){
	/* LSS1 PB1 - TIM3 CH4 - DMA1 CH3*/
	NVIC_InitStructure->NVIC_IRQChannel = DMA1_Channel3_IRQn;
	NVIC_InitStructure->NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure->NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure->NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(NVIC_InitStructure);
}

/**
  * @brief  This function initialize the nested vectored interrupt controller for the UART1 TX (PA9 - TX)
  * @param  NVIC_InitTypeDef variable
  * @retval None
  */
void InitNVIC_UART1_TX(NVIC_InitTypeDef* NVIC_InitStructure){
	/* USART1 RX*/
	NVIC_InitStructure->NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure->NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure->NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure->NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(NVIC_InitStructure);
}

/**
  * @brief  This function initialize the nested vectored interrupt controller for the UART1 RX (PA10 - RX)
  * @param  NVIC_InitTypeDef variable
  * @retval None
  */
void InitNVIC_UART1_RX(NVIC_InitTypeDef* NVIC_InitStructure){
	/* USART1 RX*/
	NVIC_InitStructure->NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure->NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure->NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure->NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(NVIC_InitStructure);
}

/**
  * @brief  This function initialize the DMA controller for the TIM3_CH3 CCR3 (PB0)
  * @param  DMA_InitTypeDef variable
  * @param  uint8_t array what to send
  * @retval None
  */
void InitDMA_CH2_TIM3_CH3(DMA_InitTypeDef* DMA_InitStructure, uint8_t* ledstrip_transmit_array){
	/* DMA 1, Channel 2 for TIM3 CH3 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel2);
	DMA_InitStructure->DMA_PeripheralBaseAddr = (uint32_t)&(TIM3->CCR3);
	DMA_InitStructure->DMA_MemoryBaseAddr = (uint32_t) ledstrip_transmit_array;
	DMA_InitStructure->DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure->DMA_BufferSize = 0;
	DMA_InitStructure->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure->DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure->DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure->DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure->DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure->DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, DMA_InitStructure);
}

/**
  * @brief  This function initialize the DMA controller for the TIM3_CH4 CCR4 (PB0)
  * @param  DMA_InitTypeDef variable
  * @param  uint8_t array what to send
  * @retval None
  */
void InitDMA_CH3_TIM3_CH4(DMA_InitTypeDef* DMA_InitStructure, uint8_t* ledstrip_transmit_array){
	/* DMA 1, Channel 3 for TIM3 CH4 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure->DMA_PeripheralBaseAddr = (uint32_t)&(TIM3->CCR4);
	DMA_InitStructure->DMA_MemoryBaseAddr = (uint32_t) ledstrip_transmit_array;
	DMA_InitStructure->DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure->DMA_BufferSize = 0;
	DMA_InitStructure->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure->DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure->DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure->DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure->DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure->DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, DMA_InitStructure);
}

/**
  * @brief  This function initialize the DMA controller for the UART1 TX (PA9 - TX)
  * @param  DMA_InitTypeDef variable
  * @param  uint8_t array what to send
  * @retval None
  */
void InitDMA_CH4_UART1_TX(DMA_InitTypeDef* DMA_InitStructure, uint8_t* usart_transmit_array){
	/* DMA 1, Channel 4 for USART1 TX */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure->DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
	DMA_InitStructure->DMA_MemoryBaseAddr = (uint32_t) usart_transmit_array;
	DMA_InitStructure->DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure->DMA_BufferSize = 0;
	DMA_InitStructure->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure->DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure->DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure->DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure->DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4, DMA_InitStructure);
}

/**
  * @brief  This function initialize the DMA controller for the UART1 RX (PA10 - RX)
  * @param  DMA_InitTypeDef variable
  * @param  uint8_t array where to receive
  * @retval None
  */
void InitDMA_CH5_UART1_RX(DMA_InitTypeDef* DMA_InitStructure, uint8_t* usart_receive_array){
	/* DMA 1, Channel 5 for USART1 RX */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel5);
	DMA_StructInit(DMA_InitStructure);
	DMA_InitStructure->DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
	DMA_InitStructure->DMA_MemoryBaseAddr = (uint32_t)usart_receive_array;
	DMA_InitStructure->DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure->DMA_BufferSize = 0;
	DMA_InitStructure->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure->DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure->DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure->DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure->DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5, DMA_InitStructure);
}

/**
  * @brief  This function initialize the UART1 settings
  * @param  USART_InitTypeDef variable
  * @retval None
  */
void InitUART1(USART_InitTypeDef* USART_InitStructure){
	/* USARTx configured as follow:
		  - BaudRate = 115200 baud
		  - Word Length = 8 Bits
		  - One Stop Bit
		  - No parity
		  - Hardware flow control disabled (RTS and CTS signals)
		  - Receive and transmit enabled
	*/
	/* deinitialize before use */
	USART_DeInit(USART1);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	USART_StructInit(USART_InitStructure);
	USART_InitStructure->USART_BaudRate = 115200;
	USART_InitStructure->USART_WordLength = USART_WordLength_8b;
	USART_InitStructure->USART_StopBits = USART_StopBits_1;
	USART_InitStructure->USART_Parity = USART_Parity_No;
	USART_InitStructure->USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure->USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,USART_InitStructure);
}

/**
  * @brief  This function initialize timer3 clock to a 400kHz frequency - 2.5us time constant
  * @param  TIM_TimeBaseInitTypeDef variable address
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
  * @param  TIM_OCInitTypeDef variable address
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
  * @param  TIM_TimeBaseInitTypeDef variable address
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
  * @param  TIM_OCInitTypeDef variable address
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
