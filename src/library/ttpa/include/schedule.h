#ifndef SCHEDULE_H
#define SCHEDULE_H
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
 * include/schedule.h
 *      functions for scheduling initialization and background tasks
 *
 * $Id: schedule.h,v 1.6 2007-01-22 16:09:01 bernd Exp $
 *
 * Revision History:
 *   2004/06/07 (0.3): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2005/08/05 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     Complete rewrite, currently no priority
 */

#include <avr/pgmspace.h>
#include <inttypes.h>

#include "ifs_bitvec.h"

#if !defined(SCHED_BGTASK_LIMIT)
#define SCHED_BGTASK_LIMIT 32
#endif /* SCHED_BGTASK_LIMIT */
#define SCHED_BGVEC (SCHED_BGTASK_LIMIT>>3)
typedef struct {
	int (* task) (void);
	uint8_t prior;
	uint8_t statevec;
} sched_task_t;

typedef sched_task_t sched_task_pgm__t PROGMEM;

#define SCHED_TASK_P const sched_task_pgm__t *

extern sched_task_t PROGMEM sched_init_ttab[];

extern sched_task_t PROGMEM sched_init_ttab_end[];

extern sched_task_t PROGMEM sched_bg_ttab[];

extern sched_task_t PROGMEM sched_bg_ttab_end[];

extern volatile uint8_t sched_bgtask_vec[SCHED_BGVEC];

#define ADD_INITTASK(handle, task, order, statevec) \
sched_task_t __attribute__ ((section(".sched_init." #order))) handle = \
{ task, 0, statevec }; \
uint8_t __attribute__ ((section(".sched_queue"))) handle ##_q

#define ADD_BGTASK(handle, task, prior, statevec) \
sched_task_t __attribute__ ((section(".sched_bg"))) handle = \
{ task, prior, statevec }; \
uint8_t __attribute__ ((section(".sched_queue"))) handle ##_q

extern int sched_runtask(SCHED_TASK_P taskptr);

extern int sched_bgtask_init(void);

extern void schedule(void);

extern void schedule_inittasks(void);

static inline void sched_at_setvec(volatile uint8_t *vec, uint8_t idx)
{
	uint8_t bmask, tmp;

	bmask = pgm_read_byte(&bitvec_dec[idx&0x7]);
	asm volatile("in __tmp_reg__, __SREG__\n\t"
		     "cli\n\t"
		     "ld %0,Z\n\t"
		     "or %0,%1\n\t"
		     "out __SREG__,__tmp_reg__\n\t"
		     "st Z,%0\n\t"
		     : : "r" (tmp), "r" (bmask), "z" (&(vec[idx>>8])));
}

static inline void sched_at_clrvec(volatile uint8_t *vec, uint8_t idx)
{
/* this code just does nothing, and if corrected background tasks
   execute only once; seems like vec[idx>>3] is nowhere reset * /
	uint8_t bmask, tmp; 
	
	bmask = ~pgm_read_byte(&bitvec_dec[idx&0x7]);
	asm volatile("in __tmp_reg__, __SREG__\n\t"
		     "cli\n\t"
		     "ld %0,Z\n\t"
		     "and %0,%1\n\t"
		     "out __SREG__,__tmp_reg__\n\t"
		     "st Z,%0\n\t"
original code:	     : : "r" (tmp), "r" (bmask), "z" (&(vec[idx>>3])));
correction:	     : "=&r" (tmp) : "r" (bmask), "z" (&(vec[idx>>3])));
*/
}

					    
#endif /* SCHEDULE_H */
