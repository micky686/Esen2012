/*
 * main.c
 *
 *  Created on: Dec 8, 2012
 *      Author: igor
 */
#include "platform.h"


PLATFORM_CONFIGURATION()
{
	AGENTS_CONFIGURATION(){
		//AGENT_INIT(0x00, 0x03, 111110100000100011111100111111100111100000000100)
		//AGENT_INIT(0x00, 0x03, 101110000100100110111000010001110111100000000100)
		//AGENT_INIT(0x00, 0x03, 10111000010010011011100001000111101110000100100110111000010001110111100000000100)
		AGENT_INIT(0x00, 0x03, 11111010000010001111110011111110011110000000010001000000111111111111110011111010)
	},

	PLATFORM_ID(0x01),
	BOARD_ID(0x00)
};
