#ifndef MS_H
#define MS_H
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
 * include/ms.h
 *      Declarations for MS files and functions
 *
 * $Id: ms.h,v 1.2 2007-01-16 16:52:23 bernd Exp $
 *
 * Revision History:
 *   2004/04/23 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/05/13 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed IFS_MSRODLFILE(), ms_data_struct
 *
 */
#include "ifs.h"
#include "ifs_types.h"
#include "ifs_rodl.h"
#include "ifs_bitvec.h"
#include "ttpa_task.h"
#include "ms_request.h"

// Structure of MS Data File (0x0C)
struct ms_data_struct {
	ifs_uint8_t epoch;
	ms_req_t msa;
	ms_data_t msd;
	union {
		// msd_ignore: 0, set to 1 if next MSD round should be ignored
		ifs_uint8_t msd_ignore;
		// ms_pending: shows if any requests are pending (master)
		ifs_bitvec_t ms_pending[ifs_bitvec_size(8)];
	};
	ifs_uint8_t caller;
};

extern struct ms_data_struct ms_datafile;

#define IFS_MSRODLFILE(name, fn, loc) \
ifs_rodlentry_t __attribute__ ((section("." #loc))) ms_## name ##_rodl[2]; \
IFS_ADDRODLFILE(fn, ms_## name ##_rodl, NULL, 2, loc, 000); \
ifs_rodlentry_t __attribute__ ((section("." #loc))) ms_## name ##_rodl[2] = 

extern ifs_rodlentry_t  ms_msa_rodl[2];

extern ifs_rodlentry_t  ms_msd_rodl[2];

extern void ms_task(ttpa_taskparam_t param);

#endif /* MS_H */
