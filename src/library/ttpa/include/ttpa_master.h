#ifndef TTPA_MASTER_H
#define TTPA_MASTER_H
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
 * include/ttpa_master.h
 *      Declarations for TTPA functions (master)
 *
 * $Id: ttpa_master.h,v 1.4 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/23 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *
 */
#include "bus.h"
#include "ifs_bitvec.h"

#define TTPA_MEMBERSHP_LEN 256

struct ttpa_membershp_struct {
	ifs_bitvec_t mp_membershp[ifs_bitvec_size(TTPA_MEMBERSHP_LEN)];
	ifs_bitvec_t ms_membershp[ifs_bitvec_size(TTPA_MEMBERSHP_LEN)];
};

extern struct ttpa_membershp_struct ttpa_membershpfile;

// functions for TTPA master
extern void ttpa_master_init(void);

extern void ttpa_eor_master(void);

extern void ttpa_master_irg(void);

extern void ttpa_sentfb(bus_iobuf_t *busbuf);

extern void ttpa_master_init(void);

extern void ttpa_rose_newseq(void);

extern void ttpa_master_irg_end(void);

extern void ttpa_master_irg_waitsync(void);

extern void ttpa_master_sendfb(void);

#endif /* TTPA_MASTER_H */
