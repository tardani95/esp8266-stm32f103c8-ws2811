/*
******************************************************************************
File        : color_palettes.h
Author      : Daniel Tar
Version     :
Copyright   :
Description : ws2811 utility library
Info        : 22.06.2018
*******************************************************************************
*/

#ifndef _COLOR_PALETTES_H_
#define _COLOR_PALETTES_H_
/*========================================================================*/
/*                             INCLUDES									  */
/*========================================================================*/
#include "stm32f10x.h"

extern uint32_t init_palette[];
extern uint32_t rainbow[];
extern uint32_t rainbowStripe[];
extern uint32_t party[];
extern uint32_t heat[];
extern uint32_t fire[];
extern uint32_t hun[];
extern uint32_t ro[];

#define PALETTES_SIZE 8
extern uint32_t * palettes[PALETTES_SIZE];

#endif /* _COLOR_PALETTES_H_ */
