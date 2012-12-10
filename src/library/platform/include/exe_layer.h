/*
* exe_layer.h
*
* Created on: Dec 10, 2012
* Author: igor
*/

#ifndef EXE_LAYER_H_
#define EXE_LAYER_H_

#include "platform.h"

#define SETSERVICE 0
#define GETSERVICE 1
#define STORE	   2

#define SERVICE_BARGRAPH 0

#define ERROR_NO_SERVICE_PRESENT 1


#define N1_MASK 0xF000
#define N2_MASK 0x0F00
#define N3_MASK 0x00F0
#define N4_MASK 0x000F
#define B1_MASK 0xFF00
#define B2_MASK 0x00FF

typedef struct {
	uint8_t id;
	uint8_t reg1;
	uint8_t reg2;
	uint16_t value;
	uint8_t node_id;
	uint8_t agent_id;
} opcode_t;

uint8_t execute_agent(agent_t *agent, uint8_t opcode_size);
opcode_t decode_opcode(uint16_t opcode);
void execute_opcode(agent_t *agent, opcode_t opcode);

#endif

