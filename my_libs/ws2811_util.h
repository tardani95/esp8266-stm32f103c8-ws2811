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


void notIncomingCall(ColorHex color);
void notSMS(ColorHex color);

void notification(uint8_t mode, uint32_t color);

void fillSolid(ColorRGB color);
void fillPattern(uint8_t paletteID);

void fill(uint8_t mode, uint32_t value);

void anim_pattern();

void anim_fadeInFadeOut(uint16_t fade_in_time, uint16_t hold_time, uint16_t fade_out_time);

void anim_bouncingBalls();

void anim_meteorRainOnAllLedStrip(
		ColorHex meteorColor,
		uint8_t meteorSize,
		uint8_t meteorTrailDecay,
		uint8_t meteorRandomDecay,
		uint16_t SpeedDelay_ms);

void anim_meteorRainOnLedStrip(
		uint8_t 	parallelLedStripID,
		ColorHex 	meteorColor,
		uint8_t 	meteorSize,
		uint8_t 	meteorTrailDecay,
		uint8_t 	meteorRandomDecay,
		uint16_t 	SpeedDelay_ms);

void fadeToBlack(uint16_t pxNr, uint8_t parallelLedStripID, uint8_t fadeValue);

#endif /* _WS2811_UTIL_H_ */
