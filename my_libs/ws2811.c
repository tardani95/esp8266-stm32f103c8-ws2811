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

#define DEBUG_PINS_ON
#define DEBUG_DMA_BUFFER_FILL_UP
//#define DEBUG_DMA_BUFFER_IRQ
#define DEBUG_DMA_BUFFER_HT
#define DEBUG_DMA_BUFFER_TC

/******************************************************************************/
/*                          Private typedef                                   */
/******************************************************************************/
typedef struct RGB{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}RGB_Pixel;
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


__IO uint8_t TIMx_OC_DMA_Buffer_BRG[DMA_BUFFER_SIZE]; /* DMA buffer for TIMx OutputCompare (OC) values */

__IO RGB_Pixel pixel_map[PARALELL_STRIPS][LED_STRIP_SIZE];  /* array to store all the pixel colors */

__IO uint16_t HT_counter = 0; /* current processed half buffer */
__IO uint16_t pixel_id = 0; /* current processed half buffer */



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

#ifdef DEBUG_PINS_ON
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* PC14 & PC15  */
	GPIO_InitStructure->GPIO_Pin   = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, GPIO_InitStructure); /* !GPIOC! */

	GPIO_ResetBits(GPIOC,GPIO_Pin_14 | GPIO_Pin_15);
#endif

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
	/* TIM_Cmd(TIM3, ENABLE); */
}


/**
  * @brief  This function initialize the DMA controller for the TIM3 OC values
  * 		from CH2 to CH4 (PA7 PB0 PB1)
  * 		Initialize TIMx (TIM3) first
  * @param  DMA_InitTypeDef variable
  * @param  ledstrip_transmit_dma_buffer - uint8_t[] buffer to send
  * @retval None
  */
void InitDMA_CH3_TIM3_CHs(DMA_InitTypeDef* DMA_InitStructure, uint8_t* ledstrip_transmit_dma_buffer){
	/* DMA 1, Channel 2 for TIM3 CH4 */

	/* using TIM DMA burst feature page 421 in Reference Manual 02-06-2018 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	/* DMA ch3 receives the TIM3 Update event - page 281 in RM 02-06-2018 */
	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure->DMA_PeripheralBaseAddr = (uint32_t)&(TIM3->DMAR);
	DMA_InitStructure->DMA_MemoryBaseAddr = (uint32_t) ledstrip_transmit_dma_buffer;
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
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE); /* transmission complete */
	DMA_ITConfig(DMA1_Channel3, DMA_IT_HT, ENABLE); /* half transfer complete */
}

void Init_WS2811(uint8_t * ptr_command_array, uint8_t command_array_size){

	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; /* nested vector interrupt controller init */
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure; /* timer init */
	TIM_OCInitTypeDef TIM_OC_InitStructure; /* output compare init */
	DMA_InitTypeDef DMA_InitStructure;		/* dma init  */

	/* ledstrip signal pin init*/
	InitGPIO_LSSs(&GPIO_InitStructure);
	InitNVIC_LSS(&NVIC_InitStructure);

	/* tim3 clock init */
	InitTIM3_CLK(&TIM_TimeBase_InitStructure);
	InitTIM3_PWM(&TIM_OC_InitStructure);
	InitDMA_CH3_TIM3_CHs(&DMA_InitStructure, (uint8_t*) TIMx_OC_DMA_Buffer_BRG ); /* look_up_table_1 */

}

uint8_t gammaCorrection(uint8_t color){
	return gammaCorrectionTable[color];
}

void Init_PixelMap(void){
	/* @TODO brg  - change to rgb */
//	uint8_t init_color[4][3]={{0,255,0},{125,0,125},{0,0,255},{125,125,0}};
	uint8_t init_color[4][3]={{255,0,0},{0,255,0},{0,0,255},{125,0,125}};
	for( uint16_t i = 0 ; i < LED_STRIP_SIZE ; ++i ){
		for( uint8_t j = 0; j < PARALELL_STRIPS; ++j ){
			pixel_map[j][i].r = init_color[i%4][0]; // gammaCorrection(init_color[i%4][0]);
			pixel_map[j][i].g = init_color[i%4][1]; // gammaCorrection(init_color[i%4][1]);
			pixel_map[j][i].b = init_color[i%4][2]; // gammaCorrection(init_color[i%4][2]);
		}
	}
}

void Clear_DMA_Buffer(uint16_t offset, uint16_t cleared_buff_size){
	for(uint16_t i = offset ; i < (offset + cleared_buff_size) ; ++i ){
		TIMx_OC_DMA_Buffer_BRG[i] = 0;
	}
}

void Clear_FH_DMA_Buffer(void){
	Clear_DMA_Buffer(0 , DMA_BUFFER_SIZE/2);
}

