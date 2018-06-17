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
#include "periph.h" /* my lib */


/* Private typedef */
/* Private define  */


/* Private macro */
/* Private variables */
uint8_t receivedData = 0;
uint8_t rArray[20];
//char startUPDListening[] = "AT+CIPSTART=\"UDP\",\"0\",0,1302,2\r\n\0";
uint8_t sendData = 0;
uint8_t uart_counter=0;
uint8_t led_counter = 0;
uint8_t isNewDataArrived = 0;




/* Private variables */
uint8_t uart_receive_array[20];

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
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure; /* timer init */
	TIM_OCInitTypeDef TIM_OC_InitStructure; /* output compare init */
	DMA_InitTypeDef DMA_InitStructure;		/* dma init  */



//	uint8_t uart_transmit_array[] = "AT+CIPSTART=\"UDP\",\"0\",0,1302,2\r\n";
	/* init receive array with dummy data to see if the dma is working */
	for(uint8_t i = 0; i<20; ++i){
		uart_receive_array[i] = 0xAA;
	}

	uint8_t receive_array_length = 13; //13
//	uint8_t transmit_array_length = 32; //32

	InitESP8266(uart_receive_array);
	StartUDPReceivingWithCallback(receive_array_length, OnUART_DataReceived );


	/**************************************************/
	/* GPIO INIT */
	/**************************************************/
	/* built-in led init */
	InitGPIO_LED(&GPIO_InitStructure);
	/*switch off the led by default*/
	GPIO_WriteBit(GPIOC,led_pin,Bit_SET);

	/* button init */
	InitGPIO_BTN(&GPIO_InitStructure);

	/* ledstrip signal pin init*/
	InitGPIO_LSS1(&GPIO_InitStructure);
	InitGPIO_LSS2(&GPIO_InitStructure);

	/* pwm for external interrupt register init*/
	InitGPIO_PWM_EXTI(&GPIO_InitStructure);


	/**************************************************/
	/* TIMER INIT */
	/**************************************************/
	/* tim2 clock init */
	//InitTIM2_CLK(&TIM_TimeBase_InitStructure);

	/* tim2 ch2 pwm inti */
	//InitTIM2_CH2_PWM(&TIM_OC_InitStructure);

	/* tim3 clock init */
	InitTIM3_CLK(&TIM_TimeBase_InitStructure);

	/* tim3 ch3 pwm init */
	InitTIM3_CH2_CH3_CH4_PWM(&TIM_OC_InitStructure);

	/**************************************************/
	/* INTERRUPT INIT */
	/* the interrupt handlers found in stm32f1xx_it.c */
	/**************************************************/
	/* button interrupt init */
	InitEXTI_BTN(&EXTI_InitStructure, &NVIC_InitStructure);

	/* pwm interrupt capture */
	//InitEXTI_TIM3_PWM(&EXTI_InitStructure, &NVIC_InitStructure);
	//InitEXTI_TIM2_PWM(&EXTI_InitStructure, &NVIC_InitStructure);
	//InitNVIC_LSS1(&NVIC_InitStructure);
	InitNVIC_LSS123(&NVIC_InitStructure);

	//InitDMA_CH2_TIM3_CH3(&DMA_InitStructure, look_up_table_1);
	InitDMA_CH3_TIM3_CH2_to_CH4(&DMA_InitStructure, look_up_table_1);

	TIM_DMAConfig(TIM3, TIM_DMABase_CCR2, TIM_DMABurstLength_3Transfers);

	TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);

	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);

	/* Enable the TIM Counter */
	TIM_Cmd(TIM3, ENABLE);

	InitLookUpTable();
	RefreshLookUpTable1();

	/* reset and enable dma*/
	/* send out initial array */
	DMA_Cmd(DMA1_Channel3, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel3, 6*24*3);

	DMA_ClearFlag(DMA1_FLAG_TC3);
	DMA_Cmd(DMA1_Channel3, ENABLE);


	/* send out initial array */
//	DMA_Cmd(DMA1_Channel2, DISABLE);
//	DMA_SetCurrDataCounter(DMA1_Channel2, LOOK_UP_TABLE_SIZE);
//
//	DMA_ClearFlag(DMA1_FLAG_TC2);
//	DMA_Cmd(DMA1_Channel2, ENABLE);
//
//	TIM3->CCR3 = look_up_table_1[0];
//	delaySec(1);
//	TIM3->CCR3 = 0;
//	delayMicroSec(55);


	while(1){
		//AnimFadeInFadeOut(4000,2000,4000);
		//RefreshLookUpTable(rArray[7],rArray[8],rArray[9]);
		/*delayMicroSec(55);
		if(isNewDataArrived){
			// send out ledstip signal
			DMA_Cmd(DMA1_Channel3, DISABLE);
			DMA_SetCurrDataCounter(DMA1_Channel3, LOOK_UP_TABLE_SIZE*3);
			DMA_ClearFlag(DMA1_FLAG_TC3);
			DMA_Cmd(DMA1_Channel3, ENABLE);
			delayMicroSec(6000);
			isNewDataArrived = 0;
		}*/
	}
}



/**
  * @brief  This function handles the PWM generation with DMA for TIM3_CH3
  * @param  None
  * @retval None
  */
void DMA1_Channel2_IRQHandler(void){
  /* data shifted out */
  DMA_ClearFlag(DMA1_FLAG_TC2);
  TIM3->CCR3 = 0;
  /*if(led4_counter > 38){
	  led4_counter = 0;
	  TIM3->CCR3 = 0;
  }
  led4_counter++;*/
}

void DMA1_Channel3_IRQHandler(void){
	/* one led data shifted out */
	led_counter++;
	if(led_counter>LED_NUMBER){
		DMA_Cmd(DMA1_Channel3, DISABLE);
		TIM3->CCR2 = 0;
		TIM3->CCR3 = 0;
		TIM3->CCR4 = 0;
		led_counter = 0;
//		delayMicroSec(50);
		DMA_Cmd(DMA1_Channel3, DISABLE);
		DMA_SetCurrDataCounter(DMA1_Channel3, 1*24*3);

		delayMicroSec(55);
		DMA_ClearFlag(DMA1_FLAG_TC3);
		DMA_Cmd(DMA1_Channel3, ENABLE);
	}else{
		DMA_ClearFlag(DMA1_FLAG_TC3);
	}

}

void OnUART_DataReceived(void){
	switch(uart_receive_array[12]){
		case 0 :{
			RefreshLookUpTable(uart_receive_array[9],uart_receive_array[10],uart_receive_array[11]);
			RefreshLookUpTable1();
		}break;

		default:break;
	}
}



