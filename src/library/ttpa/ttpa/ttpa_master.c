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
 * ttpa/ttpa_master.c
 *      Functions specific for TTP/A master (e.g., ROSE handling)
 *
 * $Id: ttpa_master.c,v 1.4 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/28 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - Only a dummy for real ROSE handling
 *     TODO: - real ROSE
 *   2004/05/06 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added real ROSE interpretation
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added RODL entry caching
 *   2005/12/05 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed to support both, caching and non-caching version
 *     - adapted to GCC 4.0 
 *   2007/01/09 (0.6): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added trig()
 *
 */
#include <stdio.h>
#include <avr/parity.h>

#include "ttpa_rose.h"
#include "ttpa_master.h"
#include "ttpa_sysfiles.h"
#include "bus.h"
#include "ttpa.h"
#include "node_config.h"
#include "ttpa_fb.h"
#include "trigger.h"

struct ttpa_membershp_struct ttpa_membershpfile;

IFS_ADDSYSFILE(TTPA_SYSFILE_MEMBERSHP, &ttpa_membershpfile,
	       NULL, IFS_FILELEN(struct ttpa_membershp_struct),
	       ifs_int_eep, 044);

void ttpa_rose_task(ttpa_taskparam_t param);

IFS_ADDSYSFILE(TTPA_SYSFILE_ROSE, &ttpa_rosefile, ttpa_rose_task, IFS_FILELEN(struct ttpa_rosefile_struct), ifs_int_eep, 067);

ifs_fd_t ttpa_rosefd;

void __attribute__ ((weak)) ttpa_master_irg_waitsync(void);
void ttpa_master_irg_waitsync(void)
{
	return;
}


void ttpa_rose_task(ttpa_taskparam_t param)
{
	if(param.rec == 1) {
		ttpa_rosefile.status.active_sec ^= 0x01;
	}
}
/*
 * void ttpa_eor_master(void): End of round; lookup next ROSE entry
 */
void ttpa_eor_master(void)
{
#if defined(USE_RODLCACHE)
	ttpa_curr_frame.stat.framecnt = 
		((ttpa_roseentry_t *) ttpa_rosefd.buf)->irg;
#else
	ttpa_framestat.framecnt = ((ttpa_roseentry_t *) ttpa_rosefd.buf)->irg;
#endif /* USE_RODLCACHE */

	ttpa_master_irg();
}

void ttpa_master_irg(void)
{
	ttpa_slotstart += ttpa_slotlength;
#if defined(USE_RODLCACHE)
	if(ttpa_curr_frame.stat.framecnt == 1) {
#else
	if(ttpa_framestat.framecnt == 1) {
#endif /* USE_RODLCACHE */
		TTPA_OCR = ttpa_slotstart - ttpa_bitlength;
		ttpa_sig_oc = (void (*)(void)) ttpa_master_irg_end;		
	} else {
#if defined(USE_RODLCACHE)
		ttpa_curr_frame.stat.framecnt--;
#else
		ttpa_framestat.framecnt--;
#endif /* USE_RODLCACHE */
		TTPA_TIFR |= (1<<TTPA_OCIE);
		TTPA_TIMSK = (TTPA_TIMSK & ~(1<<TTPA_TICIE)) | (1<<TTPA_OCIE);
		TTPA_OCR = ttpa_slotstart;
		ttpa_sig_oc = (void (*)(void)) ttpa_master_irg;
	}
}


/*
 * void ttpa_sentfb(bus_iobuf_t *busbuf): just sent a FB
 */
void ttpa_sentfb(bus_iobuf_t *busbuf)
{
	// increment mux counter for current round

	if(ttpa_conffile.stat == TTPA_STATE_UNSYNC) {
		ttpa_setstate(TTPA_STATE_ACTIVE);
	}	
	ttpa_newround();
}

/*
 * void ttpa_master_init(void): Master specific startup code
 */
void ttpa_master_init(void)
{
	ttpa_slotlength = SLOTLENGTH_NOMINAL;
	ttpa_bitlength = BITLENGTH_NOMINAL;
	ttpa_slotstart = SLOTLENGTH_NOMINAL;
	ttpa_rose_newseq();
	ttpa_master_irg_waitsync();
	ttpa_master_sendfb();
}

void ttpa_rose_newseq(void)
{
	ifs_addr_t addr;

	addr.filen = TTPA_SYSFILE_ROSE;
	addr.align = 0;
	addr.rec = ttpa_rosefile.status.active_sec ? 
		ttpa_rosefile.status.sec3_start :
		ttpa_rosefile.status.sec2_start;
	// skip starttime and period on begin of section 
	addr.rec += sizeof(ttpa_time_t)>>1;
	ifs_close(&ttpa_rosefd);
	ifs_open(&ttpa_rosefd, addr, IFS_REQ(IFS_ACCESS_APPL, IFS_OP_READ, 1));
}

void ttpa_master_irg_end(void)
{
	ttpa_roseentry_t *roseptr;

	roseptr = (ttpa_roseentry_t*) ttpa_rosefd.buf;
	if(roseptr->eors) {
		ttpa_rose_newseq();
		ttpa_master_irg_waitsync();
		// output external trigger for oscilloscope
		trig();
	} else {
		roseptr++;
		ttpa_rosefd.len -= 4;
		ttpa_rosefd.buf = roseptr;
		// ueberpruefen des file Endes
	}
	ttpa_master_sendfb();
}

void ttpa_master_sendfb(void)
{
	bus_iobuf_t buf;
	
	ttpa_conffile.crnd = buf.buf =
		ttpa_getfb(((ttpa_roseentry_t *) ttpa_rosefd.buf)->rndname);
	buf.par = ~parity_even_bit(buf.buf)&0x01;
	buf.spdup = 0;
	buf.slotstart = ttpa_slotstart;
	bus_op_complete = (void (*) (bus_iobuf_t *)) ttpa_sentfb;
	bus_sendbyte_init(&buf);
}
