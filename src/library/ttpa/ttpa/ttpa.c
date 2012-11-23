/* Copyright (c) 2004, 2005, Christian Trödhandl
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
 * ttpa/ttpa.c
 *      general TTP/A functions, timer handling
 *
 * $Id: ttpa.c,v 1.4 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     TODO: - Jitter compensation
 *           - recv. sync.
 *           - set timer to 0 on start of slot
 *           - execute handling
 *           - untested: MUX, SPDUP
 *           - improve code size
 *   2004/05/07 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added new state TTPA_STATE_PASSIVE_LOCK
 *     - changed ttpa_next_rodlentry()
 *   2004/06/09 (0.3): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - schedule
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added RODL entry caching
 *   2005/12/05 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed to support both, caching and non-caching version
 *   2007/01/09 (0.6): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added trig_init()
 *
 */
#include <string.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/parity.h>

#include "node_config.h"

#include "ttpa.h"
#include "ttpa_master.h"
#include "ttpa_slave.h"
#include "ttpa_sysfiles.h"
#include "ttpa_fb.h"
#include "bus.h"
#include "ms.h"
#include "ms_master.h"
#include "ms_slave.h"
#include "schedule.h"
#include "mutex.h"
#include "trigger.h"
#include "debug.h"

volatile uint16_t ttpa_slotlength;
volatile uint16_t ttpa_bitlength;
volatile uint16_t ttpa_slotstart;
volatile uint8_t ttpa_ovcnt;
#if defined(USE_RODLCACHE)
static volatile mutex_t ttpa_re_cache_lock;

ttpa_re_cache_t ttpa_re_cache[8];

ttpa_re_cache_t ttpa_curr_frame;

static uint8_t ttpa_incsctr;

static volatile uint8_t ttpa_rodl_idx;
#else
ttpa_framestat_t ttpa_framestat;
ifs_fd_t ttpa_rodlfd;
ifs_fd_t ttpa_opfd;
#endif /* USE_RODLCACHE */

void (* volatile ttpa_sig_oc) (void);

/*
 * SIGNAL(TTPA_SIG_TOV): signal handler for overflow
 */
SIGNAL(TTPA_SIG_TOV)
{
	ttpa_ovcnt++;
}

/*
 * SIGNAL(TTPA_SIG_OC): signal handler for output compare match interrupt
 */
