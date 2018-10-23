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
		i_percent_palette_length_plus_offset = (pxID % palette_length) + 1;
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

/**
  * @brief  This function modifies the lookup table so, that it makes a fade in fade out animation
  * @param  fade_in_time: it will fade in in 255*fade_in_time [ms]
  * @param  hold_time: hold the color at the same value for hold_time [ms]
  * @param  fade_out_time:it will fade out in 255*fade_out_time [ms]
  * @retval None
  */
void anim_fadeInFadeOut(uint16_t fade_in_time, uint16_t hold_time, uint16_t fade_out_time){
	for(uint8_t i=0;i<7;++i){
		for(uint16_t temp = 0; temp<256; temp+=1){
			for(uint8_t pLSid = 0; pLSid < PARALELL_STRIPS; pLSid++){
				switch(i){
					case 0: setAllPixelColorOnLedStrip(pLSid,temp,0,0); break;
					case 1: setAllPixelColorOnLedStrip(pLSid,0,temp,0); break;
					case 2: setAllPixelColorOnLedStrip(pLSid,0,0,temp); break;
					case 3: setAllPixelColorOnLedStrip(pLSid,temp,temp,0); break;
					case 4: setAllPixelColorOnLedStrip(pLSid,0,temp,temp); break;
					case 5: setAllPixelColorOnLedStrip(pLSid,temp,0,temp); break;
					case 6: setAllPixelColorOnLedStrip(pLSid,temp,temp,temp); break;
					default: break;
				}
			}
			refreshLedStrip();
			DelayMs(fade_in_time);
		}
		DelayMs(hold_time);
		/* watch out! negative overflow! --> int16_t */
		for(int16_t temp = 255; temp>=0; temp-=1){
			for(uint8_t pLSid = 0; pLSid < PARALELL_STRIPS; pLSid++){
				switch(i){
					case 0: setAllPixelColorOnLedStrip(pLSid,temp,0,0); break;
					case 1: setAllPixelColorOnLedStrip(pLSid,0,temp,0); break;
					case 2: setAllPixelColorOnLedStrip(pLSid,0,0,temp); break;
					case 3: setAllPixelColorOnLedStrip(pLSid,temp,temp,0); break;
					case 4: setAllPixelColorOnLedStrip(pLSid,0,temp,temp); break;
					case 5: setAllPixelColorOnLedStrip(pLSid,temp,0,temp); break;
					case 6: setAllPixelColorOnLedStrip(pLSid,temp,temp,temp); break;
					default: break;
				}
			}
			refreshLedStrip();
			DelayMs(fade_out_time);
		}
	}
}

void anim_strobe(ColorHex color, uint16_t StrobeCount, uint16_t FlashDelay, uint16_t EndPause){

	for(uint16_t j = 0; j < StrobeCount; j++) {
		for(uint8_t pLSid = 0; pLSid < PARALELL_STRIPS; ++pLSid){
			setAllPixelColorHexOnLedStrip(pLSid, color);
		}
		refreshLedStrip();
		DelayMs(FlashDelay);
		for(uint8_t pLSid = 0; pLSid < PARALELL_STRIPS; ++pLSid){
			setAllPixelColorHexOnLedStrip(pLSid, 0x000000);
		}
		refreshLedStrip();
		DelayMs(FlashDelay);
	}
	DelayMs(EndPause);
}

void anim_bouncingBallsOnLedStrip(uint8_t parallelLedStripID ,uint32_t timeout_in_us ,uint8_t BallCount, ColorHex colors[]) {

	float Gravity = -9.81;
	uint16_t StartHeight = 10;

	float Height[BallCount];
	float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
	float ImpactVelocity[BallCount];
	float TimeSinceLastBounce[BallCount];
	uint16_t   Position[BallCount];
	uint32_t  ClockTimeSinceLastBounce[BallCount];
	float Dampening[BallCount];

	for (uint8_t i = 0 ; i < BallCount ; i++) {
		ClockTimeSinceLastBounce[i] = Millis();
		Height[i] = StartHeight;
		Position[i] = 0;
		ImpactVelocity[i] = ImpactVelocityStart;
		TimeSinceLastBounce[i] = 0;
		Dampening[i] = 0.90 - (float)i/pow(BallCount,2);
	}

	uint32_t startTime = GetSysTickCount();

	while (GetSysTickCount() < startTime+timeout_in_us) {

		for (uint8_t i = 0 ; i < BallCount ; i++) {

			TimeSinceLastBounce[i] =  Millis() - ClockTimeSinceLastBounce[i];
			Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 )
							+ ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;

			if ( Height[i] < 0 ) {

				Height[i] = 0;
				ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
				ClockTimeSinceLastBounce[i] = Millis();

				if ( ImpactVelocity[i] < 0.01 ) {
					ImpactVelocity[i] = ImpactVelocityStart;
				}
			}
			Position[i] = round( Height[i] * (LED_STRIP_SIZE - 1) / StartHeight);
		}

		for (uint8_t i = 0 ; i < BallCount ; i++) {
			setPixelColorHex(Position[i],parallelLedStripID,colors[i]);
		}

		refreshLedStrip();
		setAllPixelColorHexOnLedStrip(parallelLedStripID, 0x000000);
	}
}

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
