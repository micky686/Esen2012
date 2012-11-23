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
 * ttpa/ms_slave.c
 *      MS RODL and functions for TTP/A slave
 *
 * $Id: ms_slave.c,v 1.2 2007-01-16 16:52:22 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - Only empty functions are defined
 *   2004/05/24 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - handling of MS requests
 *   2005/08/24 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added RODL entry caching
 *   2005/12/05 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed to support both, caching and non-caching version
 *
 */
#include "ifs_types.h"
#include "ifs_rodl.h"
#include "ttpa_task.h"
#include "ttpa.h"
#include "ms.h"
#include "ms_slave.h"
#include "ms_request.h"

#if !defined(MASTER)
IFS_MSRODLFILE(msa, 0x05, ifs_int_flash) 
{
	IFS_RE_RECVSYNC(1, IFS_ADDR_I(0x0c,0x01,0x00), 4),
	IFS_RE_EXEC_EOR(6, IFS_ADDR_I(0x0c,0x01,0x00))
};

IFS_MSRODLFILE(msd, 0x01, ifs_int_flash) 
{
	IFS_RE_RECV(1, IFS_ADDR_I(0x0c,0x02,0x01), 4),
	IFS_RE_EXEC_EOR(6, IFS_ADDR_I(0x0c,0x02,0x01))
};
#endif /* MASTER */

/*
 * void ms_slave_init(void): called on start of MSA round
 */
void ms_slave_init(void)
{
}

/*
 * void ms_slave_msa(void): called on end of MSA round, setup MSD
 */
void ms_slave_msa(void)
{
	uint8_t check;
	uint8_t ln;

	// check if MSA was valid
	check = ms_datafile.epoch ^ ms_msa_check(&ms_datafile.msa);
	if(check == ms_datafile.msa.check) {
		// update epoch counter
		ttpa_conffile.ectr = ms_datafile.epoch;
		ln = ms_datafile.msa.ln;
		if((ln == ttpa_conffile.cln) || (ln == LN_BROADC)) {
			ms_datafile.msd_ignore = 0;
			if((ms_datafile.msa.fn_op & OP_MSMSK) != OP_MSWRITE) {
				ms_request(&ms_datafile.msa,
					   &ms_datafile.msd);
#if defined(USE_RODLCACHE)
				ttpa_re_cache[TTPA_FB_MSD&0x07].op =
					                         RODL_OP_SEND;
#endif /* USE_RODLCACHE */
				ms_msd_rodl[0].op = RODL_OP_SEND;
			} else {
#if defined(USE_RODLCACHE)
				ttpa_re_cache[TTPA_FB_MSD&0x07].op =
					                         RODL_OP_RECV;
#endif /* USE_RODLCACHE */
				ms_msd_rodl[0].op = RODL_OP_RECV;
			}
			ms_msd_rodl[0].f_len = 4;
#if defined(USE_RODLCACHE)
			ttpa_re_cache[TTPA_FB_MSD&0x07].stat.framecnt = 4;
#endif /* USE_RODLCACHE */
		} else {
			ms_datafile.msd_ignore = 1;
		}
	} else {
		ttpa_setstate(TTPA_STATE_UNSYNC);
	}
}

/*
 * void ms_slave_msd(void): called on end of MSD round
 */
void ms_slave_msd(void)
{
	uint8_t check;

	if(!ms_datafile.msd_ignore) {
		if((ms_datafile.msa.fn_op & OP_MSMSK) == OP_MSWRITE) {
			check = ms_msd_check(&ms_datafile.msd);
			if(check == ms_datafile.msd.check) {
				ms_request(&ms_datafile.msa,
					   &ms_datafile.msd);
			}
		}
	}
}