SIGNAL(TTPA_SIG_OC)
{
	uint8_t sctr = ttpa_conffile.sctr;
	// increment slot ctr on new slot
#if defined(USE_RODLCACHE)
	if(ttpa_incsctr) {
#else
	if(ttpa_framestat.incsctr) {
#endif /* USE_RODLCACHE */
		sctr++;
	}
	ttpa_conffile.sctr = sctr;
#if defined(USE_RODLCACHE)
	ttpa_incsctr = 0;
#else
	ttpa_framestat.incsctr = 0;
#endif /* USE_RODLCACHE */
	// Jitter compensation code
	// .....

	DEBUG(0, 25);
	// call handler code
	(*ttpa_sig_oc)();
}

/*
 * int ttpa_error(void): disable timer on error.
 */
int ttpa_error(void)
{
	// set clock (no prescaling)
	TTPA_TCCRB = 0;

	TTPA_TIFR &= ~(1<<TTPA_TOIE);
	TTPA_TIMSK |= (1<<TTPA_TOIE);
	return 0;
}

ADD_INITTASK(task_ttpa_error, ttpa_error, 2, (1<<TTPA_STATE_ERROR));

/*
 * void ttpa_init(void): initialize TTP/A protocol: setup timers, ...
 */
int ttpa_init(void)
{
	DEBUG(2, 25);
#if defined(USE_RODLCACHE)
	// initialize RODL cache
	uint8_t i;
	ifs_rodlentry_t *rptr;
	for(i = 0; i<8; i++) {
		rptr = ifs_get_re_ptr(i, 0);
		ttpa_rodl_idx = 0;
		if(rptr == NULL) {
			ttpa_re_cache[i].op = RODL_OP_INVALID;
		} else {
			switch(ttpa_rodlentry_lookup(ttpa_re_cache+i, rptr)) {
			case RE_LOOKUP_OK:
				break;
			case RE_LOOKUP_MUTEX:
			case RE_LOOKUP_PARAM:
				ttpa_setstate(TTPA_STATE_ERROR);
				break;
			case RE_LOOKUP_RODL:
				ttpa_re_cache[i].op = RODL_OP_INVALID;
			break;
			}
		}
	}
#endif /* USE_RODLCACHE */
	// initialize timer to 0
	TTPA_TCNT = 0x0000;
	ttpa_ovcnt = 0x00;

	// set clock (no prescaling)
	TTPA_TCCRB = (1<<TTPA_CS0);

	TTPA_TIFR |= (1<<TTPA_TOIE);
	TTPA_TIMSK |= (1<<TTPA_TOIE);
	if(ttpa_conf_master()) {
		ttpa_master_init();
	} else {
		ttpa_slave_init();
	}
	trig_init();
	sei();
	return 0;
}

ADD_INITTASK(task_ttpa_init, ttpa_init, 7, (1<<TTPA_STATE_UNSYNC));

/* int ttpa_enableint(void) */
/* { */
/* 	sei(); */
/* 	return 0; */
/* } */

/* ADD_INITTASK(task_ttpa_enableint, ttpa_enableint, 8, 0xff); */

/*
 * void ttpa_recvsyncframe(void): receive-sync op. in next slot
 */
void ttpa_recvsyncframe(void)
{
	bus_iobuf_t buf;

#if defined(USE_RODLCACHE)
	buf.spdup = ttpa_curr_frame.stat.spdup;
#else
	buf.spdup = ttpa_framestat.spdup;
#endif /* USE_RODLCACHE */
	// start with receive on BC before slotstart
	buf.slotstart = ttpa_slotstart - ttpa_bitlength;
	// signal timeout
	buf.timeout = 1;
	DEBUG(4, 25);
	bus_op_complete = (void (*) (bus_iobuf_t *)) ttpa_recvsyncslot;
	bus_recvbyte_init(&buf);
}

/*
 * void ttpa_recvsyncslot(bus_iobuf_t *busbuf): received byte from master 
 * (sync.)
 */
void ttpa_recvsyncslot(bus_iobuf_t *busbuf)
{
	// synchronize to slot start


	ttpa_recvslot(busbuf);
}

/*
 * void ttpa_recvframe(void): receive op. in next slot
 */
void ttpa_recvframe(void)
{
	bus_iobuf_t buf;

#if defined(USE_RODLCACHE)
	buf.spdup = ttpa_curr_frame.stat.spdup;
#else
	buf.spdup = ttpa_framestat.spdup;
#endif /* USE_RODLCACHE */
	// start with receive on BC before slotstart
	buf.slotstart = ttpa_slotstart - ttpa_bitlength;
	// signal timeout
	buf.timeout = 1;
	bus_op_complete = (void (*) (bus_iobuf_t *)) ttpa_recvslot;
	bus_recvbyte_init(&buf);
}

/*
 * void ttpa_recvslot(bus_iobuf_t *busbuf): received byte
 */
void ttpa_recvslot(bus_iobuf_t *busbuf)
{
	uint8_t par;
	uint8_t spdup;

#if defined(USE_RODLCACHE)
	spdup = ttpa_curr_frame.stat.spdup;
	ttpa_curr_frame.stat.slotcnt--;
#else
	spdup = ttpa_framestat.spdup;
	ttpa_framestat.slotcnt--;
#endif /* USE_RODLCACHE */
	if(busbuf->error != BUS_IO_OK) {
#if defined(USE_RODLCACHE)
	        ttpa_curr_frame.stat.slotcnt = 0;
		ttpa_curr_frame.stat.framecnt = 0;
#else
	        ttpa_framestat.slotcnt = 0;
		ttpa_framestat.framecnt = 0;
#endif /* USE_RODLCACHE */
		if(ttpa_conf_master()) {
			if(ttpa_conffile.crnd == TTPA_FB_MSD) {
				ms_msdtimeout = 1;
			}
		}
	} else {
		par = parity_even_bit(busbuf->buf);
		if(par == busbuf->par) {
#if defined(USE_RODLCACHE)
			// Add recv-buffer here
			*(ttpa_curr_frame.stat.ifs_ptr++) = busbuf->buf;
#else
			ifs_putc(&ttpa_opfd, busbuf->buf);
#endif /* USE_RODLCACHE */
			busbuf->slotstart += (ttpa_slotlength>>spdup) -
				(ttpa_bitlength>>spdup);
		} else {
#if defined(USE_RODLCACHE)
			ttpa_curr_frame.stat.slotcnt = 0;
			ttpa_curr_frame.stat.framecnt = 0;
#else
			ttpa_framestat.slotcnt = 0;
			ttpa_framestat.framecnt = 0;
#endif /* USE_RODLCACHE */
		}
	}
#if defined(USE_RODLCACHE)
	if(ttpa_curr_frame.stat.slotcnt == 0) {
#else
	if(ttpa_framestat.slotcnt == 0) {
#endif /* USE_RODLCACHE */
		if(ttpa_nextframe()) {
			return;
		} else {
#if defined(USE_RODLCACHE)
			ttpa_curr_frame.stat.slotcnt = 
				1<<ttpa_curr_frame.stat.spdup;
#else
			ttpa_framestat.slotcnt = 1<<ttpa_framestat.spdup;
#endif /* USE_RODLCACHE */
			busbuf->slotstart = ttpa_slotstart - ttpa_bitlength;
		}
	}
	busbuf->spdup = spdup;
	busbuf->timeout = 1;
	bus_op_complete = (void (*) (bus_iobuf_t *)) ttpa_recvslot;
	bus_recvbyte_init(busbuf);
}

/*
 * void ttpa_sendframe(void): send op. in next slot
 */
void ttpa_sendframe(void)
{
	bus_iobuf_t buf;

#if defined(USE_RODLCACHE)
	buf.buf = *(ttpa_curr_frame.stat.ifs_ptr++);
#else
	buf.buf = ifs_getc(&ttpa_opfd);
#endif /* USE_RODLCACHE */
	buf.par = parity_even_bit(buf.buf);
#if defined(USE_RODLCACHE)
	ttpa_curr_frame.stat.slotcnt--;
	buf.spdup = ttpa_curr_frame.stat.spdup;
#else
	ttpa_framestat.slotcnt--;
	buf.spdup = ttpa_framestat.spdup;
#endif /* USE_RODLCACHE */
	buf.slotstart = ttpa_slotstart;
	bus_op_complete = (void (*) (bus_iobuf_t *)) ttpa_sendslot;
	bus_sendbyte_init(&buf);
}

/*
 * void ttpa_sendslot(bus_iobuf_t *busbuf): just sent a byte
 */
void ttpa_sendslot(bus_iobuf_t *busbuf)
{
#if defined(USE_RODLCACHE)
	if(ttpa_curr_frame.stat.slotcnt == 0) {
#else
	if(ttpa_framestat.slotcnt == 0) {
#endif /* USE_RODLCACHE */
		if(ttpa_nextframe()) {
			return;
		} else {
#if defined(USE_RODLCACHE)
			ttpa_curr_frame.stat.slotcnt = 
				1<<ttpa_curr_frame.stat.spdup;
#else
			ttpa_framestat.slotcnt = 1<<ttpa_framestat.spdup;
#endif /* USE_RODLCACHE */
			busbuf->slotstart = ttpa_slotstart;
		}
	}
#if defined(USE_RODLCACHE)
	ttpa_curr_frame.stat.slotcnt--;
	busbuf->buf = *(ttpa_curr_frame.stat.ifs_ptr++);
	busbuf->par = parity_even_bit(busbuf->buf);
	busbuf->spdup = ttpa_curr_frame.stat.spdup;
#else
	ttpa_framestat.slotcnt--;
	busbuf->buf = ifs_getc(&ttpa_opfd);
	busbuf->par = parity_even_bit(busbuf->buf);
	busbuf->spdup = ttpa_framestat.spdup;
#endif /* USE_RODLCACHE */
	bus_op_complete = (void (*) (bus_iobuf_t *)) ttpa_sendslot;
	bus_sendbyte_init(busbuf);
}

/*
 * void ttpa_sendpasvframe(void): listen for activity in next slot
 */
void ttpa_sendpasvframe(void)
{
	bus_iobuf_t buf;

	buf.spdup = 0;
	// start with receive on BC before slotstart
	buf.slotstart = ttpa_slotstart - ttpa_bitlength;
	// signal timeout
	buf.timeout = 1;
	bus_op_complete = (void (*) (bus_iobuf_t *)) ttpa_sendpasvslot;
	bus_recvbyte_init(&buf);
}


/*
 * void ttpa_sendpasvslot(bus_iobuf_t *busbuf): listen for bus activity 
 */
void ttpa_sendpasvslot(bus_iobuf_t *busbuf)
{
#if defined(USE_RODLCACHE)
	ttpa_curr_frame.stat.slotcnt = 0;
#else
	ttpa_framestat.slotcnt = 0;
#endif /* USE_RODLCACHE */
	// if no timeout has occured, an other node is sending in this slot
	if(busbuf->error != BUS_IO_TIMEOUT) {
		ttpa_setstate(TTPA_STATE_UNSYNC);
		return;
	} else {
		if(ttpa_nextframe()) {
			return;
		} else {
			busbuf->slotstart = ttpa_slotstart - ttpa_bitlength;
		}
	}
	busbuf->spdup = 0;
	busbuf->timeout = 1;
	bus_op_complete = (void (*) (bus_iobuf_t *)) ttpa_sendpasvslot;
	bus_recvbyte_init(busbuf);
}

/*
 * void ttpa_execframe(void): execute TTP/A task in next slot
 */
void ttpa_execframe(void)
{
	TTPA_TIFR |= (1<<TTPA_OCIE);
	TTPA_TIMSK = (TTPA_TIMSK & ~(1<<TTPA_TICIE)) | (1<<TTPA_OCIE);
	TTPA_OCR = ttpa_slotstart;
#if defined(USE_RODLCACHE)
	ttpa_curr_frame.stat.framecnt = 0;
#else
	ttpa_framestat.framecnt = 0;
#endif /* USE_RODLCACHE */
	ttpa_sig_oc = (void (*)(void)) ttpa_execslot;
}

/*
 * void ttpa_execslot(void): execute TTP/A task
 */
void ttpa_execslot(void)
{
	ttpa_taskptr_t task;
	ttpa_taskparam_t param;

#if defined(USE_RODLCACHE)
	task = ttpa_curr_frame.stat.task;
	param = ttpa_curr_frame.stat.tparam;
	ttpa_curr_frame.stat.framecnt = 0;
#else
	task = ttpa_opfd.task;
	param.rec = ttpa_opfd.addr.rec;
	param.align = ttpa_opfd.addr.align;
	ttpa_framestat.slotcnt = 0;
	ttpa_framestat.framecnt = 0;
#endif /* USE_RODLCACHE */
	ttpa_nextframe();
	// enable interrupts
	sei();
	task(param);
	return;
}

/*
 * void ttpa_execeorframe(void): EOR, execute TTP/A task in IRG 
 */
void ttpa_execeorframe(void)
{
	TTPA_TIFR |= (1<<TTPA_OCIE);
	TTPA_TIMSK = (TTPA_TIMSK & ~(1<<TTPA_TICIE)) | (1<<TTPA_OCIE);
	TTPA_OCR = ttpa_slotstart;
#if defined(USE_RODLCACHE)
	ttpa_curr_frame.stat.framecnt = 0;
#else
	ttpa_framestat.framecnt = 0;
#endif /* USE_RODLCACHE */
	ttpa_sig_oc = (void (*)(void)) ttpa_execeorslot;
}

/*
 * void ttpa_execeorslot(void): execute TTP/A task in IRG 
 */
void ttpa_execeorslot(void)
{
	ttpa_taskptr_t task;
	ttpa_taskparam_t param;

#if defined(USE_RODLCACHE)
	task = ttpa_curr_frame.stat.task;
	param = ttpa_curr_frame.stat.tparam;
#else
	task = ttpa_opfd.task;
	param.rec = ttpa_opfd.addr.rec;
	param.align = ttpa_opfd.addr.align;
#endif /* USE_RODLCACHE */
	ttpa_irg();
	// enable interrupts
	sei();
	task(param);
	return;
}

/*
 * void ttpa_eor(void): end of round reached
 */
void ttpa_eor(void)
{
	TTPA_TIFR |= (1<<TTPA_OCIE);
	TTPA_TIMSK = (TTPA_TIMSK & ~(1<<TTPA_TICIE)) | (1<<TTPA_OCIE);
	TTPA_OCR = ttpa_slotstart;
#if defined(USE_RODLCACHE)
	ttpa_curr_frame.stat.framecnt = 0;
#else
	ttpa_framestat.framecnt = 0;
#endif /* USE_RODLCACHE */
	ttpa_sig_oc = (void (*)(void)) ttpa_irg;
}

/*
 * void ttpa_irg(void): lookup next RODL file (master) or wait for
 * next FB (slave)
 */
void ttpa_irg(void)
{
	if(ttpa_conf_master()) {
		ttpa_eor_master();
	} else {
		ttpa_eor_slave();
	}
	return;
}

/*
 * int ttpa_nextframe(void): returns 0 if current operation continues,
 * 1 otherwise; sets ttpa_slotstart to start of new slot
 */
int ttpa_nextframe(void)
{
	ttpa_slotstart += ttpa_slotlength;
#if defined(USE_RODLCACHE)
	ttpa_incsctr = 1;
	if(ttpa_curr_frame.stat.framecnt > 0) {
		ttpa_curr_frame.stat.framecnt--;
#else
	ttpa_framestat.incsctr = 1;
	if(ttpa_framestat.framecnt > 0) {
		ttpa_framestat.framecnt--;
#endif /* USE_RODLCACHE */
		return 0;
	} else {
		ttpa_next_rodlentry();
		return 1;
	}
}

typedef struct {
	uint8_t mux;
#if !defined(USE_RODLCACHE)
	uint8_t mux_b2;
#endif /* USE_RODLCACHE */
	uint8_t mux_msk;
	uint8_t spdup;
	uint8_t open;
	uint8_t op;
#if !defined(USE_RODLCACHE)
	void (* func) (void);
	void (* pasv_func) (void);
#endif /* USE_RODLCACHE */
} ttpa_re_parse_t;

#if defined(USE_RODLCACHE)
ttpa_re_parse_t __attribute__ ((progmem)) ttpa_re_parsetab[] = {
	{0x00,0x00,0,1,IFS_OP_WRITE},
	{0x00,0x00,0,1,IFS_OP_READ},
	{0x00,0x00,0,1,IFS_OP_WRITE},
	{0x00,0x00,0,1,IFS_OP_EXEC},
	{0x00,0x00,1,1,IFS_OP_WRITE},
	{0x00,0x00,1,1,IFS_OP_READ},
	{0x00,0x00,0,0,0,},
	{0x00,0x00,0,1,IFS_OP_EXEC},
	{0x00,0x03,0,1,IFS_OP_WRITE},
	{0x00,0x03,0,1,IFS_OP_READ},
	{0x00,0x07,0,1,IFS_OP_WRITE},
	{0x00,0x07,0,1,IFS_OP_READ},
	{0x00,0x03,1,1,IFS_OP_WRITE},
	{0x00,0x03,1,1,IFS_OP_READ},
	{0x04,0x07,0,1,IFS_OP_WRITE},
	{0x04,0x07,0,1,IFS_OP_READ}
};

typedef void (* ttpa_framefunc_t) (void);

ttpa_framefunc_t PROGMEM ttpa_framefunc[] = {
	ttpa_recvframe,
	ttpa_sendframe,
	ttpa_recvsyncframe,
	ttpa_execframe,
	ttpa_recvframe,
	ttpa_sendframe,
	ttpa_eor,
	ttpa_execeorframe,
	ttpa_recvframe,
	ttpa_sendframe,
	ttpa_recvframe,
	ttpa_sendframe,
	ttpa_recvframe,
	ttpa_sendframe,
	ttpa_recvframe,
	ttpa_sendframe,
	ttpa_nextslotempty
};

ttpa_framefunc_t PROGMEM ttpa_pasv_framefunc[] = {
	ttpa_nextslotempty,
	ttpa_sendpasvframe,
	ttpa_nextslotempty,
	ttpa_nextslotempty,
	ttpa_nextslotempty,
	ttpa_sendpasvframe,
	ttpa_eor,
	ttpa_eor,
	ttpa_nextslotempty,
	ttpa_sendpasvframe,
	ttpa_nextslotempty,
	ttpa_sendpasvframe,
	ttpa_nextslotempty,
	ttpa_sendpasvframe,
	ttpa_nextslotempty,
	ttpa_sendpasvframe,
	ttpa_nextslotempty
};
#else
ttpa_re_parse_t __attribute__ ((progmem)) ttpa_re_parsetab[] = {
	{0,0x00,0x00,0,1,IFS_OP_WRITE,ttpa_recvframe,    ttpa_nextslotempty},
	{0,0x00,0x00,0,1,IFS_OP_READ, ttpa_sendframe,    ttpa_sendpasvframe},
	{0,0x00,0x00,0,1,IFS_OP_WRITE,ttpa_recvsyncframe,ttpa_nextslotempty},
	{0,0x00,0x00,0,1,IFS_OP_EXEC, ttpa_execframe,    ttpa_nextslotempty},
	{0,0x00,0x00,1,1,IFS_OP_WRITE,ttpa_recvframe,    ttpa_nextslotempty},
	{0,0x00,0x00,1,1,IFS_OP_READ, ttpa_sendframe,    ttpa_sendpasvframe},
	{0,0x00,0x00,0,0,0,           ttpa_eor,          ttpa_eor},
	{0,0x00,0x00,0,1,IFS_OP_EXEC, ttpa_execeorframe, ttpa_eor},
	{1,0x00,0x03,0,1,IFS_OP_WRITE,ttpa_recvframe,    ttpa_nextslotempty},
	{1,0x00,0x03,0,1,IFS_OP_READ, ttpa_sendframe,    ttpa_sendpasvframe},
	{1,0x00,0x07,0,1,IFS_OP_WRITE,ttpa_recvframe,    ttpa_nextslotempty},
	{1,0x00,0x07,0,1,IFS_OP_READ, ttpa_sendframe,    ttpa_sendpasvframe},
	{1,0x00,0x03,1,1,IFS_OP_WRITE,ttpa_recvframe,    ttpa_nextslotempty},
	{1,0x00,0x03,1,1,IFS_OP_READ, ttpa_sendframe,    ttpa_sendpasvframe},
	{1,0x04,0x07,0,1,IFS_OP_WRITE,ttpa_recvframe,    ttpa_nextslotempty},
	{1,0x04,0x07,0,1,IFS_OP_READ, ttpa_sendframe,    ttpa_sendpasvframe}
};
#endif /* USE_RODLCACHE */

#if defined(USE_RODLCACHE)
/*
 * void ttpa_next_rodlentry(void): lookup next RODL entry
 */
void ttpa_next_rodlentry(void)
{
	uint8_t update_cache = 0;
	ttpa_framefunc_t framefunc;
	ifs_rodlentry_t *rptr;
	uint8_t stat = ttpa_conffile.stat;
	uint8_t crnd = ttpa_conffile.crnd;

	if((ttpa_conffile.sctr == 0) || (ttpa_conffile.sctr >= 
					 ttpa_curr_frame.sl_pos)) {
		if(mutex_test_set(&ttpa_re_cache_lock)) {
			ttpa_setstate(TTPA_STATE_ERROR); // mutex failed
			return;
		}
		memcpy(&ttpa_curr_frame, ttpa_re_cache + (crnd & 0x07),
		       sizeof(ttpa_re_cache_t));
		mutex_release(&ttpa_re_cache_lock);
		update_cache = 1;
	}
	if(ttpa_curr_frame.op == RODL_OP_INVALID) {
		ttpa_setstate(TTPA_STATE_UNSYNC); // ev passive
	}	
	if(ttpa_conffile.sctr+1 == ttpa_curr_frame.sl_pos) {
		if(ttpa_curr_frame.mux != (ttpa_conffile.cmux & 
					   ttpa_curr_frame.mux_msk)) {
			framefunc = ttpa_nextslotempty;
			goto exec;
		}
		switch(stat) {
		case TTPA_STATE_ACTIVE:
			framefunc = (ttpa_framefunc_t) 
				pgm_read_word(ttpa_framefunc + 
					      ttpa_curr_frame.op);
			goto exec;
		case TTPA_STATE_UNBAPT:
		case TTPA_STATE_PASSIVE_LOCK:
			if((crnd == TTPA_FB_MSD) || (crnd == TTPA_FB_MSA)) {
				framefunc = (ttpa_framefunc_t) 
					pgm_read_word(ttpa_framefunc + 
						      ttpa_curr_frame.op);
				goto exec;
			} else {
				framefunc = (ttpa_framefunc_t) 
					pgm_read_word(ttpa_pasv_framefunc + 
						      ttpa_curr_frame.op);
				goto exec;
			}
		default:
			framefunc = (ttpa_framefunc_t) 
				pgm_read_word(ttpa_pasv_framefunc + 
					      ttpa_curr_frame.op);
			goto exec;
		}
	} else {
		framefunc = ttpa_nextslotempty;
		goto exec;
	}
exec:
	framefunc();
	if(update_cache) {
		sei();
		if((ttpa_curr_frame.op != RODL_OP_EOR) &&
		   (ttpa_curr_frame.op != RODL_OP_EXEC_EOR)) {
			rptr = ifs_get_re_ptr((crnd&0x07), ++ttpa_rodl_idx);
		} else {
			ttpa_rodl_idx = 0;
			rptr = ifs_get_re_ptr((crnd&0x07), 0);
		}
		if(rptr == NULL) {
			ttpa_re_cache[crnd&0x07].op = RODL_OP_INVALID;
			ttpa_setstate(TTPA_STATE_UNSYNC); // ev passive
			return;
		}
		switch(ttpa_rodlentry_lookup(ttpa_re_cache+(crnd&0x07),rptr)) {
		case RE_LOOKUP_OK:
			break;
		case RE_LOOKUP_MUTEX:
		case RE_LOOKUP_PARAM:
			ttpa_setstate(TTPA_STATE_ERROR);
			break;
		case RE_LOOKUP_RODL:
			ttpa_setstate(TTPA_STATE_UNSYNC); // ev passive
			break;
		}
	}
	return;
}

int ttpa_rodlentry_lookup(ttpa_re_cache_t *cptr, ifs_rodlentry_t *rptr)
{
	ttpa_re_parse_t parsetabentry;
	uint8_t op;
	ifs_fd_t opfd;
        ifs_addr_t addr;
	int result = RE_LOOKUP_OK;

	DEBUG(4,10);
	if(mutex_test_set(&ttpa_re_cache_lock)) {
		return RE_LOOKUP_MUTEX; // mutex failed
	}
	if((rptr == NULL) || (cptr == NULL)) {
		result = RE_LOOKUP_PARAM;
		goto cleanup;
	}
	op = rptr->op;
	memcpy_P(&parsetabentry, &ttpa_re_parsetab[op],
		 sizeof(ttpa_re_parse_t));
	cptr->op = op;
	if(parsetabentry.mux_msk) {
		cptr->mux = parsetabentry.mux | rptr->mux;
		cptr->mux_msk = parsetabentry.mux_msk;
	} else {
		cptr->mux = 0x00;
		cptr->mux_msk = 0x00;
	}
	cptr->sl_pos = rptr->sl_pos;
	addr = rptr->addr_u.addr;
	if(parsetabentry.spdup) {
		cptr->stat.spdup = addr.align+2;
		addr.align = 0;
		if(cptr->stat.spdup > MAXSPDUP) {
			cptr->op = RODL_OP_IGNORE;
			result = RE_LOOKUP_RODL;
			goto cleanup;
		}			
	} else {
		cptr->stat.spdup = 0;
	}
	cptr->stat.slotcnt = 1<<cptr->stat.spdup;
	cptr->stat.framecnt = rptr->f_len;
	if(parsetabentry.open) {
		ifs_open(&opfd, addr, IFS_REQ(IFS_ACCESS_MP, parsetabentry.op, 
					      1));
		if(opfd.error != IFS_ERR_SUCC) {
			if(op != RODL_OP_EXEC_EOR) {
				cptr->op = RODL_OP_IGNORE;
			} else {
				cptr->op = RODL_OP_EOR;
			}
			result = RE_LOOKUP_RODL;
			goto cleanup;
		}
		if(parsetabentry.op != IFS_OP_EXEC) {
			cptr->stat.ifs_ptr = (uint8_t *) opfd.buf;
		} else {
			cptr->stat.task = opfd.task;
			cptr->stat.tparam.rec = addr.rec;
			cptr->stat.tparam.align = addr.align;
		}
	} else {
		cptr->stat.ifs_ptr = NULL;
	}
cleanup:
	mutex_release(&ttpa_re_cache_lock);
	return result;
}
#else
/*
 * void ttpa_next_rodlentry(void): lookup next RODL entry
 */
void ttpa_next_rodlentry(void)
{
	ttpa_re_parse_t parsetabentry;
	uint8_t op;
	ifs_rodlentry_t *rodlptr;
	ifs_addr_t addr;

	rodlptr = (ifs_rodlentry_t *) ttpa_rodlfd.buf;
	if(ttpa_conffile.sctr >= rodlptr->sl_pos) {
		// test if end of RODL file is reached (error: no EOR)
		// statechange to unsync
		if(ttpa_rodlfd.len < 4) {
			ttpa_setstate(TTPA_STATE_UNSYNC);
			return;
		} else {
			rodlptr++;
			ttpa_rodlfd.buf = rodlptr;
			ttpa_rodlfd.len -= 4;
		}
	}
	if(ttpa_conffile.sctr+1 == rodlptr->sl_pos) {
		// parse RODL entry for next slot
		op = rodlptr->op;
		// check mux
		memcpy_P(&parsetabentry, &ttpa_re_parsetab[op],
			 sizeof(ttpa_re_parse_t));
		if(parsetabentry.mux) {
			if((uint8_t) (rodlptr->mux | parsetabentry.mux_b2) !=
			   (uint8_t) (ttpa_conffile.cmux & 
				      parsetabentry.mux_msk)) {
				ttpa_nextslotempty();
				return;
			}
		}
		addr = rodlptr->addr_u.addr;
		// check speedup
		if(parsetabentry.spdup) {
			ttpa_framestat.spdup = rodlptr->addr_u.align_spdup+2;
			addr.align = 0;
			if(ttpa_framestat.spdup > MAXSPDUP) {
				ttpa_nextslotempty();
				return;
			}			
		} else {
			ttpa_framestat.spdup = 0;
		}
		ttpa_framestat.slotcnt = 1<<ttpa_framestat.spdup;
		ttpa_framestat.framecnt = rodlptr->f_len;
		// check operation
		uint8_t stat = ttpa_conffile.stat;
		uint8_t crnd = ttpa_conffile.crnd;
		if(parsetabentry.open) {
			ifs_open(&ttpa_opfd, addr, 
				 IFS_REQ(IFS_ACCESS_MP, parsetabentry.op, 1));
			if(ttpa_opfd.error != IFS_ERR_SUCC) {
				if(op != RODL_OP_EXEC_EOR) {
					ttpa_nextslotempty();
				} else {
					ttpa_eor();
				}
				return;
			}
		}
		switch(stat) {
		case TTPA_STATE_ACTIVE:
			parsetabentry.func();
			return;
		case TTPA_STATE_UNBAPT:
		case TTPA_STATE_PASSIVE_LOCK:
			if((crnd == TTPA_FB_MSD) || (crnd == TTPA_FB_MSA)) {
				parsetabentry.func();
				return;
			} else {
				parsetabentry.pasv_func();
				return;
			}
		default:
			parsetabentry.pasv_func();
			return;
		}
	} else {
		ttpa_nextslotempty();
	}
}
#endif /* USE_RODLCACHE */

/*
 * void ttpa_nextslotempty(void): next slot is empty
 */
void ttpa_nextslotempty(void)
{
	// empty slot: set timeout
	TTPA_TIFR |= (1<<TTPA_OCIE);
	TTPA_TIMSK = (TTPA_TIMSK & ~(1<<TTPA_TICIE)) | (1<<TTPA_OCIE);
	TTPA_OCR = ttpa_slotstart;
#if defined(USE_RODLCACHE)
	ttpa_curr_frame.stat.framecnt = 0;
#else
	ttpa_framestat.framecnt = 0;
#endif /* USE_RODLCACHE */
	ttpa_sig_oc = (void (*)(void)) ttpa_emptyslot;
}

/*
 * void ttpa_emptyslot(void): slot is empty, lpookup next action
 */
void ttpa_emptyslot(void)
{
	(void) ttpa_nextframe();
}

/*
 * void ttpa_setstate(uint8_t state): set protocol state and execute
 * corresponding actions
 */
#if defined(USE_SCHEDULE)
void ttpa_setstate(uint8_t state)
{
	ttpa_conffile.stat = state;
	schedule_inittasks();
	return;
}

#else
void ttpa_setstate(uint8_t state)
{
	
	ttpa_conffile.stat = state;
	if(state == TTPA_STATE_UNSYNC) {
		ttpa_init();
	}
}
#endif

#if defined(USE_RODLCACHE)
/*
 * void ttpa_newround(void): FB has been received or sent, lookup RODL file
 */
void ttpa_newround(void)
{
	ttpa_rodl_idx = 0;	
	// set timer to 0 on start of FB

	// ignore MSD round if ignore flag is set
	if(ttpa_conffile.crnd == TTPA_FB_MSD) {
                if((!ttpa_conf_master()) && (ms_datafile.msd_ignore)) {
			ttpa_re_cache[TTPA_FB_MSD&0x07].op = 
				RODL_OP_IGNORE;
		}
	}
	ttpa_conffile.sctr = 0;

	// increment epoch counter
	ttpa_conffile.ectr++;
	// if MSA round call msa_master_init or msa_slave_init
	if(ttpa_conffile.crnd == TTPA_FB_MSA) {
		if(ttpa_conf_master()) {
			ms_master_init();
			if(ttpa_conffile.stat == TTPA_STATE_UNSYNC) {
				ttpa_setstate(TTPA_STATE_ACTIVE);
			}
		} else {
			ms_slave_init();
			if(ttpa_conffile.stat == TTPA_STATE_PASSIVE) {
				if(ttpa_conffile.cln == LN_NBAPT) {
					ttpa_setstate(TTPA_STATE_UNBAPT);
				} else {
					ttpa_setstate(TTPA_STATE_ACTIVE);
				}
			} else if(ttpa_conffile.stat == TTPA_STATE_UNSYNC) {
				ttpa_setstate(TTPA_STATE_PASSIVE);
			}
		}
	}
	// lookup next operation
	ttpa_curr_frame.stat.framecnt = 0;
	ttpa_nextframe();
	return;
}
#else
/*
 * void ttpa_newround(void): FB has been received or sent, lookup RODL file
 */
void ttpa_newround(void)
{
	ifs_addr_t addr;
	// set timer to 0 on start of FB

	// lookup RODL file
	addr.filen = ttpa_conffile.crnd & 0x07;
	addr.align = 0;
	addr.rec = 1;
	ifs_close(&ttpa_rodlfd);
	ifs_open(&ttpa_rodlfd, addr, IFS_REQ(IFS_ACCESS_APPL, IFS_OP_READ, 1));
	if(ttpa_rodlfd.error != IFS_ERR_SUCC) {
		ttpa_setstate(TTPA_STATE_UNSYNC);
		return;
	}
	// ignore MSD round if ignore flag is set
	if(ttpa_conffile.crnd == TTPA_FB_MSD) {
		if((!ttpa_conf_master()) && (ms_datafile.msd_ignore)) {
			ttpa_rodlfd.buf += 4;
			ttpa_rodlfd.len -= 4;
		}
	}
	ttpa_conffile.sctr = 0;

	// increment epoch counter
	ttpa_conffile.ectr++;
	// if MSA round call msa_master_init or msa_slave_init
	if(ttpa_conffile.crnd == TTPA_FB_MSA) {
		if(ttpa_conf_master()) {
			ms_master_init();
			if(ttpa_conffile.stat == TTPA_STATE_UNSYNC) {
				ttpa_setstate(TTPA_STATE_ACTIVE);
			}
		} else {
			ms_slave_init();
			if(ttpa_conffile.stat == TTPA_STATE_PASSIVE) {
				if(ttpa_conffile.cln == LN_NBAPT) {
					ttpa_setstate(TTPA_STATE_UNBAPT);
				} else {
					ttpa_setstate(TTPA_STATE_ACTIVE);
				}
			} else if(ttpa_conffile.stat == TTPA_STATE_UNSYNC) {
				ttpa_setstate(TTPA_STATE_PASSIVE);
			}
		}
	}
	// lookup next operation
	ttpa_framestat.slotcnt = 0;
	ttpa_framestat.framecnt = 0;
	ttpa_nextframe();
	return;
}
#endif /* USE_RODLCACHE */
