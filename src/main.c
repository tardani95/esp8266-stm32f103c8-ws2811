/*
******************************************************************************
File        : main.c
Author      : Daniel Tar
Version     : 1.0
Copyright   :
Description : Controlling WS2811 with an ESP8266 through an stm32f103c8 microcontoller
Info        : 2018-04-09
******************************************************************************
ESP8266 AT Commands:

	Test if the communication functions
	AT

	Setting the Default Wi-Fi mode; Configuration Saved in the Flash
	AT+CWMODE_DEF=<mode>
		1: Station mode
		2: SoftAP mode
		3: SoftAP+Station mode

	Listing Available APs
	AT+CWLAP

	Connecting to an AP; Configuration Saved in the Flash
	AT+CWJAP_DEF=<ssid>,<pwd>
	AT+CWJAP_DEF="ws2811_test","0123456789"

	Disconnecting from the AP
	AT+CWQAP

	Auto-Connecting to the AP or Not
	AT+CWAUTOCONN=<enable>
		0: does NOT auto-connect to AP on power-up.
 		1: connects to AP automatically on power-up.

 	Setting the MAC Address of the ESP8266 Station; Configuration Saved
	in the Flash
	AT+CIPSTAMAC_DEF=<mac>
	AT+CIPSTAMAC_DEF="18:fe:35:98:d3:7b"

	Setting the Default IP Address of the ESP8266 Station; Configuration Saved
	in the Flash
	AT+CIPSTA_DEF=<ip>[,<gateway>,<netmask>]
	AT+CIPSTA_DEF="192.168.0.100","192.168.0.1","255.255.255.0"

	Quering the IP address of the ESP8266
	Station
	AT+CIPSTA?

	Getting the Connection Status
	AT+CIPSTATUS
	Response:
		STATUS:<stat>
		+CIPSTATUS:<linkID>,<type>,<remoteIP>,<remotePort>,<localPort>,<tetype>

 			<stat>: status of the ESP8266 Station interface.
				2: The ESP8266 Station is connected to an AP and its IP is obtained.
				3: The ESP8266 Station has created a TCP or UDP transmission.
				4: The TCP or UDP transmission of ESP8266 Station is disconnected.
				5: The ESP8266 Station does NOT connect to an AP.
			<tetype>:
				0: ESP8266 runs as a client.
				1: ESP8266 runs as a server.

	Establishing TCP Connection, UDP Transmission or SSL Connection
	AT+CIPSTART=<type>,<remoteIP>,<remotePort>[,(<UDP_localPort>),(<UDP_mode>)]
	AT+CIPSTART="UDP","0",0,1302,2

		[<UDP	mode>]: optional. In the UDP transparent transmission, the value of this parameter has to be 0.
			0: the destination peer entity of UDP will not change; this is the default setting.
			1: the destination peer entity of UDP can change once.
			2: the destination peer entity of UDP is allowed to change.

	Setting Transmission Mode
	AT+CIPMODE=<mode>

		<mode>:
			0: normal transmission mode.
			1: UART-Wi-Fi passthrough mode (transparent transmission), which can only be enabled in TCP
			   single connection mode or in UDP mode when the remote IP and port do not change.

	Saving UDP Transmission in Flash
	AT+SAVETRANSLINK=<mode>,<remoteIP>,<remotePort>,<type>[,<UDP_localPort>]

		To exit send: "+++AT+SAVETRANSLINK=1\r\n"

	Updating the Software Through Wi-Fi
	AT+CIUPDATE

	Receives Network Data
	+IPD,<len>[,<remoteIP>,<remotePort>]:<data>
	\r\n+IPD,4: '255' '0' '187' '0'

Initialize ESP8266:
	AT
	AT+CWMODE_DEF=1
	AT+CWLAP
	AT+CWJAP_DEF="ws2811_test","0123456789"
	AT+CWAUTOCONN=1
	AT+CIPSTART="UDP","0",0,1302,2

Waiting for the packages:
	+IPD,<len>[,<remoteIP>,<remotePort>]:<data>


******************************************************************************
*/

/* Includes */
#include <stddef.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"  /* reset and clock control */

#include "stm32f10x_tim.h"  /* timer header */

#include "stm32f10x_exti.h" /* external interrupt*/
#include "stm32f1xx_it.h" 	/* interrupt handler */

