/*
* platform.c
*
* Created on: Dec 8, 2012
* Author: igor
*/

#include "platform.h"
#include "hw_layer.h"
#include "scheduler.h"

#include "util/delay.h"

volatile platform_t platform;

void init_drivers(void){

#ifdef BARGRAPH
	bargraph_init();
	platform.drivers.set_bargraph = set_bargraph;
#endif

#ifdef PROTOCOLL

#endif

#ifdef TIMER2

#endif

#ifdef CLOCK

#endif


#ifdef COOLER
	init_cooler();

#endif


#ifdef HEATER
	heater_init();
	//heater_set(50);
#endif


#ifdef DISPLAY

#endif

#ifdef THERMOMETER
	therm_init();
	platform.drivers.therm_get_temp = therm_get_temp;
#endif


#ifdef PUSHBUTTON

#endif

#ifdef LEDMATRIX
	init_dotmatrix();
	char buf[] = "ABCD";
	dotmatrix_send(buf);
#endif

}

void init_agents(){

	uint8_t i = 0;

	for (i=0; i < AGENT_MAX; i++) {

		if (platform_config.agents_conf[i].active == 1) {
			uint8_t id = platform_config.agents_conf[i].id;
			platform.agents[id].id = id;
			platform.agents[id].status = ready;
			platform.agents[id].priority = platform_config.agents_conf[i].prio;

			if (platform.agents[id].reg_str == 0) {
				platform.agents[id].reg_str = (char**) malloc(STR_REG_MAX * sizeof(char*));
				platform.agents[id].reg_str[0] = (char*) malloc(1);
				platform.agents[id].reg_str[1] = (char*) malloc(1);
				platform.agents[id].reg_str[2] = (char*) malloc(1);
			}

			size_t len = strlen(platform_config.agents_conf[i].code);
			platform.agents[id].code = (uint16_t*) malloc(len / OPCODE_LEN);
			uint16_t ind = 0;
			char opcode[OPCODE_LEN];
			while (ind < len / OPCODE_LEN) {
				strncpy(opcode,(platform_config.agents_conf[i].code + (ind * OPCODE_LEN)), OPCODE_LEN);
				platform.agents[id].code[ind] = strtol(opcode, NULL, 2);
				ind += 1;
			}
			platform.agents[id].code_len = ind;
		}
	}

}

void reset_agent(uint8_t id){

	agent_t *agent = (agent_t*)&(platform.agents[id]);
	agent->id = 0;
	agent->status = stopped;
	agent->priority = 0;
	memset(agent->regs, 0, sizeof(agent->regs));

	if (agent->reg_str[0] != 0) {
		free(agent->reg_str[0]);
	}

	if (agent->reg_str[1] != 0) {
			free(agent->reg_str[1]);
	}

	if (agent->reg_str[2] != 0) {
			free(agent->reg_str[2]);
	}

	if (agent->reg_str != 0){
		free(agent->reg_str);
	}


	if (agent->code != 0){
		free(agent->code);
		agent->code = 0;
	}
	agent->code_len = 0;
	agent->pc = 0;
	agent->status_flag = 0;
	agent->rec_msg_content = 0;
	agent->rec_msg_id = 0;
}

/*
* Initialize the platform with the provided configuration and
* setup all requested drivers.
 */
void platform_init(void) {

	init_drivers();
	init_agents();
}

void run_platform(void) {

	schedule_next();
	_delay_ms(1000);
}

int main(void) {

	platform_init();

	while (1){
		run_platform();
	}

	return 1;
}
