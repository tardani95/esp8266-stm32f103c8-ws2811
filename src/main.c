/******************************************************************************
File        : main.c
Author      : Daniel Tar
Version     : 1.0
Copyright   :
Description : Controlling WS2811 with an ESP8266 through an stm32f103c8 microcontoller
Info        : 2018-04-09
******************************************************************************/

/******************************************************************************/
/*                               Includes									  */
/******************************************************************************/
#include <stddef.h>
#include <stdio.h>
#include "stm32f10x.h"

#include "esp8266.h" 		/* my lib */
#include "ws2811.h"  		/* my lib */
#include "ws2811_util.h"	/* my lib */
#include "periph.h"  		/* my lib */
#include "util.h"    		/* my lib */
#include "eeprom.h"			/* my lib */

/* Private typedef */
/* Private define  */
#define OFFSET_LENGTH_VIRT_START_ADD	0x0000
#define OFFSET_LENGTH_SIZE				(2*PARALELL_STRIPS)

// #define DEBUG

/* Private macro */

/* Private variables */
uint8_t security_code = 233;

__IO uint8_t uart_receive_array[UART_BUFFER_SIZE]; /* UART_BUFFER_SIZE in esp8266.h */



/* Private function prototypes */
void OnUART_DataReceived(void);
uint16_t loadOffsetLengthValues(uint16_t *);
uint16_t saveOffsetLengthValues(uint16_t *);

/* Private functions */

#ifdef DEBUG
	#include "stm32f10x_dbgmcu.h"

	void InitDBG(void){
		DBGMCU_Config(DBGMCU_TIM2_STOP, ENABLE); /* this will make TIM2 stop when core is halted during debug */
		DBGMCU_Config(DBGMCU_TIM3_STOP, ENABLE);
		DBGMCU_Config(DBGMCU_STOP, ENABLE);
	}
#endif


/**
**===========================================================================
**  Abstract: main program
**===========================================================================
*/

/**
 * Hinweise:
 * Bei den Timern 1, 8, 15, 16 und 17 muessen die PWM Ausgaenge zusaetzlich mit der Funktion TIM_CtrlPWMOutputs(...) aktiviert werden.
 * Nur, falls der Timer aktiviert, aber die PWM-Outputs deaktiviert sind kommen die Einstellungen zum Idle State zum Tragen.
 * Die OCN Ausgaenge gibt es allgemein nur bei den Timern 1, 8, 15, 16 und 17.
 */

