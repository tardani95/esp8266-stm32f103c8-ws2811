/*
******************************************************************************
File        : esp8266.c
Author      : Daniel Tar
Version     :
Copyright   :
Description : esp8266 (ESP-01) library for STM32F10x
Info        : 31.05.2018
*******************************************************************************
*                  ESP8266 AT COMMANDS AND DESCRIPTION						  *
*******************************************************************************
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

*******************************************************************************
Initialize ESP8266:
*******************************************************************************
	AT
	AT+CWMODE_DEF=1
	AT+CWLAP
	AT+CWJAP_DEF="ws2811_test","0123456789"
	AT+CWAUTOCONN=1

*******************************************************************************
After Startup:
*******************************************************************************
	AT+CIPSTART="UDP","0",0,1302,2

*******************************************************************************
Waiting for the packages:
*******************************************************************************
	\r\n+IPD,<len>[,<remoteIP>,<remotePort>]:<data>

*/

/******************************************************************************/
/*                               Includes									  */
/******************************************************************************/
#include "esp8266.h"


/**
  * @brief  This function initialize the gpio pins for the UART1 (PA9 - TX, PA10 - RX)
  * @param  GPIO_InitTypeDef variable
  * @retval None
  */
void InitGPIO_UART1(GPIO_InitTypeDef* GPIO_InitStructure){
	/* TX on PA9 */
	GPIO_StructInit(GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure->GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, GPIO_InitStructure);

	/* RX on PA10 */
	GPIO_StructInit(GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure->GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, GPIO_InitStructure);
}

/**
  * @brief  This function initialize the nested vectored interrupt controller for the UART1 (PA9 - TX , PA10 - RX)
  * @param  NVIC_InitTypeDef variable
  * @retval None
  */
void InitNVIC_UART1(NVIC_InitTypeDef* NVIC_InitStructure){
	/* USART1 TX*/
	NVIC_InitStructure->NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure->NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure->NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure->NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(NVIC_InitStructure);

	/* USART1 RX*/
	NVIC_InitStructure->NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure->NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure->NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure->NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(NVIC_InitStructure);
}

/**
  * @brief  This function initialize the DMA controller for the UART1 TX (PA9 - TX)
  * @param  DMA_InitTypeDef variable
  * @param  uint8_t array what to send
  * @retval None
  */
void InitDMA_CH4_UART1_TX(DMA_InitTypeDef* DMA_InitStructure, uint8_t* usart_transmit_array){
	/* DMA 1, Channel 4 for USART1 TX */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure->DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
	DMA_InitStructure->DMA_MemoryBaseAddr = (uint32_t) usart_transmit_array;
	DMA_InitStructure->DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure->DMA_BufferSize = 0;
	DMA_InitStructure->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure->DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure->DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure->DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure->DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4, DMA_InitStructure);
}

/**
  * @brief  This function initialize the DMA controller for the UART1 RX (PA10 - RX)
  * @param  DMA_InitTypeDef variable
  * @param  uint8_t array where to receive
  * @retval None
  */
void InitDMA_CH5_UART1_RX(DMA_InitTypeDef* DMA_InitStructure, uint8_t* usart_receive_array){
	/* DMA 1, Channel 5 for USART1 RX */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel5);
	DMA_StructInit(DMA_InitStructure);
	DMA_InitStructure->DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
	DMA_InitStructure->DMA_MemoryBaseAddr = (uint32_t)usart_receive_array;
	DMA_InitStructure->DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure->DMA_BufferSize = 0;
	DMA_InitStructure->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure->DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure->DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure->DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure->DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5, DMA_InitStructure);
}

/**
  * @brief  This function initialize the UART1 settings
  * @param  USART_InitTypeDef variable
  * @retval None
  */
void InitUART1(USART_InitTypeDef* USART_InitStructure){
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

	USART_StructInit(USART_InitStructure);
	USART_InitStructure->USART_BaudRate = 115200;
	USART_InitStructure->USART_WordLength = USART_WordLength_8b;
	USART_InitStructure->USART_StopBits = USART_StopBits_1;
	USART_InitStructure->USART_Parity = USART_Parity_No;
	USART_InitStructure->USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure->USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,USART_InitStructure);
}

/**
  * @brief  This function initialize the ESP8266 (ESP-01)
  * @param  None
  * @retval None
  */
void InitESP8266(){

}

/**
  * @brief  This function starts the listening to the UPD packages for a specified size
  * @param  None
  * @retval None
  */
void StartUDPReceiving(uint8_t*){

}