void Clear_SH_DMA_Buffer(void){
	Clear_DMA_Buffer(DMA_BUFFER_SIZE/2 , DMA_BUFFER_SIZE/2);
}

/*void FillUp_DMA_Buffer(uint16_t offset, uint16_t fillUp_length, uint16_t pixel_idx){

#ifdef DEBUG_DMA_BUFFER_FILL_UP
	GPIOC->ODR &= (~GPIO_Pin_13);
#endif

	uint16_t i_percent_PS;
	uint16_t i_per_PS;

	for(uint16_t i = offset; i < (offset + fillUp_length) ; ++i ){
		i_percent_PS = i % PARALELL_STRIPS;
		i_per_PS = i / PARALELL_STRIPS;
		switch(i / (PARALELL_STRIPS*8)){
			case 0 : {
				TIMx_OC_DMA_Buffer_BRG[i] = (pixel_map[i_percent_PS][pixel_idx].b & (0x80 >> (i_per_PS%8))) ? T1H : T0H;
				break;
			}
			case 1 : {
				TIMx_OC_DMA_Buffer_BRG[i] = (pixel_map[i_percent_PS][pixel_idx].r & (0x80 >> (i_per_PS%8))) ? T1H : T0H;
				break;
			}
			case 2 : {
				TIMx_OC_DMA_Buffer_BRG[i] = (pixel_map[i_percent_PS][pixel_idx].g & (0x80 >> (i_per_PS%8))) ? T1H : T0H;
				break;
			}
			default:{ // the same as case 0
				TIMx_OC_DMA_Buffer_BRG[i] = (pixel_map[i_percent_PS][pixel_idx].b & (0x80 >> (i_per_PS%8))) ? T1H : T0H;
				break;
			}
		}
	}
#ifdef DEBUG_DMA_BUFFER_FILL_UP
	GPIOC->ODR |= (GPIO_Pin_13);
#endif
}*/

void FillUp_DMA_Buffer(uint16_t offset, uint16_t fillUp_length, uint16_t pixel_id1){

#ifdef DEBUG_DMA_BUFFER_FILL_UP
	GPIOC->ODR &= (~GPIO_Pin_13);
#endif

	uint16_t i_percent_PS;
	uint16_t i_per_PS;

	for(uint16_t i = offset; i < (offset + fillUp_length) ; ++i ){ /* fillUp_length / 3 - because there are 3 components-RGB*/
		i_percent_PS = i % PARALELL_STRIPS;
		i_per_PS = i / PARALELL_STRIPS;
		switch( (i/(PARALELL_STRIPS*8)) % 3 ){
			case 0 : {
				TIMx_OC_DMA_Buffer_BRG[i] = (pixel_map[i_percent_PS][pixel_id1 + i/DMA_PIXEL_SIZE].b & (0x80 >> (i_per_PS%8))) ? T1H : T0H;
				break;
			}
			case 1 : {
				TIMx_OC_DMA_Buffer_BRG[i] = (pixel_map[i_percent_PS][pixel_id1 + i/DMA_PIXEL_SIZE].r & (0x80 >> (i_per_PS%8))) ? T1H : T0H;
				break;
			}
			case 2 : {
				TIMx_OC_DMA_Buffer_BRG[i] = (pixel_map[i_percent_PS][pixel_id1 + i/DMA_PIXEL_SIZE].g & (0x80 >> (i_per_PS%8))) ? T1H : T0H;
				break;
			}
			/*default:{ // the same as case 0
				TIMx_OC_DMA_Buffer_BRG[i] = (pixel_map[i_percent_PS][pixel_idx].b & (0x80 >> (i_per_PS%8))) ? T1H : T0H;
				break;
			}*/
		}
	}
#ifdef DEBUG_DMA_BUFFER_FILL_UP
	GPIOC->ODR |= (GPIO_Pin_13);
#endif
	/*
	for(uint16_t i = offset ; i < (offset + cleared_buff_size) ; ++i){
		for(uint8_t l = 0; l<3; ++l){
			TIMx_OC_DMA_Buffer_BRG[i] = pixel_colorRGB[l][pixel_idx][k] & (0x80 >> i%8);
		}
	}
	*/
}

void Init_DMA_Buffer(void){
	/* buffer for the first pixel -> [0] */
	FillUp_DMA_Buffer(0, DMA_BUFFER_SIZE, 0);
}

void FillUpNext_FH_DMA_Buffer(void){
//	DMA_ClearFlag(DMA1_FLAG_HT3);
//	DMA_ClearITPendingBit(DMA1_IT_HT3);
	FillUp_DMA_Buffer(0, DMA_BUFFER_SIZE/2, pixel_id+1);

}

