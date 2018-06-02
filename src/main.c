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

uint8_t uart_receive_array[20];
uint8_t uart_transmit_array[] = "AT+CIPSTART=\"UDP\",\"0\",0,1302,2\r\n";


/* Private function prototypes */
/* Private functions */

/**
**===========================================================================
**
**  Abstract: main program
**
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

	/**************************************************/
	/* INIT STUCTURES                                 */
	/**************************************************/
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure; /* external interrupt init */
	NVIC_InitTypeDef NVIC_InitStructure; /* nested vector interrupt controller init */
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure; /* timer init */
	TIM_OCInitTypeDef TIM_OC_InitStructure; /* output compare init */
	USART_InitTypeDef USART_InitStructure;  /* uart init */
	DMA_InitTypeDef DMA_InitStructure;		/* dma init  */

	/**************************************************/
	/* UART INIT                                      */
	/**************************************************/
	InitGPIO_UART1(&GPIO_InitStructure);
	InitNVIC_UART1_TX(&NVIC_InitStructure);
	InitNVIC_UART1_RX(&NVIC_InitStructure);
	InitUART1(&USART_InitStructure);

	/*wait for esp8266 system startup*/
	delaySec(1); //10

	/* USART_IT_TXE:  Transmit Data Register empty interrupt */
	/* the transmit data register is empty at the beginning, so the an interrupt will be generated
	 * and in the interrupt handler it sends our data out, after it sent out
	 */
	/* USART_IT_TC:   Transmission complete interrupt */
	/* USART_IT_RXNE: Receive Data register not empty interrupt */
	USART_Cmd(USART1, ENABLE);

	/* init receive array with dummy data to see if the dma is working */
	for(uint8_t i = 0; i<20; ++i){
		uart_receive_array[i] = 0xAA;
	}

	uint8_t receive_array_length = 13; //13
	uint8_t transmit_array_length = 32; //32


	/**************************************************/
	/* DMA for UART INIT                              */
	/**************************************************/
	InitDMA_CH4_UART1_TX(&DMA_InitStructure, uart_transmit_array);
	InitDMA_CH5_UART1_RX(&DMA_InitStructure, uart_receive_array);

	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

	// clear rx dma buffer
	DMA_Cmd(DMA1_Channel5, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel5, 1);
	DMA_ClearFlag(DMA1_FLAG_TC5);
	DMA_Cmd(DMA1_Channel5, ENABLE);

	/* start transmission */
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel4, transmit_array_length);

	DMA_ClearFlag(DMA1_FLAG_TC4);
	DMA_Cmd(DMA1_Channel4, ENABLE);
	/* end transmission */

	/* wait for esp8266 sets up the upd connection */
	delaySec(2); //2


	/* start circular receiving */
	DMA_Cmd(DMA1_Channel5, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel5, receive_array_length);

	DMA_ClearFlag(DMA1_FLAG_TC5);
	DMA_Cmd(DMA1_Channel5, ENABLE);
	/* end receiving */


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
  * @brief  This function handles the UART1_TX DMA
  * @param  None
  * @retval None
  */
void DMA1_Channel4_IRQHandler(void){
	/* all uart data sent out*/
	DMA_ClearFlag(DMA1_FLAG_TC4);
	/*if(DMA_GetFlagStatus(DMA1_FLAG_TC4)){
		DMA_ClearFlag(DMA1_FLAG_TC4);
	}*/
	DMA_Cmd(DMA1_Channel4, DISABLE);
}

/**
  * @brief  This function handles the UART1_RX DMA
  * @param  None
  * @retval None
  */
void DMA1_Channel5_IRQHandler(void){
	/* all data received */
	switch(uart_receive_array[12]){
		case 0 :{
			RefreshLookUpTable(uart_receive_array[9],uart_receive_array[10],uart_receive_array[11]);
			RefreshLookUpTable1();

			DMA_Cmd(DMA1_Channel3, DISABLE);
			DMA_SetCurrDataCounter(DMA1_Channel3, 6*24*3);

			DMA_ClearFlag(DMA1_FLAG_TC3);
			DMA_Cmd(DMA1_Channel3, ENABLE);
		}break;

		default:break;
	}
	DMA_ClearFlag(DMA1_FLAG_TC5);
	//isNewDataArrived = 1;
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
	if(led_counter>4-1){
		DMA_Cmd(DMA1_Channel3, DISABLE);
		TIM3->CCR2 = 0;
		TIM3->CCR3 = 0;
		TIM3->CCR4 = 0;
		led_counter = 0;
		delayMicroSec(50);

	}
	DMA_ClearFlag(DMA1_FLAG_TC3);
}



