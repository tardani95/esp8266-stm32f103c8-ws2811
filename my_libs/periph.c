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
#include "ws2811.h"
/******************************************************************************/
/*                               Variables									  */
/******************************************************************************/
uint16_t led_pin	= GPIO_Pin_13; 		/* PC13 */
uint16_t button_pin = GPIO_Pin_14; 		/* PB14 */
uint16_t ledstrip_signal1 = GPIO_Pin_0; 	/* PB0  */
uint16_t ledstrip_signal2 = GPIO_Pin_1; 	/* PB1  */
uint16_t pwm_exti_pin 	= GPIO_Pin_1;	/* PA1  */
__IO uint8_t  look_up_table_1[LOOK_UP_TABLE_SIZE*3];
__IO uint8_t  look_up_table_2[LOOK_UP_TABLE_SIZE];

/******************************************************************************/
/*            			         Functions                                    */
/******************************************************************************/

/**
  * @brief  This function freeze the cpu for x [us]
  * @param  time in [us]
  * @retval None
  */
void delayMicroSec(uint32_t us){
	us *= 5.15;
	for(uint32_t i = 0; i<us;i++){
	}
}

/**
  * @brief  This function freeze the cpu for x [sec]
  * @param  time in [sec]
  * @retval None
  */
void delaySec(uint32_t sec){
	sec *= 5.15*1000000;
	for(uint32_t i = 0; i<sec;i++){
	}
}

/**
  * @brief  This function initialize the lookup table - it is in BRG dimension
  * @param  None
  * @retval None
  */
void InitLookUpTable(void){
	uint8_t look_up_table_3[4][3]={{0,150,0},{125,0,125},{0,0,255},{125,125,0}};
	for(uint16_t i=0;i<LOOK_UP_TABLE_SIZE/24;++i){
		for(uint16_t j=0;j<3;++j){
			for(uint16_t k = 0; k<8; ++k){
				look_up_table_2[i*24 + j*8 + k] = look_up_table_3[i%4][j] & (0x80 >> k);
			}
		}
	}
}

/**
  * @brief  This function updates the lookup table 1
  * @param  None
  * @retval None
  */
void RefreshLookUpTable1(uint8_t first_x_byte){
	uint8_t val = 0;
	for( uint16_t i=0 ; i < 24 * first_x_byte ; ++i){
		val = look_up_table_2[i] ? 43 : 18;
		look_up_table_1[i*3] = val;
		look_up_table_1[i*3+1] = val;
		look_up_table_1[i*3+2] = val;
	}
}

/**
  * @brief  This function updates the lookup table
  * @param  R: Red value in range 0-255
  * @param  G: Green value in range 0-255
  * @param  B: Blue value in range 0-255
  * @retval None
  */
void RefreshLookUpTable(uint8_t r_t, uint8_t g_t, uint8_t b_t){
//	uint8_t look_up_table_3[4][3]={{b_t,r_t,g_t},{b_t,r_t,g_t},{b_t,r_t,g_t},{b_t,r_t,g_t}};
//	for(uint16_t i=0;i<LOOK_UP_TABLE_SIZE/24;++i){
//		for(uint16_t j=0;j<3;++j){
//			for(uint16_t k = 0; k<8; ++k){
//				look_up_table_2[i*24 + j*8 + k] = look_up_table_3[i%4][j] & (0x80 >> k);
//			}
//		}
//	}
	for( uint16_t i = 0 ; i < LED_STRIP_SIZE ; ++i ){
		for( uint8_t j = 0; j < PARALELL_STRIPS; ++j ){
			pixel_mapBRG[j][i][R] = /*r_t;*/ gammaCorrection(r_t);
			pixel_mapBRG[j][i][G] = /*g_t;*/ gammaCorrection(g_t);
			pixel_mapBRG[j][i][B] = /*b_t;*/ gammaCorrection(b_t);
		}
	}
}

/**
  * @brief  This function modifies the lookup table so, that it makes a fade in fade out animation
  * @param  fade_in_time: it will fade in in 255*fade_in_time [us]
  * @param  hold_time: hold the color at the same value for hold_time [us]
  * @param  fade_out_time:it will fade out in 255*fade_out_time [us]
  * @retval None
  */
void AnimFadeInFadeOut(uint16_t fade_in_time, uint16_t hold_time, uint16_t fade_out_time){
	for(uint8_t i=0;i<7;++i){
		for(uint16_t temp = 0; temp<256; temp+=1){
			switch(i){
				case 0: RefreshLookUpTable(temp,0,0); break;
				case 1: RefreshLookUpTable(0,temp,0); break;
				case 2: RefreshLookUpTable(0,0,temp); break;
				case 3: RefreshLookUpTable(temp,temp,0); break;
				case 4: RefreshLookUpTable(0,temp,temp); break;
				case 5: RefreshLookUpTable(temp,0,temp); break;
				case 6: RefreshLookUpTable(temp,temp,temp); break;
				default: break;
			}
			delayMicroSec(fade_in_time);

		}
		delayMicroSec(hold_time);
		/* watch out! negative overflow! --> int16_t */
		for(int16_t temp = 255; temp>=0; temp-=1){
			switch(i){
				case 0: RefreshLookUpTable(temp,0,0); break;
				case 1: RefreshLookUpTable(0,temp,0); break;
				case 2: RefreshLookUpTable(0,0,temp); break;
				case 3: RefreshLookUpTable(temp,temp,0); break;
				case 4: RefreshLookUpTable(0,temp,temp); break;
				case 5: RefreshLookUpTable(temp,0,temp); break;
				case 6: RefreshLookUpTable(temp,temp,temp); break;
				default: break;
			}
			delayMicroSec(fade_out_time);

		}
	}
}

/**
  * @brief  This function initialize the LED on PC13 pin
  * @param  GPIO_InitTypeDef variable address
  * @retval None
  */
void InitGPIO_LED(GPIO_InitTypeDef* GPIO_InitStructure){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_StructInit(GPIO_InitStructure);

	GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure->GPIO_Pin   = led_pin;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, GPIO_InitStructure);
}

/**
  * @brief  This function initialize the button on PB14 pin
  * @param  GPIO_InitTypeDef variable address
  * @retval None
  */
void InitGPIO_BTN(GPIO_InitTypeDef* GPIO_InitStructure){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_StructInit(GPIO_InitStructure);

	GPIO_InitStructure->GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure->GPIO_Pin   = button_pin;
	GPIO_InitStructure->GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, GPIO_InitStructure);
}

/**
  * @brief  This function initialize the external interrupt on PB14 and sets the nested vectored interrupt controller
  * @param  EXTI_InitStructure: pointer to a EXTI_InitTypeDef structure which will
  *         be initialized.
  *         NVIC_InitStructure: pointer to a NVIC_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void InitEXTI_BTN(EXTI_InitTypeDef* EXTI_InitStructure, NVIC_InitTypeDef* NVIC_InitStructure){
	EXTI_StructInit(EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);

	EXTI_InitStructure->EXTI_Line = EXTI_Line14;
	EXTI_InitStructure->EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure->EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure->EXTI_LineCmd = ENABLE;
	EXTI_Init(EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	/* NVIC_PriorityGroup_4: 4 bits for pre-emption priority
	 *                       0 bits for subpriority*
	 */

	NVIC_InitStructure->NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure->NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure->NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure->NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(NVIC_InitStructure);
}
