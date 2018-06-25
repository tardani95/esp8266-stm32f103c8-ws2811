/*
******************************************************************************
File        : color_palettes.c
Author      : Daniel Tar
Version     :
Copyright   :
Description :
Info        : 23.06.2018
*******************************************************************************
*/

/*========================================================================*/
/*                             INCLUDES									  */
/*========================================================================*/
#include "color_palettes.h"

/******************************************************************************/
/*                       Color Palettes in RGB hex code						  */
/******************************************************************************/

// 0 - initial palette
uint32_t init_palette[]={
		8+5,
		0x00FF00, 0x7d007d, 0xFF0000, 0x7d7d00,
		0x0000FF, 0x7d007d, 0xFFFFFF, 0x000000,
		0xFF0000, 0x00FF00, 0x0000FF, 0x7d007d, 0x7d7d00
};

// 1 - Rainbow colors
uint32_t rainbow[] =
{
	7,
	0x9400D3,0x4B0082,0x0000FF,0x00FF00,0xFFFF00,0xFF7F00,0xFF0000
};


// 2 - Rainbow colors with alternating stripes of black
uint32_t rainbowStripe[] =
{
	16,
    0xFF0000, 0x000000, 0xAB5500, 0x000000, 0xABAB00, 0x000000, 0x00FF00, 0x000000,
    0x00AB55, 0x000000, 0x0000FF, 0x000000, 0x5500AB, 0x000000, 0xAB0055, 0x000000
};

// 3 - Blue purple ping red orange yellow (and back)
// Basically, everything but the greens.
// This palette is good for lighting at a club or party.
uint32_t party[] =
{
	16,
    0x5500AB, 0x84007C, 0xB5004B, 0xE5001B,
    0xE81700, 0xB84700, 0xAB7700, 0xABAB00,
    0xAB5500, 0xDD2200, 0xF2000E, 0xC2003E,
    0x8F0071, 0x5F00A1, 0x2F00D0, 0x0007F9
};

// 4 - Approximate "black body radiation" palette, akin to
// the FastLED 'Heatuint32_t' function.
// Recommend that you use values 0-240 rather than
// the usual 0-255, as the last 15 colors will be
// 'wrapping around' from the hot end to the cold end,
// which looks wrong.
uint32_t heat[] =
{
	4,
    0x000000, 0xFF0000, 0xFFFF00, 0xFFFFCC
};

// 5
uint32_t fire[] =
{
	6,
    0x000000, 0x220000,
    0x880000, 0xFF0000,
    0xFF6600, 0xFFCC00
};

// 6
uint32_t hun[]=
{
		4,
		0xff0000,0xffffff,0x00ff00,0x000000
};

// 7
uint32_t ro[]={
		4,
		0xff0000,0xffff00,0x0000ff,0x000000
};

// 8


/******************************************************************************/

uint32_t * palettes[PALETTES_SIZE]={
	init_palette,
	rainbow,
	rainbowStripe,
	party,
	heat,
	fire,
	hun,
	ro
};

