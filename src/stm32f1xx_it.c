/**
  ******************************************************************************
  * @file    stm32f1xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_it.h"



/** @addtogroup IO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t repetition_counter = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M Processor Exceptions Handlers                          */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_md.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */
void EXTI0_IRQHandler(void){
	EXTI->PR = EXTI_Line0;
	repetition_counter++;

	//EXTI_ClearITPendingBit(EXTI_Line1);
	if (repetition_counter==1){
//		TIM3->CCR3=18+(repetition_counter%2)*25;
//		TIM3->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
		TIM3->CCR3=43;
//		TIM3->CR1 |= TIM_CR1_CEN;
		return;
	}
	if (repetition_counter==16){
		//TIM3->CCR3=18+(repetition_counter%2)*25;
		//TIM3->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
		TIM3->CCR3=18;
		//TIM3->CR1 |= TIM_CR1_CEN;
		return;
	}

	if(repetition_counter==23){
			//TIM3->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
		for(uint32_t i = 0; i<1;i++){
		}
		TIM3->CCR3=0;
			//TIM3->CR1 |= TIM_CR1_CEN;
	}

	//repetition_counter = 0;
	/*if (~(repetition_counter%72)){
		//TIM3->CCR3=0;
	}
	repetition_counter++;
	//GPIOC->ODR ^= (GPIO_Pin_13);*/
}


/**
  * @brief  This function handles the button interrupt request on portB, pin14.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void){
  EXTI_ClearITPendingBit(EXTI_Line14);

  if((GPIOB->IDR & GPIO_Pin_14)){
	GPIOC->ODR |= GPIO_Pin_13;
	//GPIOC->ODR |= GPIO_Pin_15;
  }else{
	GPIOC->ODR &= (~GPIO_Pin_13);
	//GPIOC->ODR &= (~GPIO_Pin_15);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
