/*
******************************************************************************
File        : ws2811.c
Author      : Daniel Tar - ideas based on Peter Vass implementation
Version     :
Copyright   :
Description : ws2811 library for stm32f103c8 microcontroller
Info        : 16.06.2018
*******************************************************************************
*/

/******************************************************************************/
/*                               Includes									  */
/******************************************************************************/
#include "ws2811.h"


/******************************************************************************/
/*                          Private variables                                 */
/******************************************************************************/

/* gamma correction table - by Peter Vass */
const uint8_t gammaCorrectionTable[] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
        2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
        5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
       10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
       17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
       25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
       37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
       51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
       69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
       90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
      115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
      144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
      177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
      215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };


__IO uint8_t TIMx_OC_DMA_Buffer[DMA_BUFFER_SIZE]; /* DMA buffer for TIMx OutputCompare (OC) values */

__IO uint32_t pixel_colorRGB[PARALELL_STRIPS][LED_STRIP_SIZE];  /* array to store all the pixel colors */

__IO uint16_t pixel_id = 0; /* current processed led 3s */


/******************************************************************************/
/*                    Callback function declaration                           */
/******************************************************************************/



/******************************************************************************/
/*                          Function definitions                              */
/******************************************************************************/

/**
  * @brief  This function initialize the led strip signals on PA7, PB0, PB1
  * @param  GPIO_InitTypeDef variable address
  * @retval None
  */
void InitGPIO_LSSs(GPIO_InitTypeDef* GPIO_InitStructure){

	uint16_t ledstrip_signal1 = GPIO_Pin_7; 	/* PA7  */
	uint16_t ledstrip_signal2 = GPIO_Pin_0; 	/* PB0  */
	uint16_t ledstrip_signal3 = GPIO_Pin_1; 	/* PB1  */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	/* PA7  */
	GPIO_InitStructure->GPIO_Pin   = ledstrip_signal1;
	GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, GPIO_InitStructure); /* !GPIOA! */

	/* PB0 & PB1  */
	GPIO_InitStructure->GPIO_Pin   = ledstrip_signal2 | ledstrip_signal3;
	GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, GPIO_InitStructure); /* !GPIOB! */

}

/**
  * @brief  This function initialize the nested vectored interrupt controller for the DMA CH3 - TIM3
  * @param  NVIC_InitTypeDef variable
  * @retval None
  */
void InitNVIC_LSS(NVIC_InitTypeDef* NVIC_InitStructure){
	/* DMA1 CH3 - TIM3 OC*/
	NVIC_InitStructure->NVIC_IRQChannel = DMA1_Channel3_IRQn;
	NVIC_InitStructure->NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure->NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure->NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(NVIC_InitStructure);
}

/**
  * @brief  This function initialize TIM3 clock to a 800/400kHz frequency - 1.25/2.5us time constant
  *         depends on the prescaler value
  * @param  TIM_TimeBaseInitTypeDef variable address
  * @retval None
  */
void InitTIM3_CLK(TIM_TimeBaseInitTypeDef* TIM_TimeBase_InitStructure){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructInit(TIM_TimeBase_InitStructure);

	TIM_TimeBase_InitStructure->TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure->TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure->TIM_Period = TIM_PERIOD-1;
	TIM_TimeBase_InitStructure->TIM_Prescaler = TIM_PRESCALER; // divider = prescaler + 1
	TIM_TimeBaseInit(TIM3, TIM_TimeBase_InitStructure);
}

/**
  * @brief  This function initialize TIM3 output compare mode to pwm1 on ch2, ch3, ch4
  * @param  TIM_OCInitTypeDef variable address
  * @retval None
  */
void InitTIM3_PWM(TIM_OCInitTypeDef* TIM_OC_InitStructure){
	TIM_OCStructInit(TIM_OC_InitStructure);

	TIM_OC_InitStructure->TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OC_InitStructure->TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC_InitStructure->TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC_InitStructure->TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC_InitStructure->TIM_Pulse = 0x0000;

	TIM_OC2Init(TIM3, TIM_OC_InitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_OC3Init(TIM3, TIM_OC_InitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_OC4Init(TIM3, TIM_OC_InitStructure);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* Enable the TIM Counter */
	TIM_Cmd(TIM3, ENABLE);
}


/**
  * @brief  This function initialize the DMA controller for the TIM3 OC values
  * 		from CH2 to CH4 (PA7 PB0 PB1)
  * 		Initialize TIMx (TIM3) first
  * @param  DMA_InitTypeDef variable
  * @param  ledstrip_transmit_dma_buffer - uint8_t[] buffer to send
  * @retval None
  */
void InitDMA_CH3_TIM3_CHs(DMA_InitTypeDef* DMA_InitStructure,uint8_t* ledstrip_transmit_dma_buffer){
	/* DMA 1, Channel 2 for TIM3 CH4 */

	/* using TIM DMA burst feature page 421 in Reference Manual 02-06-2018 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	/* DMA ch3 receives the TIM3 Update event - page 281 in RM 02-06-2018 */
	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure->DMA_PeripheralBaseAddr = (uint32_t)&(TIM3->DMAR);
	DMA_InitStructure->DMA_MemoryBaseAddr = (uint32_t) ledstrip_transmit_array;
	DMA_InitStructure->DMA_DIR = DMA_DIR_PeripheralDST;
	/*
	 * 24 bits need to determine the color of one led
	 * 3  there are 3 strips (ch2,ch3,ch4)
	 */
	DMA_InitStructure->DMA_BufferSize = DMA_BUFFER_SIZE;
	DMA_InitStructure->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure->DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure->DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure->DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure->DMA_Priority = DMA_Priority_High;
	DMA_InitStructure->DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, DMA_InitStructure);

	TIM_DMAConfig(TIM3, TIM_DMABase_CCR2, TIM_DMABurstLength_3Transfers);
	TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);

}





/* interval: [ fromCH ; toCh ) */
void Init_WS2811(uint8_t* ptr_command_array, uint8_t command_array_size){

}

uint8_t gammaCorrection(uint8_t color){

}
void refreshLedStrip(void){

}

/* dma buffer handling*/
void DMA1_Channel3_IRQHandler(void){

}
