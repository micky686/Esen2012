/* Copyright (c) 2004, Christian Trödhandl
   All rights reserved.
 
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
 
   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.
 
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */
 
/*
 * ttpa/schedule.c
 *      functions for scheduling initialization and background tasks
 *
 * $Id: schedule.c,v 1.3 2007-01-18 09:09:01 bernd Exp $
 *
 * Revision History:
 *   2004/06/07 (0.3): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2005/08/05 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     Complete rewrite, currently no priority
 *
 */
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "ttpa.h"
#include "schedule.h"
#include "ifs_bitvec.h"
#include "ttpa_lifesign.h"
#include "ttpa_sysfiles.h"

volatile uint8_t sched_bgtask_vec[SCHED_BGVEC];

/*
 * int sched_runtask(SCHED_TASK_P taskptr): execute init or BG task
 */
int sched_runtask(SCHED_TASK_P taskptr)
{
	sched_task_t task;

	memcpy_P(&task, taskptr, sizeof(sched_task_t));
	if(pgm_read_byte(&bitvec_dec[ttpa_conffile.stat]) & task.statevec) {
		return ((*task.task) ()) ? 1 : 0;
	} else {
		return -1;
	}
}

/*
 * int sched_bgtask_init(void): initialize the bitvector for all active BG 
 * tasks
 */
int sched_bgtask_init(void)
{
	uint8_t tasks, i;

	tasks = sched_bg_ttab_end - sched_bg_ttab;
	for(i = 0; i<(tasks>3); i++) {
		sched_bgtask_vec[i] = 0xff;
	}
	sched_bgtask_vec[tasks>>3] = pgm_read_byte(&bitvec_dec[tasks&0x07])-1;
	return 0;
}

ADD_INITTASK(task_bgtask_init, sched_bgtask_init, 4, (1<<TTPA_STATE_UNSYNC));

/*
 * void schedule(void): schedule BG tasks
 */
void schedule(void)
{
	uint8_t tasks, i, j = 0, tasks_active;
	int result;

	tasks = sched_bg_ttab_end - sched_bg_ttab;
	tasks_active = 0;
	for(i = 0; i < tasks; i++) {
		ttpa_lifesign();
		if(!(i&0x07)) {
			j = i>>3;
			if(!sched_bgtask_vec[j]) {
				i += 7;
				continue;
			}
		}
		if(sched_bgtask_vec[j] & pgm_read_byte(&bitvec_dec[i&0x07])) {
		        result = sched_runtask(sched_bg_ttab + i);
			if(result == 0) {
				sched_at_clrvec(sched_bgtask_vec, i);
			} else if(result == 1) {
				tasks_active = 1;
			}
		}
	}
	ttpa_lifesign();
	if(!tasks_active) {
		// sleep if no tasks active
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_mode();
	}
}

/*
 * void schedule_inittasks(void): execute all init tasks for the current 
 * runlevel
 */
void schedule_inittasks(void)
{
	SCHED_TASK_P tptr;

	for(tptr = sched_init_ttab; tptr < sched_init_ttab_end; tptr++) {
		sched_runtask(tptr);
	}
}

