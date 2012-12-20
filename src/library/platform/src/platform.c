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

#ifdef PROTOCOL0
	protocol_init(platform_config.comm_timeout, recv_handler);
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
			platform.agents[id].code = (uint16_t*) malloc((len / OPCODE_LEN) * sizeof(uint16_t));
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

	if (agent->reg_str != 0){

		if (agent->reg_str[0] != 0) {
			free(agent->reg_str[0]);
			agent->reg_str[0] = 0;
		}

		if (agent->reg_str[1] != 0) {
			free(agent->reg_str[1]);
			agent->reg_str[1] = 0;
		}

		if (agent->reg_str[2] != 0) {
			free(agent->reg_str[2]);
			agent->reg_str[2] = 0;
		}

		free(agent->reg_str);
		agent->reg_str = 0;

	}

	memset(agent->regstr_len, 0, sizeof(uint16_t) * STR_REG_MAX);

	if (agent->code != 0){
		free(agent->code);
		agent->code = 0;
	}
	agent->code_len = 0;
	agent->pc = 0;
	agent->status_flag = 0;

	if (agent->rec_msg_id != 0){
		free(agent->rec_msg_id);
		agent->rec_msg_id = 0;

	}

	if (agent->rec_msg_content != 0) {
		free(agent->rec_msg_content);
		agent->rec_msg_content = 0;
	}

}

uint8_t clone_agent(agent_t *agent){
	uint8_t i = 0;
	uint8_t result = 1;

	for (i = 0; i < AGENT_MAX; i++){
		if (platform.agents[i].status == stopped){
			reset_agent(i);

			platform.agents[i].id = i;
			platform.agents[i].status = ready;
			platform.agents[i].priority = agent->priority;

			if (platform.agents[i].reg_str == 0) {
				platform.agents[i].reg_str = (char**) malloc(STR_REG_MAX * sizeof(char*));
				platform.agents[i].reg_str[0] = (char*) malloc(agent->regstr_len[0]);
				platform.agents[i].reg_str[1] = (char*) malloc(agent->regstr_len[1]);
				platform.agents[i].reg_str[2] = (char*) malloc(agent->regstr_len[2]);
			}

			memcpy(platform.agents[i].reg_str[0], agent->reg_str[0], agent->regstr_len[0]);
			platform.agents[i].regstr_len[0] = agent->regstr_len[0];

			memcpy(platform.agents[i].reg_str[1], agent->reg_str[1], agent->regstr_len[1]);
			platform.agents[i].regstr_len[1] = agent->regstr_len[1];

			memcpy(platform.agents[i].reg_str[2], agent->reg_str[2], agent->regstr_len[2]);
			platform.agents[i].regstr_len[2] = agent->regstr_len[2];

			platform.agents[i].code_len = agent->code_len;
			platform.agents[i].code = (uint16_t*) malloc( agent->code_len * sizeof(uint16_t));
			memcpy(platform.agents[i].code, agent->code, agent->code_len * sizeof(uint16_t));

			platform.agents[i].pc = agent->pc + 1;
			platform.agents[i].status_flag = agent->status_flag;

			memcpy(platform.agents[i].regs, agent->regs, REG_MAX * sizeof(int16_t));
			platform.agents[i].regs[REG_ACC] = 0;
			result = 0;
			break;
		}
	}

	return result;
}

void recv_handler(uint8_t msg_length, uint8_t *msg_body){

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
