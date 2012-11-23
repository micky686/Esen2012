#ifndef DEBUG_H
#define DEBUG_H
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
 * include/debug.h
 *      Debug macros
 *
 * $Id: debug.h,v 1.2 2007-01-16 16:52:23 bernd Exp $
 *
 * Revision History:
 *   2004/03/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *
 */
extern uint8_t *debugptr;
extern volatile uint16_t debugidx;
#define DWINDOW 0x07ff

#define DEBUG_E0S(port, pin) PORT ## port &= ~(1<<P ## port ## pin)
#define DEBUG_E1S(port, pin) PORT ## port |= (1<<P ## port ## pin)
#define DEBUG_E0(port, pin) DEBUG_E0S(port, pin)
#define DEBUG_E1(port, pin) DEBUG_E1S(port, pin)
#define DEBUG_SETDDRS(port) DDR ## port = 0xff
#define DEBUG_SETDDR(port) DEBUG_SETDDRS(port)
#ifdef DEBUG_PORT
#define DEBUG_INIT() \
do { \
	DEBUG_SETDDR(DEBUG_PORT); \
} while(0)
#define DEBUG(pin, cnt) \
do { \
	uint8_t i; \
        DEBUG_E0(DEBUG_PORT, pin); \
	for(i = cnt;i;i--) { \
		asm volatile("nop" "\n\t" ::); \
	} \
	DEBUG_E1(DEBUG_PORT, pin); \
} while(0)
#define DEBUG_LOG(label,num) \
do { \
	debugptr[DWINDOW & debugidx++] = (uint8_t) (label); \
	debugptr[DWINDOW & debugidx++] = (uint8_t) (num); \
	debugptr[DWINDOW & debugidx] = (uint8_t) ('@'); \
	debugptr[DWINDOW & debugidx+1] = (uint8_t) ('@'); \
} while(0)
#else
#define DEBUG_LOG(label,num) do {} while (0)
#define DEBUG_INIT() do {} while (0)
#define DEBUG(pin, cnt) do {} while (0)
#endif /* DEBUG_PORT */

#endif /* DEBUG_H */
