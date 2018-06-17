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


__IO uint8_t TIMx_OC_DMA_Buffer[DMA_BUF_LEN]; /* DMA buffer for TIMx OutputCompare (OC) values */

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
void InitGPIO_LSSs(GPIO_InitTypeDef*){

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
void InitNVIC_LSS(NVIC_InitTypeDef*){

}
void InitDMA_CH3_TIM3_CHs(DMA_InitTypeDef*,uint8_t*){

}
void InitTIM3_CLK(TIM_TimeBaseInitTypeDef*){

}
void InitTIM3_PWM(TIM_OCInitTypeDef*){

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
