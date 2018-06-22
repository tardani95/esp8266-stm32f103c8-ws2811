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

#include "esp8266.h" /* my lib */
#include "ws2811.h"  /* my lib */
#include "periph.h"  /* my lib */
#include "util.h"    /* my lib */

/* Private typedef */
/* Private define  */
// #define DEBUG

/* Private macro */

/* Private variables */

__IO uint8_t uart_receive_array[UART_BUFFER_SIZE]; /* UART_BUFFER_SIZE in esp8266.h */

/* Private function prototypes */
void OnUART_DataReceived(void);

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

int main(void)
{

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

	/* pwm for external interrupt register init*/
//	InitGPIO_PWM_EXTI(&GPIO_InitStructure);

	/**************************************************/
	/* TIMER INIT */
	/**************************************************/
	/* tim2 clock init */
	//InitTIM2_CLK(&TIM_TimeBase_InitStructure);

	/* tim2 ch2 pwm inti */
	//InitTIM2_CH2_PWM(&TIM_OC_InitStructure);

	/**************************************************/
	/* INTERRUPT INIT */
	/* the interrupt handlers found in stm32f1xx_it.c */
	/**************************************************/
	/* button interrupt init */
	InitEXTI_BTN(&EXTI_InitStructure, &NVIC_InitStructure);


	/**************************************************/
	/* ESP-01 INIT									  */
	/**************************************************/

	uint8_t receive_array_length = 13; //13
	InitESP8266((uint8_t*)uart_receive_array/*, receive_array_length*/);
	StartUDPReceivingWithCallback(receive_array_length, OnUART_DataReceived );

	/**************************************************/
	/* WS2811 INIT									  */
	/**************************************************/
	Init_WS2811((uint8_t*)uart_receive_array,receive_array_length);

	/* send out initial array */
//	InitLookUpTable();
//	RefreshLookUpTable1(50);

	Init_PixelMap();
	refreshLedStrip();


	while(1){
		/*if(isNewDataArrived){
			isNewDataArrived = 0;
			animOff = 0;
			while(!animOff){
				showAnim[uart_receive_array[12]](&animOff);
			}
		}*/
	}
}

void OnUART_DataReceived(void){
	switch(uart_receive_array[12]){
		case 0 :{
			RefreshLookUpTable(uart_receive_array[9],uart_receive_array[10],uart_receive_array[11]);
//			RefreshLookUpTable1(50);
			if(!txOn){
				refreshLedStrip();
			}
		}break;

		default:break;
	}
}



