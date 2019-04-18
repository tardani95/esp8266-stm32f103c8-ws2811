/*
 * periph.c
 *
 *  Created on: Apr 25, 2018
 *      Author: tar
 */

/******************************************************************************/
/*                               Includes									  */
/******************************************************************************/
#include "periph.h"
#include "eeprom.h"
#include "ws2811.h"
#include "ws2811_util.h"
/******************************************************************************/
/*                               Variables									  */
/******************************************************************************/
uint16_t led_pin = GPIO_Pin_13; /* PC13 */
uint16_t button_pin0 = GPIO_Pin_12; /* PB12 */
uint16_t button_pin1 = GPIO_Pin_13; /* PB13 */
uint16_t button_pin2 = GPIO_Pin_14; /* PB14 */

__IO uint8_t button0_state = STD_MODE;
__IO uint8_t button_palette_pointer = 0;
__IO uint8_t button_palette_size;

uint16_t VirtAddVarTab2[] = { 0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005 };

/******************************************************************************/
/*            			         Functions                                    */
/******************************************************************************/

void Init_EEPROM() {
	/* Unlock the Flash Program Erase controller */
	FLASH_Unlock();
	/* EEPROM Init */
	EE_Init();
	/* Lock the Flash Program Erase controller */
	FLASH_Lock();
}

/**
 * @brief  This function initialize the LED on PC13 pin
 * @param  GPIO_InitTypeDef variable address
 * @retval None
 */
void InitGPIO_LED(GPIO_InitTypeDef* GPIO_InitStructure) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_StructInit(GPIO_InitStructure);

	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure->GPIO_Pin = led_pin;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, GPIO_InitStructure);
}

/**
 * @brief  This function initialize the button on PB12,PB13,PB14 pin
 * @param  GPIO_InitTypeDef variable address
 * @retval None
 */
void InitGPIO_BTN(GPIO_InitTypeDef* GPIO_InitStructure) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_StructInit(GPIO_InitStructure);

	GPIO_InitStructure->GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_InitStructure->GPIO_Pin = button_pin0;
	GPIO_Init(GPIOB, GPIO_InitStructure);

	GPIO_InitStructure->GPIO_Pin = button_pin1;
	GPIO_Init(GPIOB, GPIO_InitStructure);

	GPIO_InitStructure->GPIO_Pin = button_pin2;
	GPIO_Init(GPIOB, GPIO_InitStructure);
}

/**
 * @brief  This function initialize the external interrupt on PB12,PB13,PB14 and sets the nested vectored interrupt controller
 * @param  EXTI_InitStructure: pointer to a EXTI_InitTypeDef structure which will
 *         be initialized.
 *         NVIC_InitStructure: pointer to a NVIC_InitTypeDef structure which will
 *         be initialized.
 * @retval None
 */
void InitEXTI_BTN(EXTI_InitTypeDef* EXTI_InitStructure,
		NVIC_InitTypeDef* NVIC_InitStructure) {

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	/* NVIC_PriorityGroup_4: 4 bits for pre-emption priority
	 *                       0 bits for subpriority*
	 */

	NVIC_InitStructure->NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure->NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure->NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure->NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_Init(NVIC_InitStructure);

	EXTI_StructInit(EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);

	EXTI_InitStructure->EXTI_Line = EXTI_Line12 | EXTI_Line13 | EXTI_Line14;
	EXTI_InitStructure->EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure->EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure->EXTI_LineCmd = ENABLE;
	EXTI_Init(EXTI_InitStructure);


	button_palette_pointer = 0;
	if (button0_state) {
		button_palette_size = rainbow[0];
		fillSolid(colorHexToRGB(rainbow[1]));
	} else {
		button_palette_size = PALETTES_SIZE;
		fillPattern(4); /*party palette*/
	}

//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
//
//	EXTI_InitStructure->EXTI_Line = EXTI_Line13;
//	EXTI_InitStructure->EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure->EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure->EXTI_LineCmd = ENABLE;
//	EXTI_Init(EXTI_InitStructure);
//
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
//
//	EXTI_InitStructure->EXTI_Line = EXTI_Line14;
//	EXTI_InitStructure->EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure->EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure->EXTI_LineCmd = ENABLE;
//	EXTI_Init(EXTI_InitStructure);

}

void EXTI15_10_IRQHandler() {

	if (SET == EXTI_GetITStatus(EXTI_Line12)) {
		button0_state = !button0_state;

		button_palette_pointer = 0;
		if (button0_state) {
			button_palette_size = rainbow[0];
			fillSolid(colorHexToRGB(rainbow[1]));
		} else {
			button_palette_size = PALETTES_SIZE;
			fillPattern(4); /*party palette*/
		}

		if ((GPIOB->IDR & GPIO_Pin_14)) {
			GPIOC->ODR |= GPIO_Pin_13; /* led not light */
			//GPIOC->ODR |= GPIO_Pin_15;
		} else {
			GPIOC->ODR &= (~GPIO_Pin_13); /* led light */
			//GPIOC->ODR &= (~GPIO_Pin_15);
		}

		EXTI_ClearITPendingBit(EXTI_Line12);
	}

	if (SET == EXTI_GetITStatus(EXTI_Line13)) {
		button_palette_pointer++;

		if (button0_state) {
			fillSolid(
					colorHexToRGB(
							rainbow[(button_palette_pointer
									% button_palette_size) + 1]));
		} else {
			fillPattern(button_palette_pointer % button_palette_size);
		}

		EXTI_ClearITPendingBit(EXTI_Line13);
	}

	if (SET == EXTI_GetITStatus(EXTI_Line14)) {
		button_palette_pointer--;

		if (button0_state) {
//			if (button_palette_pointer < 1) {
//				button_palette_pointer = button_palette_size - 1;
//			}

			fillSolid(
					colorHexToRGB(
							rainbow[(button_palette_pointer
									% button_palette_size) + 1]));
		} else {
//			if (button_palette_pointer > button_palette_size) {
//				button_palette_pointer = button_palette_size - 1;
//			}
			fillPattern(button_palette_pointer % button_palette_size);
		}

		EXTI_ClearITPendingBit(EXTI_Line14);
	}

	refreshLedStrip();
}
