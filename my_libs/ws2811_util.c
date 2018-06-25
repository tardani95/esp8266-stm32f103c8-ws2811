/*
******************************************************************************
File        : ws2811_util.c
Author      : Daniel Tar - ideas based on Peter Vass implementation
Version     :
Copyright   :
Description : ws2811 utility library
Info        : 22.06.2018
*******************************************************************************
*/

/******************************************************************************/
/*                               Includes									  */
/******************************************************************************/
#include "ws2811_util.h"

void notIncomingCall(ColorHex color){

}
void notSMS(ColorHex color){

}

void notification(uint8_t mode, ColorHex color){

}

void fillSolid(ColorRGB color){
	for( uint8_t parallelLedStripID = 0; parallelLedStripID < PARALELL_STRIPS; ++parallelLedStripID ){
		setAllPixelColorRGBOnLedStrip(parallelLedStripID, color);
	}
}

void fillPattern(uint8_t paletteID){
	uint16_t palette_length = palettes[paletteID][0];
	ColorRGB pxColor;
	uint8_t i_percent_palette_length_plus_offset;

	for( uint16_t pxID = 0 ; pxID < LED_STRIP_SIZE ; ++pxID ){
		i_percent_palette_length_plus_offset = (pxID%palette_length) + 1;
		for( uint8_t parallelLedStripID = 0; parallelLedStripID < PARALELL_STRIPS; ++parallelLedStripID ){
			pxColor.r = ((palettes[paletteID][i_percent_palette_length_plus_offset] & RED  ) >> RED_S);
			pxColor.g = ((palettes[paletteID][i_percent_palette_length_plus_offset] & GREEN) >> GREEN_S);
			pxColor.b = ((palettes[paletteID][i_percent_palette_length_plus_offset] & BLUE ));

			setPixelColorRGB(pxID, parallelLedStripID, pxColor);
		}
	}
}

void fill(uint8_t mode, uint32_t value){}

void anim_pattern(){}

void anim_bouncingBalls(){}

void anim_meteorRainOnAllLedStrip(ColorHex meteorColor, uint8_t meteorSize, uint8_t meteorTrailDecay,
		uint8_t meteorRandomDecay, uint16_t SpeedDelay_ms){

	srand(meteorColor);
	ColorHex black = 0x000000;
	setAllPixelColorHexOnLedStrip(0, black);
	setAllPixelColorHexOnLedStrip(1, black);
	setAllPixelColorHexOnLedStrip(2, black);

	for(int i = 0; i < LED_STRIP_SIZE + LED_STRIP_SIZE; i++) {

		for(uint8_t pLSid = 0; pLSid < PARALELL_STRIPS; pLSid++){
			// fade brightness all LEDs one step
			for(int j=0; j<LED_STRIP_SIZE; j++) {
				if( (!meteorRandomDecay) || ((rand()%10)>5) ) {
					fadeToBlack(j, pLSid, meteorTrailDecay );
				}
			}

			// draw meteor
			for(int j = 0; j < meteorSize; j++) {
				if( ( i-j <LED_STRIP_SIZE) && (i-j>=0) ) {
					setPixelColorHex(i-j, pLSid, meteorColor);
				}
			}
		}
		refreshLedStrip();
		DelayMs(SpeedDelay_ms);
	}
}

void anim_meteorRainOnLedStrip(uint8_t parallelLedStripID,
			ColorHex meteorColor, uint8_t meteorSize, uint8_t meteorTrailDecay,
			uint8_t meteorRandomDecay, uint16_t SpeedDelay_ms) {

	srand(meteorColor);
	ColorHex black = 0x000000;
	setAllPixelColorHexOnLedStrip(parallelLedStripID, black);

	for(int i = 0; i < LED_STRIP_SIZE + LED_STRIP_SIZE; i++) {
		// fade brightness all LEDs one step
		for(int j=0; j<LED_STRIP_SIZE; j++) {
			if( (!meteorRandomDecay) || ((rand()%10)>5) ) {
				fadeToBlack(j, parallelLedStripID, meteorTrailDecay );
			}
		}

		// draw meteor
		for(int j = 0; j < meteorSize; j++) {
			if( ( i-j <LED_STRIP_SIZE) && (i-j>=0) ) {
				setPixelColorHex(i-j, parallelLedStripID, meteorColor);
			}
		}

		refreshLedStrip();
		DelayMs(SpeedDelay_ms);
	}
}

void fadeToBlack(uint16_t pxNr, uint8_t parallelLedStripID, uint8_t fadeValue) {
    ColorRGB oldColor;
    uint8_t r, g, b;

    oldColor = getPixelColorRGB(pxNr, parallelLedStripID);
    r = oldColor.r;
    g = oldColor.g;
    b = oldColor.b;

    r=(r<=10)? 0 : (uint8_t) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (uint8_t) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (uint8_t) b-(b*fadeValue/256);

    setPixelColor(pxNr, parallelLedStripID, r, g, b);
}
