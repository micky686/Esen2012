/*
* scheduler.c
*
* Created on: Dec 10, 2012
* Author: igor
*/
#include "scheduler.h"

volatile uint8_t last_agent = 0;

void schedule_next(void) {

	// consider next agent
	if (last_agent < 3) {
		last_agent += 1;
	} else {
		last_agent = 0;
	}
	agent_t *next_agent = (agent_t*)&(platform.agents[last_agent]);
	
	// search for an unblocked agent
	while (next_agent->status == blocked){
		if (last_agent < 3) {
			last_agent += 1;
		} else {
			last_agent = 0;
		}
		agent_t* next_agent = (agent_t*)&(platform.agents[last_agent]);
	}
	
	// execute the next opcodes for the agent
	uint8_t opcodes_done = execute_agent(next_agent, next_agent->priority + 1);
	
	// if the agent was blocked update its status
	if ( opcodes_done != next_agent-> priority + 1) {
		next_agent->status = blocked;
	}

}



