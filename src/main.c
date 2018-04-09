/*
******************************************************************************
File        : main.c
Author      : Daniel Tar
Version     : 1.0
Copyright   :
Description :
Info        : 2018-04-09
******************************************************************************
*/

/* Includes */
#include <stddef.h>
#include <stdio.h>
#include "stm32f10x.h"

/* Private typedef */
/* Private define  */



/* Private macro */
/* Private variables */
 USART_InitTypeDef USART_InitStructure;


/* Private function prototypes */
/* Private functions */



/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/

int main(void)
{
  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used. 
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates 
  *  SCB->VTOR register.  
  *  E.g.  SCB->VTOR = 0x20000000;  
  */

  SystemInit();

  GPIO_InitTypeDef GPIO_InitStructure_OUT;


  /* USARTx configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - No parity
          - Hardware flow control disabled (RTS and CTS signals)
          - Receive and transmit enabled
   */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure_OUT.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure_OUT.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure_OUT.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure_OUT);

  GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);


  int i = 0;

  while(1)
  {
	  /* the built in led on PC13 blinks with 1 second duration */
	  /* turn on PC13 */
	  GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);

	  /* delay 1 sec */
	  for (i=0;i<10000000;i++){

	  }
	  /* turn off PC13 */
	  GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);

	  /* delay 1 sec */
	  for (i=0;i<10000000;i++){

	  }
  }

  return 0;
}

