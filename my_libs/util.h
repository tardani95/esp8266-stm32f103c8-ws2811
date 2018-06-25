/*
******************************************************************************
File        : util.h
Author      : Peter Vass - modified by Daniel Tar
Version     :
Copyright   :
Description :
Info        : 17.06.2018
*******************************************************************************
*/

#ifndef _UTIL_H_
#define _UTIL_H_


#include "stm32f10x.h"
#include "stm32f10x_rcc.h" /* Reset & Clock Control */

#define SYS_TICK_FREQ (1000000) /* frequency of sys_tick event in Hz - used by InitSystick() function */

typedef void (*callback)(void); /* callback function definition */

void SysTick_Handler(void);
void InitSysTick(void);
uint32_t GetSysTickCount(void);
uint32_t Millis(void);
void DelayUs(uint32_t us);
void DelayMs(uint32_t ms);
void DelaySec(uint32_t sec);

#endif /* _UTIL_H_ */
