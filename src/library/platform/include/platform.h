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
#include "protocol0.h"
#include "ledmatrix.h"
#include "pushbutton.h"

#define AGENT_MAX 4
#define OPCODE_LEN 16
#define STR_REG_MAX 3
#define REG_MAX 13

#define MAX_SERVICE 7
#define MAX_NODES 4
#define INVALID 0xff

#define NODE0_ID 0x04
#define NODE1_ID 0x03
#define NODE2_ID 0x02
#define NODE3_ID 0x01

#define PLATFORM_CONFIGURATION() \
platform_config_t platform_config =

#define AGENTS_CONFIGURATION() \
.agents_conf =

#define AGENT_INIT(agentid, agentprio, agentcode) \
{ .id=agentid, \
.active = 1, \
.prio = agentprio, \
.code = #agentcode }

#define PLATFORM_ID(id) \
.platform_id = id

#define BOARD_ID(id) \
.board_id = id

typedef struct {

	uint8_t id;
	uint8_t active;
	uint8_t prio;
	char *code;

} agent_config_t;

typedef struct {
	agent_config_t agents_conf[AGENT_MAX];
	//uint8_t platform_id;
	uint8_t board_id;
	uint8_t frame_id;
} platform_config_t;

extern platform_config_t platform_config;

typedef struct {

	void (*set_bargraph)(uint8_t value);

	uint32_t (*clk_get_time)(void);

	void (*set_cooler)(uint8_t duty_cycle);

	void (*DISPLAY_string)(uint8_t x, uint8_t y, uint16_t font_color, uint16_t bg_color, uint8_t pixel_size, char *string);
	void (*DISPLAY_drawBg)(uint16_t rgb);

	void (*heater_set)(uint8_t duty_cycle);

	void(*button0_callback)(void);
	void(*button1_callback)(void);

	uint16_t (*therm_get_temp)(uint8_t name);

	void (*dotmatrix_send)(char *data);

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

	volatile char* rec_msg_content;
	volatile uint16_t rec_msg_len;

	volatile uint8_t sem;
} agent_t;

typedef struct {
	volatile agent_t agents[4];
	drivers_t drivers;
	uint8_t id;
} platform_t;

extern volatile platform_t platform;
extern uint8_t service_locations[MAX_SERVICE][MAX_NODES];
extern volatile uint8_t button0_pressed;
extern volatile uint8_t button1_pressed;

void init_drivers(void);
void init_agents(void);
void reset_agent(uint8_t id);
uint8_t clone_agent(agent_t *agent);
void platform_init(void);
void run_platform(void);

void buttoncallback0(void);
void buttoncallback1(void);


#endif /* PLATFORM_H_ */
