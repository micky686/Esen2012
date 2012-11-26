#ifndef MON_STATEM_H
#define MON_STATEM_H
/* Copyright (c) 2005, Christian Trödhandl
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
 * ttpa/mon_uart.c
 *      declarations for monitoring state-machine
 *
 * $Id: mon_statem.h,v 1.2 2007-01-16 16:52:23 bernd Exp $
 *
 * Revision History:
 *   2005/12/05 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - first version
 *   2005/12/14 (0.6): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - changed MON_OP definitions
 *
 */
#include <inttypes.h>

#include "node_config.h"
#include "mon_uart.h"
#include "ttpa_fb.h"

#define MON_OP_READ 0x00
#define MON_OP_WRITE 0x01
#define MON_OP_EXEC 0x03
#define MON_OP_MSK 0x03

#define MON_BUFLEN (1<<7)
#define MON_BUFMASK (MON_BUFLEN-1)
#define MON_MAXRID 256

#if !defined(MON_FB_MSA)
#define MON_FB_MSA TTPA_FB_MSA
#endif /* MON_FB_MSA */

#if !defined(MON_FB_MSD)
#define MON_FB_MSD TTPA_FB_MSD
#endif /* MON_FB_MSD */

#define MON_TIFR TIFR

#define MON_TIMSK TIMSK

#define MON_TCNT TCNT0

#define MON_TCCR TCCR0

#define MON_OCR OCR0

#define MON_TOIE TOIE0

#define MON_OCIE OCIE0

#define MON_SIG_TOVF SIG_OVERFLOW0

#define MON_SIG_OC SIG_OUTPUT_COMPARE0

//#define MON_TPRESCALE ((1<<CS00) | (1<<CS01) | (1<<CS02))
//#define PRESC_FAKT 1024UL
#define MON_TPRESCALE (1<<CS01)
#define PRESC_FAKT 8UL

#if !defined(MON_TIMEOUT)
#define MON_TIMEOUT 14 // timeout in UART frames (MSA+MSD+2*ACK)
#endif /* MON_TIMEOUT */

#define MON_BITS_FRAME 11UL

#define MON_TIMEOUTVAL ((uint8_t) \
((((unsigned long) CLOCKRATE)*((unsigned long) MON_TIMEOUT)*MON_BITS_FRAME)/ \
(PRESC_FAKT*256UL*((unsigned long) MON_BAUDRATE))))

typedef struct {
	uint8_t cluster;
	uint8_t ln;
	uint8_t fn_op;
	uint8_t rec;
	uint8_t rid;
	uint8_t check;
} mon_req_t;

typedef struct {
	uint8_t data[4];
	uint8_t rid;
	uint8_t check;
} mon_data_t;

typedef enum {
	MON_BUF_FREE,
	MON_BUF_GETMSA,
	MON_BUF_GETMSD,
	MON_BUF_CURRENT,
	MON_BUF_PENDING,
	MON_BUF_READY
} mon_bufstat_t;

typedef struct {
	uint8_t status;
	mon_req_t req;
	mon_data_t data;
} mon_buf_t;

#define MON_RID_NOTREADY 0xff

extern mon_buf_t mon_rb[MON_BUFLEN];

extern uint8_t mon_rid[MON_MAXRID];

extern uint8_t mon_freeind, mon_pendind;

extern int mon_init(void);

extern void mon_timer_init(void);

extern void mon_timer_set(uint8_t timeout, void (*cb)(void));

extern void mon_timeout_cb(void);

extern void mon_msa_cb(mon_uart_pkt_t *pkt);

extern void mon_msd_read_cb(mon_uart_pkt_t *pkt);

extern void mon_msd_write_cb(mon_uart_pkt_t *pkt);

#endif /* MON_STATEM_H */

