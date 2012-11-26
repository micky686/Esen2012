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
 * include/ms.c
 *      MS datafile and MS task
 *
 * $Id: ms.c,v 1.2 2007-01-16 16:52:22 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/05/06 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed IFS_ADDSYSFILE
 *
 */
#include "ifs.h"
#include "ifs_types.h"
#include "ifs_rodl.h"
#include "ttpa_task.h"
#include "ttpa_sysfiles.h"
#include "ms.h"
#include "ms_slave.h"
#include "ms_master.h"

struct ms_data_struct IFS_LOC(ifs_int_0) ms_datafile;

IFS_ADDSYSFILE(TTPA_SYSFILE_MSDATA, &ms_datafile, ms_task, IFS_FILELEN(struct ms_data_struct), ifs_int_eep, 070);

void ms_task(ttpa_taskparam_t param);

/*
 * void ms_task(ttpa_taskparam_t param): TTP/A task called on end of MSA or
 * MSD round; call corresponding master or slave task
 */
void ms_task(ttpa_taskparam_t param)
{
	if(ttpa_conf_master()) {
		switch(param.rec) {
		case 0x01: ms_master_msa();
			break;
		case 0x02: ms_master_msd();
			break;
		}
	} else {
		switch(param.rec) {
		case 0x01: ms_slave_msa();
			break;
		case 0x02: ms_slave_msd();
			break;
		}
	}
	return;
}