int main(void){

	SystemInit();


#ifdef DEBUG
	InitDBG();
#endif

	/* configure SysTick for delay functions */
	InitSysTick();


	/**************************************************/
	/* INIT STUCTURES                                 */
	/**************************************************/
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure; /* external interrupt init */
	NVIC_InitTypeDef NVIC_InitStructure; /* nested vector interrupt controller init */

	/**************************************************/
	/* GPIO INIT */
	/**************************************************/
	/* built-in led init */
	InitGPIO_LED(&GPIO_InitStructure);
	/*switch off the led by default*/
	GPIO_WriteBit(GPIOC,led_pin,Bit_SET);

	/* button init */
	InitGPIO_BTN(&GPIO_InitStructure);

	/**************************************************/
	/* TIMER INIT */
	/**************************************************/

	/**************************************************/
	/* INTERRUPT INIT */
	/* the interrupt handlers found in stm32f1xx_it.c */
	/**************************************************/
	/* button interrupt init */
	InitEXTI_BTN(&EXTI_InitStructure, &NVIC_InitStructure);


	/**************************************************/
	/* ESP-01 INIT									  */
	/**************************************************/

	uint8_t receive_array_length = 14; //13
 	InitESP8266((uint8_t*)uart_receive_array/*, receive_array_length*/);
	StartUDPReceivingWithCallback(receive_array_length, OnUART_DataReceived );

	/**************************************************/
	/* WS2811 INIT									  */
	/**************************************************/
	Init_WS2811((uint8_t*)uart_receive_array,receive_array_length);

	/**************************************************/
	/* EEPROM INIT									  */
	/**************************************************/
	Init_EEPROM();


	uint16_t offset_length[2*PARALELL_STRIPS] = {
			0,50,
			0,50,
			0,50
	};

//	saveOffsetLengthValues(offset_length);

	if(!loadOffsetLengthValues(offset_length)){
		setOffsetLengthValues(offset_length); 	// if load was successful then set the values
	}



	/* send out initial array */
//	Init_PixelMap();
	ColorRGB initColor;
	initColor.r = 244;
	initColor.g = 194;
	initColor.b = 66;
	setAllPixelColorRGBOnLedStrip(0,initColor);
	setAllPixelColorRGBOnLedStrip(1,initColor);
	setAllPixelColorRGBOnLedStrip(2,initColor);
	refreshLedStrip();

	/* wait 3 seconds */
	DelaySec(3);

//	Clear_PixelMap();

	while(1){
		/*if(isNewDataArrived){
			isNewDataArrived = 0;
			animOff = 0;
			while(!animOff){
				showAnim[uart_receive_array[12]](&animOff);
			}
		}*/

//		anim_strobe(0xff00ff,10,50,1000);
//		ColorHex colors[] = {0xFF0000, 0x00FF00, 0x0000FF, 0x7d007d, 0x7d7d00};
//		anim_bouncingBallsOnLedStrip(1, 10000*1000, 1, colors);

		/*
		anim_fadeInFadeOut(4,2000,7);

		for(uint8_t paletteID = 0; paletteID < PALETTES_SIZE; paletteID++){
			fillPattern(paletteID);
			refreshLedStrip();
			DelaySec(20);
		}

		ColorHex color;
		for(uint8_t i=0;i<7;++i){
			uint16_t temp = 255;
			switch(i){
				case 0: color = colorToHex(temp,0,0); break;
				case 1: color = colorToHex(0,temp,0); break;
				case 2: color = colorToHex(0,0,temp); break;
				case 3: color = colorToHex(temp,temp,0); break;
				case 4: color = colorToHex(0,temp,temp); break;
				case 5: color = colorToHex(temp,0,temp); break;
				case 6: color = colorToHex(temp,temp,temp); break;
				default: break;
			}
			anim_meteorRainOnLedStrip(1,color,6,64,1,60);
			DelaySec(20);
		}*/

		//anim_meteorRainOnLedStrip(2,0xff0000,10,64,1,30);
		//DelayMs(200);
	}
}

void OnUART_DataReceived(void){
	if(uart_receive_array[SECURITY_BYTE] == security_code){
		switch(uart_receive_array[MODE_BYTE]){
				case 0 :{
					ColorRGB solidColor;
					solidColor.r = uart_receive_array[PARAM_START];
					solidColor.g = uart_receive_array[PARAM_START+1];
					solidColor.b = uart_receive_array[PARAM_START+2];

					if(!txOn){
						fillSolid(solidColor);
						refreshLedStrip();
					}
				}break;

				default:break;
			}
	}
}

uint16_t loadOffsetLengthValues(uint16_t *offset_length){
	uint16_t failCounter = 0; // 0 - success

	/* READ */
	for(uint16_t i = OFFSET_LENGTH_VIRT_START_ADD; i < OFFSET_LENGTH_SIZE ; i+=2){
		failCounter += EE_ReadVariable(i,&offset_length[i]);
		failCounter += EE_ReadVariable(i+1,&offset_length[i+1]);
	}

	return failCounter;
}

uint16_t saveOffsetLengthValues(uint16_t *offset_length){
	/* WRITE */
	FLASH_Unlock();

	for(uint16_t i = OFFSET_LENGTH_VIRT_START_ADD; i < OFFSET_LENGTH_SIZE ; i+=2){
		if(EE_WriteVariable(i,offset_length[i]) != FLASH_COMPLETE){
			return 1;
		}
		if(EE_WriteVariable(i+1,offset_length[i+1]) != FLASH_COMPLETE){
			return 1;
		}
	}

	FLASH_Lock();

	return 0; // returns 0 if all data are written successfully
}



