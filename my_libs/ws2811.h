/*
******************************************************************************
File        : ws2811.h
Author      : Daniel Tar
Version     :
Copyright   :
Description : ws2811 library for stm32f103c8 microcontroller
Info        : 16.06.2018
*******************************************************************************
*/

#ifndef _WS2811_H_
#define _WS2811_H_

/*========================================================================*/
/*                             INCLUDES									  */
/*========================================================================*/
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"   /* reset and clock control */
#include "stm32f10x_gpio.h"  /* general purpose input output */
#include "stm32f10x_tim.h"   /* timer header */
#include "stm32f10x_dma.h"   /* direct memory access */

#include "stm32f10x_exti.h"  /* external interrupt */
#include "stm32f1xx_it.h" 	 /* interrupt handler  */

/* High level functions for NVIC and SysTick (add-on to CMSIS functions)
 * NVIC - Nested Vector Interrupt Controller
 */
#include "misc.h"

#include "util.h" 			/* my lib - delay functions */
//#include "ws2811_util.h"	/* utility functions for this library */

/*==========================================================================*/
/*                           WS2811  SETTINGS								*/
/*==========================================================================*/

/* Select operating mode according to your led strip */
//#define MODE_800kHz
#define MODE_400kHz

/* high period times in counter ticks @72MHz */
/* 90 tick is equal with 1.25 us - prescaler = 0 */
/* 90 tick is equal with 2.5  us - prescaler = 1 */
#ifdef MODE_400kHz
	#define TIM_PRESCALER 1
#endif /*MODE_400kHz*/
#ifdef MODE_800kHz
	#define TIM_PRESCALER 0
#endif /*MODE_800kHz*/

/* according to ws2811 datasheet */
#define TIM_PERIOD 90	/* 90 */
#define T1H        43	/* 43 */
#define T0H        18	/* 18 */
/*==========================================================================*/

#define LED_NUMBER_ON_ONE_STRIP 150
#define LED_PER_IC              3
#define DIFFERENT_LEDGROUPS     (LED_NUMBER_ON_ONE_STRIP / LED_PER_IC)

#define PARALELL_STRIPS    		3 	/* 1-4 */
#define SERIES_STRIPS      		2   /* 1-.. depends on power supply */
#define PIXEL_ARRAY_SIZE   		( PARALELL_STRIPS * (SERIES_STRIPS * DIFFERENT_LEDGROUPS * 3))

#define COLOR_NUM				3
#define COLOR_BITS				8
#define LED_BITS           		( COLOR_NUM * COLOR_BITS ) /* = 24 */
#define BITS_PER_COLOR			( PARALELL_STRIPS * COLOR_BITS )

#define PIXEL_PER_BUFFER		6 /* only even */
#define DMA_PIXEL_SIZE			( PARALELL_STRIPS * LED_BITS )			 /* 3*24 = 72 */
#define DMA_BUFFER_SIZE   		( PIXEL_PER_BUFFER * DMA_PIXEL_SIZE )
#define LED_STRIP_SIZE    		( SERIES_STRIPS * DIFFERENT_LEDGROUPS) + PIXEL_PER_BUFFER/2

#define R						1
#define G						2
#define B						0

#define RED						0x00FF0000
#define GREEN					0x0000FF00
#define BLUE					0x000000FF
#define EMPTY_COLOR				0x00000000
#define RED_S 		16
#define GREEN_S		8
#define BLUE_S		0
/*==========================================================================*/
/*                             Private typedef                              */
/*==========================================================================*/
typedef struct RGB{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}ColorRGB;

typedef uint32_t ColorHex;
/*============================================================================*/

//extern __IO uint8_t TIMx_OC_DMA_Buffer_BRG[DMA_BUFFER_SIZE];
//extern __IO uint8_t pixel_mapBRG[PARALELL_STRIPS][LED_STRIP_SIZE][COLOR_NUM];
//extern __IO ColorRGB pixel_map[LED_STRIP_SIZE][PARALELL_STRIPS];

extern __IO uint8_t  txOn; /* @TODO - init function pointer parameter passing*/



/*========================================================================*/

void InitGPIO_LSSs(GPIO_InitTypeDef*);
void InitNVIC_LSS(NVIC_InitTypeDef*);
void InitDMA_CH3_TIM3_CHs(DMA_InitTypeDef*,uint8_t*);
void InitTIM3_CLK(TIM_TimeBaseInitTypeDef*);
void InitTIM3_PWM(TIM_OCInitTypeDef*);

void Init_WS2811(uint8_t* ptr_command_array, uint8_t command_array_size);

void InitOffsetLengthValues();
void setOffsetLengthValues(uint16_t *array);
void getOffsetLengthValues(uint16_t *array);

uint8_t gammaCorrection(uint8_t color);

/* color convert functions */
ColorHex colorToHex(uint8_t red, uint8_t green, uint8_t blue);
ColorRGB colorToRGB(uint8_t red, uint8_t green, uint8_t blue);
ColorHex colorRGBToHex(ColorRGB color);
ColorRGB colorHexToRGB(ColorHex color);


void setPixelColorHex(uint16_t pxNr, uint8_t parallelLedStripID, ColorHex color);
void setPixelColorRGB(uint16_t pxNr, uint8_t parallelLedStripID, ColorRGB pxColor);
void setPixelColor(uint16_t pxNr, uint8_t parallelLedStripID, uint8_t red, uint8_t green, uint8_t blue);

ColorHex getPixelColorHex(uint16_t pxNr, uint8_t parallelLedStripID);
ColorRGB getPixelColorRGB(uint16_t pxNr, uint8_t parallelLedStripID);

void setAllPixelColorHexOnLedStrip(uint8_t parallelLedStripID, ColorHex pxColor);
void setAllPixelColorRGBOnLedStrip(uint8_t parallelLedStripID, ColorRGB pxColor);
void setAllPixelColorOnLedStrip(uint8_t parallelLedStripID, uint8_t red, uint8_t green, uint8_t blue);

void Init_PixelMap(void);
void Clear_PixelMap(void);
void refreshLedStrip(void);

/* dma buffer handling*/
void DMA1_Channel3_IRQHandler(void);

#endif /* _WS2811_H_ */
