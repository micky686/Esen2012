#ifndef BUS_H
#define BUS_H
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
 * include/bus.h
 *      Declarations for global UART functions and datastructures
 *
 * $Id: bus.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *    2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *
 */
#include <inttypes.h>

typedef enum { 
	BUS_IO_OK, 
	BUS_IO_TIMEOUT,
	BUS_IO_FE
} bus_io_err_t; 

// parameter for bus_sendbyte_init and callback bus_op_complete

typedef struct {
	uint8_t buf; // byte buffer
	uint8_t par; // parity
	uint16_t slotstart; // start of uart slot
	union {
		struct {
			unsigned spdup : 3; // speedup, union with errorcode
			unsigned timeout :1; // 1: signal timeout, 0: wait
		};
		bus_io_err_t error : 8;
	};
} bus_iobuf_t;

// global functions

// setup of bus related HW
extern int bus_init(void);

// transmit byte to bus
extern void bus_sendbyte_init(bus_iobuf_t *param);

// receive byte from bus
extern void bus_recvbyte_init(bus_iobuf_t *param);

// start synchronization (slave only)
extern void bus_sync(void);

// callback: bus operation complete
extern void (* volatile bus_op_complete) (bus_iobuf_t *);

#endif /* BUS_H */
