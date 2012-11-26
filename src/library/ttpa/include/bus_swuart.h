#ifndef BUS_SWUART_H
#define BUS_SWUART_H
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
 * include/bus_swuart.h
 *      Declarations for global bus functions and datastructures
 *
 * $Id: bus_swuart.h,v 1.2 2007-01-16 16:52:23 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *
 */

// buffer for storing byte for transmission
#ifdef OPTIMIZE_SRAM
typedef struct {
	uint8_t buf;
	unsigned cnt : 4;
	unsigned bitbuf : 1;
	unsigned par : 1;
	unsigned timeout : 1;
} bus_swuart_buf_t;
#else
typedef struct {
	uint8_t buf;
	uint8_t cnt;
	union {
		uint8_t bitbuf;
		uint8_t timeout;
	};
	uint8_t par;
} bus_swuart_buf_t;
#endif /* OPTIMIZE_SRAM */

extern volatile uint16_t bus_eff_bitlen;

extern volatile uint16_t bus_timer_ic;

extern volatile bus_swuart_buf_t bus_swuart_buf;

extern void bus_sendbyte(void);

extern void bus_recvbyte(void);

extern void bus_recvbyte_setup(void);

extern void bus_recvbyte_startb(void);

extern void bus_recvbyte_to(void);

// handler for input capture
extern void (* volatile bus_sig_ic) (void);

#endif /* BUS_SWUART_H */
