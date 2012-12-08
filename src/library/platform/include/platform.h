/*
 * platform.h
 *
 *  Created on: Dec 8, 2012
 *      Author: igor
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <stdio.h>
#include <string.h>

#include "global.h"
#include "bargraph.h"
#include "DISPLAY.h"

#define DRV_LEN 30

typedef struct config_platform{

	uint8_t agent0_active;
	uint8_t agent0_prio;
	char* agent0_code;

	uint8_t agent1_active;
	uint8_t agent1_prio;
	char* agent1_code;

	uint8_t agent2_active;
	uint8_t agent2_prio;
	char* agent2_code;

	uint8_t agent3_active;
	uint8_t agent3_prio;
	char* agent3_code;

} config_platform_t;

static config_platform_t platform_configuration;

typedef struct drivers {

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


typedef struct agent{

	uint8_t id;
	uint8_t priority;
	uint8_t status;

	int reg_0;
	int reg_1;
	int reg_2;
	int reg_3;
	int reg_4;
	int reg_5;

	char* reg_str_0;
	char* reg_str_1;
	char* reg_str_2;

	int acc;
	int pc;

	char* code;

	char* rec_msg_id;
	char* rec_msg_content;

} agent_t;

typedef struct platform{
	agent_t agents[4];
	drivers_t drivers;

} platform_t;

extern platform_t platform;

void init_drivers(platform_t* platform);
void platform_init(config_platform_t conf, platform_t* platform);
void run_platform(platform_t* platform);


#endif /* PLATFORM_H_ */
