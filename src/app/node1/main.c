/*
* main.c
*
* Created on: Dec 9, 2012
* Author: igor
*/
#include "platform.h"


PLATFORM_CONFIGURATION()
{
	AGENTS_CONFIGURATION(){
		//AGENT_INIT(0x00, 0x03, 1101000000000000011100000000000000110000000000011111001111111101),
		//AGENT_INIT(0x00, 0x03, 1101000000000000011100000000000000000111000000011111001111111101)
		//AGENT_INIT(0x01, 0x03, 11010000111100000111000000000000),
		//AGENT_INIT(0x02, 0x03, 110100001111000001110000000000001101000011110001011100000000000011010000111100100111000000000000),
		//AGENT_INIT(0x03, 0x03, 11010000111100000111000000000000)
		AGENT_INIT(0x01, 0x03, 0100001000000001010000010000000001110001000000000011000100000001000011010001000010100010000000001111001111111011)
	},
	PLATFORM_ID(0x02),
	BOARD_ID(0x00)
};
