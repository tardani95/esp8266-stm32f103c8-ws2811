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

void notIncomingCall(){}
void notSMS(){}

void notification(uint8_t mode, uint32_t color){}

void fill_solid(){}
void fill_pattern(uint8_t palette){}

void fill(uint8_t mode, uint32_t value){}

void anim_pattern(){}
void anim_bouncingBalls(){}

void anim_meteorRain(){
}


void anim_meteorRainOnLedStrip(uint8_t parrallelLedStripID,
			ColorHex meteorColor, uint8_t meteorSize, uint8_t meteorTrailDecay,
			uint8_t meteorRandomDecay, uint16_t SpeedDelay_ms) {

	srand(meteorColor);
	ColorHex black = 0x000000;
	setAllPixelColorHexOnLedStrip(parrallelLedStripID, black);

	for(int i = 0; i < LED_STRIP_SIZE + LED_STRIP_SIZE; i++) {
		// fade brightness all LEDs one step
		for(int j=0; j<LED_STRIP_SIZE; j++) {
			if( (!meteorRandomDecay) || ((rand()%10)>5) ) {
				fadeToBlack(j, parrallelLedStripID, meteorTrailDecay );
			}
		}

		// draw meteor
		for(int j = 0; j < meteorSize; j++) {
			if( ( i-j <LED_STRIP_SIZE) && (i-j>=0) ) {
				setPixelColorHex(i-j, parrallelLedStripID, meteorColor);
			}
		}

		refreshLedStrip();
		DelayMs(SpeedDelay_ms);
	}
}

void fadeToBlack(uint16_t pxNr, uint8_t parrallelLedStripID, uint8_t fadeValue) {
//    uint32_t oldColor;
    ColorRGB oldColor;
    uint8_t r, g, b;

    oldColor = getPixelColorRGB(pxNr, parrallelLedStripID);
    r = oldColor.r; //(oldColor & 0x00ff0000UL) >> 16;
    g = oldColor.g; //(oldColor & 0x0000ff00UL) >> 8;
    b = oldColor.b; //(oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (uint8_t) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (uint8_t) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (uint8_t) b-(b*fadeValue/256);

    setPixelColor(pxNr, parrallelLedStripID, r, g, b);
}
