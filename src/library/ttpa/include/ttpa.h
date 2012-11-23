#ifndef TTPA_H
#define TTPA_H
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
 * include/ttpa.h
 *      Declarations for TTP/A functions and variables
 *
 * $Id: ttpa.h,v 1.4 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/03/18 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/05/07 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added new state TTPA_STATE_PASSIVE_LOCK
 *   2004/06/15 (0.3): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added caching of RODL entries
 *   2005/12/09 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed to support both, caching and non-caching version
 *
 */

// global functions
#include <inttypes.h>
#include "ifs.h"
#include "ifs_rodl.h"
#include "bus.h"

#if defined(USE_RODLCACHE)
#include "mutex.h"

typedef struct {
	union {
		struct {
			uint8_t *ifs_ptr;
			uint8_t spdup;
			uint8_t slotcnt; // no. of bytes in this slot
		};
		struct {
			ttpa_taskptr_t task;
			ttpa_taskparam_t tparam;
		};
			
	};
	uint8_t framecnt; // no. of slots in this frame - 1
} ttpa_framestat_t;

typedef struct {
	ttpa_framestat_t stat;
	uint8_t op;
	uint8_t mux;
	uint8_t mux_msk;
	uint8_t sl_pos;
} ttpa_re_cache_t;
#else
#ifdef OPTIMIZE_SRAM
typedef struct {
	uint8_t spdup;
	uint8_t slotcnt; // no. of bytes in this slot
	unsigned framecnt : 4; // no. of slots in this frame - 1
	unsigned incsctr : 1;
} ttpa_framestat_t;
#else
typedef struct {
	uint8_t spdup;
	uint8_t slotcnt; // no. of bytes in this slot
	uint8_t framecnt; // no. of slots in this frame - 1
	uint8_t incsctr;
} ttpa_framestat_t;
#endif /* OPTIMIZE_SRAM */
#endif /* USE_RODLCACHE */

typedef struct {
	uint16_t sync_timerval;
	uint8_t sync_slot;
} ttpa_sync_t;

typedef enum {
	TTPA_STATE_UNSYNC,
	TTPA_STATE_PASSIVE,
	TTPA_STATE_UNBAPT,
	TTPA_STATE_PASSIVE_LOCK,
	TTPA_STATE_ACTIVE,
	TTPA_STATE_ERROR
} ttpa_state_t;

#if defined(USE_RODLCACHE)
extern ttpa_re_cache_t ttpa_re_cache[8];

extern ttpa_re_cache_t ttpa_curr_frame;

#define RE_LOOKUP_OK 0
#define RE_LOOKUP_MUTEX 1
#define RE_LOOKUP_PARAM 2
#define RE_LOOKUP_RODL 3
#else
extern ttpa_framestat_t ttpa_framestat;

extern ifs_fd_t ttpa_rodlfd;

extern ifs_fd_t ttpa_opfd;
#endif /* USE_RODLCACHE */

extern ttpa_sync_t ttpa_sync;

extern int ttpa_init(void);

extern void ttpa_recvsyncframe(void);

extern void ttpa_recvsyncslot(bus_iobuf_t *busbuf);

extern void ttpa_recvframe(void);

extern void ttpa_recvslot(bus_iobuf_t *busbuf);

extern void ttpa_sendframe(void);

extern void ttpa_sendslot(bus_iobuf_t *busbuf);

extern void ttpa_sendpasvframe(void);

extern void ttpa_sendpasvslot(bus_iobuf_t *busbuf);

extern void ttpa_execframe(void);

extern void ttpa_execslot(void);

extern void ttpa_execeorframe(void);

extern void ttpa_execeorslot(void);

extern void ttpa_eor(void);

extern void ttpa_irg(void);

extern int ttpa_nextframe(void);

extern void ttpa_next_rodlentry(void);

#if defined(USE_RODLCACHE)
extern int ttpa_rodlentry_lookup(ttpa_re_cache_t *cptr, ifs_rodlentry_t *rptr);
#endif /* USE_RODLCACHE */

extern void ttpa_nextslotempty(void);

extern void ttpa_emptyslot(void);

extern void ttpa_newround(void);

extern void ttpa_setstate(uint8_t state);

// interrupt vector for output compare interrupt
void (* volatile ttpa_sig_oc) (void);

// global variables

extern volatile uint16_t ttpa_slotlength;

extern volatile uint16_t ttpa_bitlength;

extern volatile uint16_t ttpa_slotstart;

extern volatile uint8_t ttpa_ovcnt;

#endif /* TTPA_H */

