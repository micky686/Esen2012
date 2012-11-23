#ifndef IFS_BITVEC_H
#define IFS_BITVEC_H
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
 * include/ifs_bitvec.h
 *      macros to access bitvectors
 *
 * $Id: ifs_bitvec.h,v 1.2 2007-01-16 16:52:23 bernd Exp $
 *
 * Revision History:
 *   2004/05/12 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - macros to access bitvectors
 *
 */
#include <inttypes.h>
#include <avr/pgmspace.h>

typedef uint8_t ifs_bitvec_t; 


extern uint8_t __attribute__ ((progmem)) bitvec_dec[];

#define ifs_bitvec_setb(p_addr, p_size, p_idx) \
do { \
	ifs_bitvec_t *addr = \
		p_addr + (ifs_bitvec_size(p_size)-(p_idx>>3)-1); \
	uint8_t bitmsk = __LPM(&bitvec_dec[p_idx & 0x07]); \
	__asm__ __volatile__ ( \
                "in __tmp_reg__,__SREG__" "\n\t"        \
                "cli" "\n\t"                            \
 		"ld  r24,Z" "\n\t"                      \
		"or r24, %A1" "\n\t"                    \
                "out __SREG__,__tmp_reg__" "\n\t"       \
		"st Z, r24" "\n\t"                      \
 		:                                       \
		: "z" (addr), "r" (bitmsk)              \
		: "r24");                               \
} while(0)

#define ifs_bitvec_clearb(p_addr, p_size, p_idx) \
do { \
	ifs_bitvec_t *addr = \
		p_addr + (ifs_bitvec_size(p_size)-(p_idx>>3)-1); \
	uint8_t bitmsk = ~__LPM(&bitvec_dec[p_idx & 0x07]); \
	__asm__ __volatile__ ( \
                "in __tmp_reg__,__SREG__" "\n\t"        \
                "cli" "\n\t"                            \
 		"ld  r24,Z" "\n\t"                      \
		"and r24, %A1" "\n\t"                   \
                "out __SREG__,__tmp_reg__" "\n\t"       \
		"st Z, r24" "\n\t"                      \
 		:                                       \
		: "z" (addr), "r" (bitmsk)              \
		: "r24");                               \
} while(0)

#define ifs_bitvec_getb(addr, size, idx) \
({ \
	uint8_t result; \
	ifs_bitvec_t *addr = \
		p_addr + (ifs_bitvec_size(p_size)-(p_idx>>3)-1); \
	uint8_t bitmsk = ~__LPM(&bitvec_dec[p_idx & 0x07]); \
	result = (*addr & bitmsk) ? 1 : 0; \
	result; \
})

#define ifs_bitvec_size(size) ((size>>3) + ((size%8) ? 1 : 0))

#endif /* IFS_BITVEC_H */
