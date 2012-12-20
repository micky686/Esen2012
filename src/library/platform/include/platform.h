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
#include "thermometer.h"
#include "cooler.h"
#include "heater.h"
#include "DISPLAY.h"

#define AGENT_MAX 4
#define OPCODE_LEN 16
#define STR_REG_MAX 3
#define REG_MAX 7

#define PLATFORM_CONFIGURATION() \
platform_config_t platform_config =

#define AGENTS_CONFIGURATION() \
.agents_conf =

#define AGENT_INIT(agentid, agentprio, agentcode) \
{ .id=agentid, \
.active = 1, \
.prio = agentprio, \
.code = #agentcode }

#define COMMUNICATION_ID(id) \
.comm_timeout = id

typedef struct {

	uint8_t id;
	uint8_t active;
	uint8_t prio;
	char *code;

} agent_config_t;

typedef struct {
	agent_config_t agents_conf[AGENT_MAX];
	uint8_t comm_timeout;
} platform_config_t;

extern platform_config_t platform_config;

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

	void (*init_pushbutton0)(void(*callback)(void));
	void (*init_pushbutton1)(void(*callback)(void));

	uint16_t (*therm_get_temp)(uint8_t name);

} drivers_t;

enum agent_status {
	stopped, ready, blocked
};
typedef enum agent_status agent_status_t;

#define REG_ACC 0
#define OVERFLOW 32
#define OVERFLOW_MASK 0x80000000
#define ERROR 0x000000FF
#define SET_ERROR(flag, errno)	(flag |= (errno & ERROR))

typedef struct {

	uint8_t id;
	uint8_t priority;
	agent_status_t status;

	uint32_t status_flag;
	uint16_t pc;

	int16_t regs[REG_MAX];

	uint16_t code_len;
	uint16_t regstr_len [STR_REG_MAX];

	uint16_t* code;

	char** reg_str;

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
void reset_agent(uint8_t id);
uint8_t clone_agent(agent_t *agent);
void platform_init(void);
void run_platform(void);
void recv_handler(uint8_t msg_length, uint8_t *msg_body); 


#endif /* PLATFORM_H_ */
