#ifndef INIT_RUNLEVEL_H
#define INIT_RUNLEVEL_H
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
 * include/init_runlevel.h
 *	Declaration of functions and types for runlevel initialization
 *
 * $Id: init_runlevel.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/02/27 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *
 */

// declared runlevels (0-15)
#define INIT_RLVL_COLDSTART_SYS 0
#define INIT_RLVL_COLDSTART_APPL 1
#define INIT_RLVL_SYNC_SYS 2
#define INIT_RLVL_SYNC_APPL 3
#define INIT_RLVL_BAPT_SYS 4
#define INIT_RLVL_BAPT_APPL 5
#define INIT_RLVL_BG_SYS 6
#define INIT_RLVL_BG_APPL 7


// define function pointer type for init functions
typedef void (*init_func_t)(void);

// calls all init functions for a given runlevel
void init_runlevel(uint8_t runlevel);

#define INITTAB_ADDENTRY(runlevel, func) \
init_func_t itab_entry_ ## func __attribute__ ((section(".inittab." #runlevel))) = \
        = (init_func_t) func;

#endif /* INIT_RUNLEVEL_H */
