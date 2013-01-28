/*
* exe_layer.h
*
* Created on: Dec 10, 2012
* Author: igor
*/

#ifndef EXE_LAYER_H_
#define EXE_LAYER_H_

#include "platform.h"

//functions
#define SETSERVICE 0
#define GETSERVICE 1
#define STORE_L 2
#define ADD_R 3
#define ADD_V 4
#define SUB_R 5
#define SUB_V 6
#define DIV_R 7
#define DIV_V 8
#define MUL_R 9
#define MUL_V 10
#define JMP_G 11
#define JMP_E 12
#define JMP_L 13
#define CMP_R 14
#define CMP_V 15
#define MOVE  16
#define CLONE 17
#define DIE	  18
#define SEND  19
#define RECV  20
#define STORE_H 21
#define STORE_C 22
#define MV      23
#define WAIT    24
#define PRIO    25
#define CLEAR	26

//services
#define SERVICE_BARGRAPH 0
#define SERVICE_THERMOMETER 1
#define SERVICE_COOLER 2
#define SERVICE_HEATER 3
#define SERVICE_LED 4
#define SERVICE_LCD 5
#define SERVICE_BUTTON0 6
#define SERVICE_BUTTON1 7

#define ERROR_NO_SERVICE_PRESENT 1

#define MAX_LCD_ROWS 8

#define N1_MASK 0xF000
#define N2_MASK 0x0F00
#define N3_MASK 0x00F0
#define N4_MASK 0x000F
#define B1_MASK 0xFF00
#define B2_MASK 0x00FF
#define HN4_MASK 0x000C
#define LN4_MASK 0x0003

#define NIBBLE1(opcode) ((opcode & N1_MASK) >> 12)
#define NIBBLE2(opcode) ((opcode & N2_MASK) >> 8)
#define NIBBLE3(opcode) ((opcode & N3_MASK) >> 4)
#define NIBBLE4(opcode)  (opcode & N4_MASK)
#define BYTE1(opcode)   ((opcode & B1_MASK) >> 8)
#define BYTE2(opcode)    (opcode & B2_MASK)
#define HNIBBLE4(opcode)((opcode & HN4_MASK) >> 2)
#define LNIBBLE4(opcode) (opcode & LN4_MASK)

#define BYTE_SIGN 0x80
#define NEG_SIGN 0xff00
#define POS_SIGN 0x00ff

#define TEMP_MASK 0xfff8
#define REG_STR_MASK 0x07

typedef struct {
uint8_t id;
uint8_t reg1;
uint8_t reg2;
int16_t value;
uint8_t node_id;
uint8_t agent_id;
} opcode_t;

int16_t get_signed_value(uint8_t value);
uint8_t execute_agent(agent_t *agent, uint8_t opcode_size);
opcode_t decode_opcode(uint16_t opcode);
void execute_opcode(agent_t *agent, opcode_t opcode);

#ifdef X86
#define PRINTF printf
#else
#define PRINTF //
#endif


#endif
