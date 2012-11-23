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
 * ttpa/ttpa_sysfiles.c
 *      TTP/A IFS system files
 *
 * $Id: ttpa_sysfiles.c,v 1.2 2007-01-16 16:52:22 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/05/06 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added ; on end of IFS_ADD_SYSFILE()
 *
 */
#include "ifs.h"
#include "ifs_types.h"
#include "ttpa_sysfiles.h"
#include "ttpa_task.h"
#include "services_slave.h"
#include "ms.h"

struct ttpa_conffile_struct IFS_LOC(ifs_int_eep) ttpa_conffile = {
	IFS_UINT8_I(0x00), // status
	IFS_UINT8_I(CN_NODE), // cluster name
	IFS_UINT8_I(0x00), // logical name for baptize
	IFS_UINT8_I(LN_NODE), // logical name
	{
		{
			IFS_UINT32_I(0x00000000), // baptize series
			IFS_UINT32_I(0x00000000) // baptize serial
		}
	},
	IFS_UINT8_I(0x00), // mux code for current round
	IFS_UINT8_I(0x00), // current round
	IFS_UINT8_I(0x00), // current epoch
	IFS_UINT8_I(0x00)  // current slot
#if defined(SUPPORT_SLEEP)
	, {
		IFS_UINT8_I(0x00), // sleep
		IFS_UINT8_I(0x00),
		IFS_UINT8_I(0x00),
		IFS_UINT8_I(0x00)
	}
#endif /* SUPPORT_SLEEP */	
};

void ttpa_conf_task(ttpa_taskparam_t param);

IFS_ADDSYSFILE(TTPA_SYSFILE_CONF, &ttpa_conffile, ttpa_conf_task, IFS_FILELEN(struct ttpa_conffile_struct), ifs_int_eep, 067);

/*
 * void ttpa_conf_task(ttpa_taskparam_t param): conf file TTP/A task;
 * call corresponding service routines
 */
void ttpa_conf_task(ttpa_taskparam_t param)
{
	if(ttpa_conf_master()) {
		// master services
	} else {
		switch(param.rec) {
#if defined(SUPPORT_BAPTIZE)
		case 0x01: services_bapt_assign_slave();
			break;
		case 0x02: services_bapt_comp_slave();
			break;
#endif /* SUPPORT_BAPTIZE */	
#if defined(SUPPORT_SLEEP)
		case 0x05: services_sleep_slave();
			break;
#endif /* SUPPORT_SLEEP */	
		default: ms_datafile.msd_ignore = 1;
			break;
		}
	}
}

struct ttpa_docfile_struct IFS_LOC(ifs_flash) ttpa_docfile = {
	IFS_UINT32_I(SERIES_NR),
	IFS_UINT32_I(SERIAL_NR)
};

IFS_ADDSYSFILE(TTPA_SYSFILE_DOC, &ttpa_docfile, NULL, IFS_FILELEN(struct ttpa_docfile_struct), ifs_flash, 044);
