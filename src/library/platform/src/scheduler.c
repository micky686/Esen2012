/*
 * scheduler.c
 *
 * Created on: Dec 10, 2012
 * Author: igor
 */
#include "scheduler.h"

volatile uint8_t next_agent_id = 0;

void schedule_next(void) {

	uint8_t first_agent_id = next_agent_id;
	uint8_t all_blocked = 0;

	// consider next agent
	agent_t *next_agent = (agent_t*) &(platform.agents[next_agent_id]);

	// search for an unblocked agent
	while (next_agent->status != ready) {
		if (next_agent_id < 3) {
			next_agent_id += 1;
		} else {
			next_agent_id = 0;
		}

		if (first_agent_id == next_agent_id){
			all_blocked = 1;
			break;
		}

		next_agent = (agent_t*) &(platform.agents[next_agent_id]);
	}

	if (all_blocked){
		return;
	}

	// execute the next opcodes for the agent
	uint8_t opcodes_done = execute_agent(next_agent, next_agent->priority + 1);

	// schedule next agent
	next_agent_id += 1;
}
