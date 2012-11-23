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
 * ttpa/ms_master.c
 *      MS RODL and functions for TTP/A master
 *
 * $Id: ms_master.c,v 1.2 2007-01-16 16:52:22 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - Only empty functions are defined
 *   2004/05/10 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - Implementation of MS handling
 *   2004/06/07 (0.3): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added RODL entry caching
 *   2005/12/05 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed to support both, caching and non-caching version
 *   2007/01/10 (0.6): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - fixed bug with gateway interface
 *     - MSD checksum is automatically calculated in MSA round
 *
 */
#include <avr/pgmspace.h>

#include "ifs_types.h"
#include "ifs_rodl.h"
#include "ttpa.h"
#include "ttpa_task.h"
#include "ttpa_fb.h"
#include "ttpa_master.h"
#include "ms.h"
#include "ms_master.h"

uint8_t ms_msdtimeout;

IFS_MSRODLFILE(msa, 0x05, ifs_int_flash) 
{
	IFS_RE_SEND(1, IFS_ADDR_I(0x0c,0x01,0x00), 4),
	IFS_RE_EXEC_EOR(6, IFS_ADDR_I(0x0c,0x01,0x00))
};

IFS_MSRODLFILE(msd, 0x01, ifs_int_flash) 
{
	IFS_RE_SEND(1, IFS_ADDR_I(0x0c,0x02,0x01), 4),
	IFS_RE_EXEC_EOR(6, IFS_ADDR_I(0x0c,0x02,0x01))
};

/*
 * void ms_master_init(void): called on start of MSA round, setup MS request
 */
void ms_master_init(void)
{
	// copy current epoch to MSA data file and calculate checksum
	ms_datafile.epoch = ttpa_conffile.ectr;
	ms_datafile.msa.check = ms_datafile.epoch ^ ms_datafile.msa.ln ^
		ms_datafile.msa.fn_op ^ ms_datafile.msa.rec  ^ TTPA_FB_MSA;
}

/*
 * void ms_master_msa(void): called on end of MSA round, setup MSD
 */
void ms_master_msa(void)
{
	// set RODL operation corresponding to MS request
	if((ms_datafile.msa.fn_op & OP_MSMSK) == OP_MSWRITE) {
		ms_msd_rodl[0].op = RODL_OP_SEND;
#if defined(USE_RODLCACHE)
		ttpa_re_cache[TTPA_FB_MSD&0x07].op = RODL_OP_SEND;
#endif /* USE_RODLCACHE */
		// calculate check byte for MSD round
		ms_datafile.msd.check = ms_msd_check(&ms_datafile.msd);
	} else {
		ms_msd_rodl[0].op = RODL_OP_RECV;
#if defined(USE_RODLCACHE)
		ttpa_re_cache[TTPA_FB_MSD&0x07].op = RODL_OP_RECV;
#endif /* USE_RODLCACHE */
	}
	ms_msdtimeout = 0;
}

/*
 * void ms_master_msd(void): called on end of MSD round
 */
void ms_master_msd(void)
{
	uint8_t i, msk;
	void (* task) (void);

	// update membership vector
	if(ms_msdtimeout) {
		ifs_bitvec_clearb(ttpa_membershpfile.ms_membershp,
				  TTPA_MEMBERSHP_LEN, ms_datafile.msa.ln);
	} else {
		ifs_bitvec_setb(ttpa_membershpfile.ms_membershp,
				  TTPA_MEMBERSHP_LEN, ms_datafile.msa.ln);
	}
	task = (void (*) (void)) pgm_read_word(
		&ms_handler[ms_datafile.caller].callback);
	(*task)();
	msk = (1<<MS_HANDLER_MAX)-1;
	for(i = MS_HANDLER_MAX; i>0; i--) {
		if(ms_datafile.ms_pending[0] > msk) {
			break;
		}
		msk >>= 1;
	}
	ms_msd_rodl[0].f_len = 4;
	task = (void (*) (void)) pgm_read_word(
		&ms_handler[i].setup);
	ms_datafile.caller = i;
	(*task)();
}

static uint8_t ms_idlecnt = LN_BROADC+1;

void ms_setup_idle(void)
{
	ms_datafile.msa.ln = ms_idlecnt;
	ms_datafile.msa.fn_op = OP_MSREAD | (TTPA_SYSFILE_DOC<<2);
	ms_datafile.msa.rec = 0x01;
}

void ms_callback_idle(void)
{
	ms_idlecnt++;
	if(ms_idlecnt == LN_MASTER) {
		ms_idlecnt = LN_BROADC+1;
		// look if there are unbaptized nodes
		ifs_bitvec_setb(ms_datafile.ms_pending, 8, 1);
	}

}

void ms_callback_baptize(void) __attribute__ ((weak));
void ms_callback_baptize(void)
{
	ifs_bitvec_clearb(ms_datafile.ms_pending, 8, 1);
}

void ms_setup_baptize(void) __attribute__ ((naked, weak));
void ms_setup_baptize(void)
{
}
void ms_setup_owner(void) __attribute__ ((naked, weak));
void ms_setup_owner(void)
{
}
void ms_callback_owner(void) __attribute__ ((naked, weak));
void ms_callback_owner(void)
{
}
void ms_setup_gw0(void) __attribute__ ((naked, weak));
void ms_setup_gw0(void)
{
}
void ms_callback_gw0(void) __attribute__ ((naked, weak));
void ms_callback_gw0(void)
{
}
void ms_setup_gw1(void) __attribute__ ((naked, weak));
void ms_setup_gw1(void)
{
}
void ms_callback_gw1(void) __attribute__ ((naked, weak));
void ms_callback_gw1(void)
{
}
void ms_setup_gw2(void) __attribute__ ((naked, weak));
void ms_setup_gw2(void)
{
}
void ms_callback_gw2(void) __attribute__ ((naked, weak));
void ms_callback_gw2(void)
{
}
void ms_setup_gw3(void) __attribute__ ((naked, weak));
void ms_setup_gw3(void)
{
}
void ms_callback_gw3(void) __attribute__ ((naked, weak));
void ms_callback_gw3(void)
{
}
void ms_setup_gw4(void) __attribute__ ((naked, weak));
void ms_setup_gw4(void)
{
}
void ms_callback_gw4(void) __attribute__ ((naked, weak));
void ms_callback_gw4(void)
{
}
void ms_callback_weak(void)
{
	ifs_bitvec_clearb(ms_datafile.ms_pending, 8, ms_datafile.caller);
}


ms_handler_t __attribute__ ((progmem)) ms_handler[] = {
	{ ms_setup_idle , ms_callback_idle},
	{ ms_setup_baptize, ms_callback_baptize},
	{ ms_setup_owner, ms_callback_owner},
	{ ms_setup_gw0, ms_callback_gw0},
	{ ms_setup_gw1, ms_callback_gw1},
	{ ms_setup_gw2, ms_callback_gw2},
	{ ms_setup_gw3, ms_callback_gw3},
	{ ms_setup_gw4, ms_callback_gw4}
};
