/*
******************************************************************************
File        : ws2811_util.h
Author      : Daniel Tar
Version     :
Copyright   :
Description : ws2811 utility library
Info        : 22.06.2018
*******************************************************************************
*/

#ifndef _WS2811_UTIL_H_
#define _WS2811_UTIL_H_
/*========================================================================*/
/*                             INCLUDES									  */
/*========================================================================*/
#include "ws2811.h"
#include "color_palettes.h"
#include <stdlib.h>         /* for rand() function */

void notIncomingCall();
void notSMS();

void notification(uint8_t mode, uint32_t color);

void fillSolid();
void fillPattern(uint8_t paletteID);

void fill(uint8_t mode, uint32_t value);

void anim_pattern();
void anim_bouncingBalls();
void anim_meteorRain();

//typedef uint32_t ColorHex;

void anim_meteorRainOnLedStrip(
		uint8_t 	parrallelLedStripID,
		uint32_t 	meteorColor,
		uint8_t 	meteorSize,
		uint8_t 	meteorTrailDecay,
		uint8_t 	meteorRandomDecay,
		uint16_t 	SpeedDelay_ms);

void fadeToBlack(uint16_t pxNr, uint8_t parrallelLedStripID, uint8_t fadeValue);

#endif /* _WS2811_UTIL_H_ */
