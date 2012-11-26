#ifndef BUS_TRANSCVR_H
#define BUS_TRANSCVR_H
/* Copyright (c) 2004, 2005, Christian Trödhandl
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
 * include/bus_transcvr.h
 *      defines macros for various transceiver types
 *
 * $Id: bus_transcvr.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/03/07 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *   2004/12/10 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - Transceiver definition
 *   2005/12/05 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - fixed bug in RS485 macros
 *
 */
#include "node_swconfig.h"

#define bus_transcvr_sw_init(transcvr) bus_transcvr_sw_init_x(transcvr)
#define bus_transcvr_sw_send(transcvr) bus_transcvr_sw_send_x(transcvr)
#define bus_transcvr_sw_recv(transcvr) bus_transcvr_sw_recv_x(transcvr)

// nothing to do for ISO K
#define bus_transcvr_isok_init() do {} while(0)

#define bus_transcvr_isok_send() do {} while(0)

#define bus_transcvr_isok_recv() do {} while(0)

// transceiver settings for RS485
#define bus_transcvr_rs485_init() do { \
        TRANSCVR_IOPORT &= ~(1<<TRANSCVR_PORT); \
        TRANSCVR_IODDR |= (1<<TRANSCVR_PORT); \
} while(0)

#define bus_transcvr_rs485_send() do { \
        TRANSCVR_IOPORT |= (1<<TRANSCVR_PORT); \
} while(0)

#define bus_transcvr_rs485_recv() do { \
        TRANSCVR_IOPORT &= ~(1<<TRANSCVR_PORT); \
} while(0)

// initialization for SW configurable nodes
#define bus_transcvr_sw_init_x(transcvr) do { \
        node_swconf_transcvr_##transcvr(); \
        bus_transcvr_##transcvr##_init(); \
} while(0)

#define bus_transcvr_sw_send_x(transcvr) do { \
        bus_transcvr_##transcvr##_send(); \
} while(0)

#define bus_transcvr_sw_recv_x(transcvr) do { \
        bus_transcvr_##transcvr##_recv(); \
} while(0)

#endif /* BUS_TRANSCVER_H */