/* High level functions for NVIC and SysTick (add-on to CMSIS functions)
 * NVIC - Nested Vector Interrupt Controller
 */
#include "misc.h"

#include "periph.h"

#include "stm32f10x_usart.h"

/* Private typedef */
/* Private define  */


/* Private macro */
/* Private variables */
//uint16_t led_pin = GPIO_Pin_13; /* on port C*/
//uint16_t button_pin = GPIO_Pin_14; /* on port B*/
//uint16_t pwm_pin = GPIO_Pin_0;
//extern uint16_t repetition_counter = 0;

uint8_t receivedData = 0;
uint8_t rArray[20];
char startUPDListening[] = "AT+CIPSTART=\"UDP\",\"0\",0,1302,2\r\n\0";
uint8_t sendData = 0;
uint8_t uart_counter=0;


/* Private function prototypes */
/* Private functions */
void USART1_IRQHandler(void){
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET){
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);

		receivedData = USART_ReceiveData(USART1);
		if(receivedData == '+'){
			uart_counter=0;
		}
		rArray[uart_counter] = receivedData;
		uart_counter++;
	}

	if(USART_GetITStatus(USART1,USART_IT_TXE) == SET){
		sendData = startUPDListening[uart_counter];
		if(sendData == '\0'){
			uart_counter=0;
			USART_ClearITPendingBit(USART1, USART_IT_TXE);
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			return;
		}
		USART_SendData(USART1, startUPDListening[uart_counter]);
		uart_counter++;
	}

}



/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/

/**
 * Hinweise:
 * Bei den Timern 1, 8, 15, 16 und 17 müssen die PWM Ausgänge zusätzlich mit der Funktion TIM_CtrlPWMOutputs(...) aktiviert werden.
 * Nur, falls der Timer aktiviert, aber die PWM-Outputs deaktiviert sind kommen die Einstellungen zum Idle State zum Tragen.
 * Die OCN Ausgänge gibt es allgemein nur bei den Timern 1, 8, 15, 16 und 17.
 */

int main(void)
{

	SystemInit();

	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure; /* external interrupt init */
	NVIC_InitTypeDef NVIC_InitStructure; /* nested vector interrupt controller init */
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure; // timer init
	TIM_OCInitTypeDef TIM_OC_InitStructure; // output compare init

	USART_InitTypeDef USART_InitStructure;

	/* init uart gpio pins */
	/* tx */
	GPIO_StructInit(&GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* rx */
	GPIO_StructInit(&GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	/* USART1 RX*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

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

	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);


	/*wait for esp8266 system startup*/
	delayMicroSec(100000);

	USART_Cmd(USART1, ENABLE);
	/* USART_IT_TXE:  Transmit Data Register empty interrupt */
	/* the transmit data register is empty at the beginning, so the an interrupt will be generated
	 * and in the interrupt handler it sends our data out, after it sent out
	 */
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	USART_Cmd(USART1, ENABLE);

	delayMicroSec(100000);

	for(uint8_t i=0;i<20;i++){
		rArray[i]=0;
	}

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);




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
	InitTIM2_CLK(&TIM_TimeBase_InitStructure);

	/* tim2 ch2 pwm inti */
	InitTIM2_CH2_PWM(&TIM_OC_InitStructure);

	/* tim3 clock init */
	InitTIM3_CLK(&TIM_TimeBase_InitStructure);

	/* tim3 ch3 pwm init */
	InitTIM3_CH3_CH4_PWM(&TIM_OC_InitStructure);

	/**************************************************/
	/* INTERRUPT INIT */
	/* the interrupt handlers found in stm32f1xx_it.c */
	/**************************************************/
	/* button interrupt init */
	InitEXTI_BTN(&EXTI_InitStructure, &NVIC_InitStructure);

	/* pwm interrupt capture */
	InitEXTI_TIM3_PWM(&EXTI_InitStructure, &NVIC_InitStructure);
	InitEXTI_TIM2_PWM(&EXTI_InitStructure, &NVIC_InitStructure);


	InitLookUpTable();

	delayMicroSec(500);
	TIM3->CCR3 = TIM3->CCR4 = look_up_table_2[0] ? 43 : 18;
	while(1){
		//AnimFadeInFadeOut(2000,1000,3000);
		RefreshLookUpTable(rArray[7],rArray[8],rArray[9]);
	}
}


