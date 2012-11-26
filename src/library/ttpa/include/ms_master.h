#ifndef MS_MASTER_H
#define MS_MASTER_H
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
 * include/ms_master.h
 *      MS function declaration for TTP/A master
 *
 * $Id: ms_master.h,v 1.2 2007-01-16 16:52:23 bernd Exp $
 *
 * Revision History:
 *   2004/04/23 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/05/12 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added ms_msdtimeout
 *   2007/01/10 (0.6): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed MS_HANDLER_MAX
 *
 */
#include <inttypes.h>

// define maximum number of MS handler functions
#if !defined(MS_HANDLER_MAX)
#define MS_HANDLER_MAX 3 // gw 0; Change if you use additional gw functions
#endif /* MS_HANDLER_MAX */

typedef struct {
	void (* setup) (void);
	void (* callback) (void);
} ms_handler_t;

extern ms_handler_t __attribute__ ((progmem)) ms_handler[];

extern void ms_master_init(void);

extern void ms_master_msa(void);

extern void ms_master_msd(void);

extern void ms_init_idle(void);

extern void ms_callback_idle(void);

extern void ms_init_baptize(void);

extern void ms_callback_baptize(void);

extern void ms_setup_owner(void);

extern void ms_callback_owner(void);

extern void ms_setup_gw0(void);

extern void ms_callback_gw0(void);

extern void ms_setup_gw1(void);

extern void ms_callback_gw1(void);

extern void ms_setup_gw2(void);

extern void ms_callback_gw2(void);

extern void ms_setup_gw3(void);

extern void ms_callback_gw3(void);

extern void ms_setup_gw4(void);

extern void ms_callback_gw4(void);

extern uint8_t ms_msdtimeout; // set to 1 if no response from slave

#endif /* MS_MASTER_H */
