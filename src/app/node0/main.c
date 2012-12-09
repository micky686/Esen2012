/*
 * main.c
 *
 *  Created on: Dec 9, 2012
 *      Author: igor
 */
#include "platform.h"


PLATFORM_CONFIGURATION()
{
	AGENTS_CONFIGURATION(){
		AGENT_INIT(0, 3, code),
		AGENT_INIT(1, 3, code),
		AGENT_INIT(2, 3, code),
		AGENT_INIT(3, 3, code)
	}
};