void FillUpNext_SH_DMA_Buffer(void){
//	DMA_ClearFlag(DMA1_FLAG_TC3);
//	DMA_ClearITPendingBit(DMA1_IT_TC3);
	FillUp_DMA_Buffer(DMA_BUFFER_SIZE/2 , DMA_BUFFER_SIZE/2, pixel_id+1);
}


void refreshLedStrip(void){

	TIM_Cmd(TIM3, DISABLE);
	TIM3->CCR2 = 0;
	TIM3->CCR3 = 0;
	TIM3->CCR4 = 0;

	DMA_Cmd(DMA1_Channel3, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel3, DMA_BUFFER_SIZE);

	/* buffer initialization*/
	pixel_id = 0;
//	HT_counter = 0;
	Init_DMA_Buffer();

	DMA_ClearFlag(DMA1_FLAG_HT3);
	DMA_ClearFlag(DMA1_FLAG_TC3);
	DMA_Cmd(DMA1_Channel3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);

}

/* DMA circular buffer handling*/
void DMA1_Channel3_IRQHandler(void){

#ifdef DEBUG_DMA_BUFFER_IRQ
	GPIOC->ODR &= (~GPIO_Pin_13);
#endif



	 /* HT or TC event completed, increment half transfer number */

	/* Half transfer */
	if(DMA_GetFlagStatus(DMA1_FLAG_HT3) != RESET){
#ifdef DEBUG_DMA_BUFFER_HT
	GPIOC->ODR |= (GPIO_Pin_14); // half transfer flag set
#endif
		DMA_ClearFlag(DMA1_FLAG_HT3);

		pixel_id += PIXEL_PER_BUFFER;

		if(pixel_id > LED_STRIP_SIZE-1){ // preparing for end transfer - setting TIMx OC values to 0
			//Clear_FH_DMA_Buffer();
			DMA_Cmd(DMA1_Channel3, DISABLE);
			TIM_Cmd(TIM3, DISABLE);
			DMA_ClearFlag(DMA1_FLAG_HT3);
			DMA_ClearFlag(DMA1_FLAG_TC3);
			//pixel_id = 0;
			Clear_SH_DMA_Buffer();
			return;
		}
//		FillUp_DMA_Buffer(0, DMA_BUFFER_SIZE/2, pixel_id+1);
		FillUpNext_FH_DMA_Buffer();

#ifdef DEBUG_DMA_BUFFER_HT
	GPIOC->ODR &= (~GPIO_Pin_14);
#endif
	} /* endif - Half transfer */

	/* Transfer Complete */
	if(DMA_GetFlagStatus(DMA1_FLAG_TC3) != RESET){
#ifdef DEBUG_DMA_BUFFER_TC
	GPIOC->ODR |= (GPIO_Pin_15); // transfer complete flag set
#endif

		DMA_ClearFlag(DMA1_FLAG_TC3);

		/*if(pixel_id > LED_STRIP_SIZE){   // end transfer
			DMA_Cmd(DMA1_Channel3, DISABLE);
			pixel_id = 0;
			return;
		}*/
		if(pixel_id > LED_STRIP_SIZE-1){ // preparing for end transfer - setting TIMx OC values to 0
			DMA_Cmd(DMA1_Channel3, DISABLE);
			TIM_Cmd(TIM3, DISABLE);
			DMA_ClearFlag(DMA1_FLAG_HT3);
			DMA_ClearFlag(DMA1_FLAG_TC3);
			//pixel_id = 0;
			Clear_SH_DMA_Buffer();
			return;
		}
//		FillUp_DMA_Buffer(DMA_BUFFER_SIZE/2 , DMA_BUFFER_SIZE/2, pixel_id+1);
		FillUpNext_SH_DMA_Buffer();

#ifdef DEBUG_DMA_BUFFER_TC
	GPIOC->ODR &= (~GPIO_Pin_15);
#endif
	} /* endif - Transfer complete */

	/*if( pixel_id >= LED_STRIP_SIZE -1){
		DMA_Cmd(DMA1_Channel3, DISABLE);
		TIM_Cmd(TIM3, DISABLE);
		Clear_DMA_Buffer(offset, DMA_BUFFER_SIZE/2);

		pixel_id = 0;
	}else{
		FillUp_DMA_Buffer( offset, DMA_BUFFER_SIZE/2 , pixel_id );
		DMA_ClearFlag(DMA1_FLAG_TC3);
		DMA_ClearFlag(DMA1_FLAG_HT3);
	}*/
#ifdef DEBUG_DMA_BUFFER_IRQ
	GPIOC->ODR |= (GPIO_Pin_13);
#endif

//#ifdef DEBUG_DMA_BUFFER_HT
//	GPIOC->ODR &= (~GPIO_Pin_14);
//#endif

//#ifdef DEBUG_DMA_BUFFER_TC
//	GPIOC->ODR &= (~GPIO_Pin_15);
//#endif

}
