/*
* platform.h
*
* Created on: Dec 8, 2012
* Author: igor
*/

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "bargraph.h"
#include "DISPLAY.h"

#define AGENT_MAX 4

#define ACTIVATE_AGENT(idx) { \
platform.agents[idx].id=idx; \
platform.agents[idx].status=ready; \
}

#define SET_AGENT_PRIO(id, prio) { \
platform.agents[id].priority = prio; \
}

#define SET_AGENT_CODE(id, inpcode) { \
size_t len = strlen(inpcode); \
char* opcode_str = (char*) malloc(len); \
strncpy(opcode_str, inpcode, len); \
platform.agents[id].code = (uint16_t*)malloc(len/16); \
uint16_t ind = 0; \
char opcode[16]; \
while (ind < len/16) { \
	strncpy(opcode, opcode_str+(ind*16), 16); \
	platform.agents[id].code[ind] = strtol(opcode, NULL, 2); \
	ind+=1; \
} \
 free(opcode_str); \
 platform.agents[id].code_len = ind; \
}

#define SET_AGENT_ACTIVE(id) \
.agent0.active=1

#define SET_AGENT0_PRIO(prio) \
.agent0_prio = prio

#define SET_AGENT0_CODE(inpcode) \
.code=#inpcode

#define AGENT_INIT(agentid, agentprio, agentcode) \
{ .id=agentid, \
.active = 1, \
.prio = agentprio, \
.code = #agentcode }


typedef struct {

uint8_t id;
uint8_t active;
uint8_t prio;
char code[];

} agent_config_t;


typedef struct {
agent_config_t agents_conf[AGENT_MAX];
}platform_config_t;

extern platform_config_t platform_config;

#define PLATFORM_CONFIGURATION() \
platform_config_t platform_config =

#define AGENTS_CONFIGURATION() \
.agents_conf =

typedef struct {

void (*set_bargraph)(uint8_t value);

uint32_t (*clk_get_time)(void);

void (*set_cooler)(uint8_t duty_cycle);

void (*DISPLAY_drawBg)(uint16_t rgb);
uint8_t (*DISPLAY_drawElement)(uint8_t row, uint8_t col, uint16_t rgb,
DisplayObject_t object);
void (*DISPLAY_drawBorder)(void);
uint8_t (*DISPLAY_drawPoints)(uint8_t player_id, uint8_t points);

void (*heater_set)(uint8_t duty_cycle);

void (*init_pushbutton0)(void (*callback)(void));
void (*init_pushbutton1)(void (*callback)(void));

int16_t (*therm_get_temp)(uint8_t name);

} drivers_t;

enum agent_status { stopped, ready, blocked };
typedef enum agent_status agent_status_t;

typedef struct {

uint8_t id;
uint8_t priority;
agent_status_t status;

uint32_t regs[6];

char* reg_str_0;
char* reg_str_1;
char* reg_str_2;

uint32_t error;
int acc;
int pc;

uint16_t* code;
uint16_t  code_len;

char* rec_msg_id;
char* rec_msg_content;

} agent_t;

typedef struct {
agent_t agents[4];
drivers_t drivers;

} platform_t;

extern volatile platform_t platform;

void init_drivers(void);
void init_agents(void);
void platform_init(void);
void run_platform(void);


#endif /* PLATFORM_H_ */
