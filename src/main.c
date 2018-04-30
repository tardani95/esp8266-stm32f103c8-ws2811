/*
******************************************************************************
File        : main.c
Author      : Daniel Tar
Version     : 1.0
Copyright   :
Description : esp8266 -> uart -> stm32f103c8
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

#include "stm32f10x_rcc.h"

#include "stm32f10x_gpio.h"

#include "stm32f10x_tim.h"
#include "stm32f1xx_it.h"
#include "stm32f10x_exti.h"
#include "misc.h"

#include "stm32f10x_usart.h"


/* Private typedef */
/* Private define  */



/* Private macro */
/* Private variables */
uint8_t receivedData = 0;
uint8_t rArray[20];
uint8_t i=0;

/* Private function prototypes */
/* Private functions */
void USART1_IRQHandler(void){
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	//USART_ClearITPendingBit(USART1, USART_IT_PE);
	receivedData = USART_ReceiveData(USART1);
	if(receivedData == '+'){
		i=0;
	}
	rArray[i] = receivedData;
	i++;
}


/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/

int main(void)
{

	SystemInit();

	GPIO_InitTypeDef GPIO_InitStructure_OUT;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure_OUT.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure_OUT.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure_OUT.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure_OUT);

	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);

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


	NVIC_InitTypeDef NVIC_InitStructure;

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

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART1, ENABLE);




	int i = 0;

	while(1)
	{
		/* the built in led on PC13 blinks with 1 second duration */
		/* turn on PC13 -> turn off LED */
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);

		/* delay 1 sec */
		for (i=0;i<10000000;i++){

		}
		/* turn off PC13 -> turn on LED */
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);

		/* delay 1 sec */
		for (i=0;i<10000000;i++){

		}
	}

	return 0;
}



