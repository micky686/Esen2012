/*
* scheduler.h
*
* Created on: Dec 10, 2012
* Author: igor
*/

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "platform.h"
#include "exe_layer.h"

extern volatile uint8_t last_agent;

void schedule_next(void);

#endif

