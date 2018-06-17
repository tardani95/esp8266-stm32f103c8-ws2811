/*
******************************************************************************
File        : ws2811.c
Author      : Daniel Tar
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



/******************************************************************************/
/*                    Callback function declaration                           */
/******************************************************************************/



/******************************************************************************/
/*                          Function definitions                              */
/******************************************************************************/
void InitGPIO_LSSs(GPIO_InitTypeDef*);
void InitNVIC_LSS(NVIC_InitTypeDef*);
void InitDMA_CH3_TIM3_CHs(DMA_InitTypeDef*,uint8_t*);
void InitTIM3_CLK(TIM_TimeBaseInitTypeDef*);
void InitTIM3_PWM(TIM_OCInitTypeDef*);

/* interval: [ fromCH ; toCh ) */
void Init_WS2811(uint8_t* ptr_command_array, uint8_t command_array_size);

uint8_t gammaCorrection(uint8_t color);
void refreshLedStrip(void);

/* dma buffer handling*/
void DMA1_Channel3_IRQHandler(void);
