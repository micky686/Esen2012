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
		//AGENT_INIT(0x03, 0x03, 0100001000000001010000010000000001110001000000000011000100000001000011010001000010100010000000001111001111111011)
		//AGENT_INIT(0x01, 0x03, 11010000111100000111000000000000),
		//AGENT_INIT(0x02, 0x03, 110100001111000001110000000000001101000011110001011100000000000011010000111100100111000000000000),
		//AGENT_INIT(0x03, 0x03, 11010000111100000111000000000000)
		//AGENT_INIT(0x03, 0x03, 01000010000000010100000100000000111100100000000001110001000000000011000100000001000011010001000010100010000000001111001111111011)
		//AGENT_INIT(0x01, 0x03, 101110000100100110111000010001111011100001001111101110000101001010111000010010011011100001000111101110000100011110111000010010011011100001000111101110000100111110111000010100101111100010000101)
		//AGENT_INIT(0x01, 0x03, 10111000010010011011100001000111101110000100111110111000010100101111100010000001)
		//AGENT_INIT(0x01, 0x03, 1011100001001001101110000100011110111000010011111011100001010010111110001000000101000000111111111111110011111101)
		//AGENT_INIT(0x01, 0x03, 10111000010010011011100001000111111110001000000101000000111111111111110011111101)

		//AGENT_INIT(0x01, 0x03, 10111000001100001111100010000001000011010001100000110001000000010000110100010000000000100000100010100001001110011111001111111000000011011000000111111000100000010100000000000001111100111111011010111000010010011011100001000111111110001000000101000000111111111111110011111101)
		//AGENT_INIT(0x01, 0x03, 1011100000110000111110001000000100001101000110000011000100000001000011010001000000000101011001000000001000001000101000010011100111110011111101110000110110000001111110001000000101000000000000011111001111110101),
		//AGENT_INIT(0x02, 0x01, 1011100000110000101110000011000110111000001100101011100000110011101110000011010010111000001101011011100000110110101110000011011110111000001110001111100010000001000001010011001001000000000000011111001111111100)

		//AGENT_INIT(0x00, 0x03, 111100010000000110100000000000001111011000000001111101000000000000000111000000011111100011010001000001010001111001000000000000011111001111111011)
		AGENT_INIT(0x00, 0x03, 1111000100000001101000000000000011110110000000011111010000000000010000010001110011000001000010000000110100010000010010000101000000000111000000010000101000010000111111000000000101001000000000000111100000000010000001010001111001000000000000011111001111110111),
		AGENT_INIT(0x01, 0x03, 1111000100000001101000000000000011110110000000011111010000000000010000010001110011000001000010000000110100010000010010000000101000000111000000010000101000000001111111000000000101001000000000000111100000000011000001010001111001000000000000011111001111110111)
	},
	//PLATFORM_ID(0x04),
	BOARD_ID(0x00)
};
