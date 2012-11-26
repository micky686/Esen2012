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
 *      UART functions for monitoring code
 *
 * $Id: mon_uart.c,v 1.2 2007-01-16 16:52:22 bernd Exp $
 *
 * Revision History:
 *   2005/12/05 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - first version
 *
 */
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <string.h>

#include "node_config.h"
#include "mon_uart.h"

mon_uart_pkt_t mon_rx, mon_tx;

#if defined(OPTIMIZE_CALLBACK)
void mon_uart_rx_callback(void);
void mon_uart_tx_callback(void);
#endif /* OPTIMIZE_CALLBACK */

SIGNAL(MON_UART_RX_SIG)
{
	uint8_t stat, data;

	// get status flags
	stat = MON_UART_STATREGA;
	// get data
        data = MON_UART_DATAREG;
	// enable ints to minimize jitter
	sei();
	// disable UART INT
	MON_UART_STATREGB &= ~(1<<MON_UART_RXCIE);
	if(stat & (1<<MON_UART_FE)) {
		mon_rx.error = UART_STAT_FRERR;
		goto callback;
	} else if(stat & (1<<MON_UART_ORE)) {
		mon_rx.error = UART_STAT_OVRERR;
		goto callback;
	} else if(stat & (1<<MON_UART_PE)) {
		mon_rx.error = UART_STAT_PARERR;
		goto callback;
	} else {
		mon_rx.buf[mon_rx.cnt] = data;
		if(mon_rx.cnt == mon_rx.end) {
			MON_UART_STATREGB &= ~(1<<MON_UART_RXEN);
			mon_rx.error = UART_STAT_OK;
			goto callback;
		} else {
			mon_rx.cnt++;
			// enable UART INT
			MON_UART_STATREGB |= (1<<MON_UART_RXCIE);
			return;
		}
	}
 callback:
	MON_UART_STATREGB &= ~(1<<MON_UART_RXEN);
#if defined(OPTIMIZE_CALLBACK)
	mon_uart_rx_callback();
}

SIGNAL(mon_uart_rx_callback)
{
#endif /* OPTIMIZE_CALLBACK */
	mon_uart_pkt_t result;

	memcpy(&result, &mon_rx, sizeof(mon_uart_pkt_t));
	mon_rx.flags &= ~(MON_UART_FL_BUSY);
	if(mon_rx.callback != NULL) {
		mon_rx.callback(&result);
	}
	return;
}

SIGNAL(MON_UART_TX_SIG)
{
	// transmit data
        MON_UART_DATAREG = mon_tx.buf[mon_tx.cnt];
	// enable ints to minimize jitter
	sei();
	// disable UART INT
	MON_UART_STATREGB &= ~(1<<MON_UART_UDRIE);
	if(mon_tx.cnt == mon_tx.end) {
		mon_tx.error = UART_STAT_OK;
		goto callback;
	} else {
		mon_tx.cnt++;
	       	MON_UART_STATREGB |= (1<<MON_UART_UDRIE);
		return;
	}
 callback:
	do {} while(0);
#if defined(OPTIMIZE_CALLBACK)
	mon_uart_tx_callback();
}

SIGNAL(mon_uart_rx_callback)
{
#endif /* OPTIMIZE_CALLBACK */
	mon_uart_pkt_t result;

	memcpy(&result, &mon_tx, sizeof(mon_uart_pkt_t));
	mon_tx.flags &= ~(MON_UART_FL_BUSY);
	if(mon_tx.callback != NULL) {
		mon_tx.callback(&result);
	}
	return;
}

void mon_uart_init(void)
{
	MON_UART_STATREGA = 0;
	MON_UART_STATREGB = (1<<MON_UART_TXEN);
	MON_UART_STATREGC = (1<<MON_UART_PM1) | (1<<MON_UART_UCSZ0) |
		(1<<MON_UART_UCSZ1);
	MON_UART_UBRREGH = MON_UBRH;
	MON_UART_UBRREGL = MON_UBRL;
}

int mon_uart_send(mon_uart_pkt_t *pkt)
{
	if(mon_tx.flags & MON_UART_FL_BUSY) {
		return 1;
	} else {
		// poll until ready to send
		while(!(MON_UART_STATREGA & (1<<MON_UART_UDRE))) {
		}
		if((pkt->flags & MON_UART_FL_PARMSK) == MON_UART_FL_PAREVEN) {
			MON_UART_STATREGC &= ~(1<<MON_UART_PM0);
		} else {
			MON_UART_STATREGC |= (1<<MON_UART_PM0);
		}
		MON_UART_DATAREG = pkt->buf[pkt->cnt];
		if(pkt->cnt == pkt->end) {
			pkt->error = UART_STAT_OK;
			if(pkt->callback != NULL) {
				pkt->callback(pkt);
			}
		} else {
			memcpy(&mon_tx, pkt, sizeof(mon_uart_pkt_t));
			mon_tx.cnt++;
			mon_tx.flags |= MON_UART_FL_BUSY;
			MON_UART_STATREGB |= (1<<MON_UART_UDRIE);
		}
		return 0;
	}
}

int mon_uart_send_poll(mon_uart_pkt_t *pkt)
{
	while(!(MON_UART_STATREGA & (1<<MON_UART_UDRE))) {
	}
	if((pkt->flags & MON_UART_FL_PARMSK) == MON_UART_FL_PAREVEN) {
		MON_UART_STATREGC &= ~(1<<MON_UART_PM0);
	} else {
		MON_UART_STATREGC |= (1<<MON_UART_PM0);
	}
	do {
		// poll until ready to send
		while(!(MON_UART_STATREGA & (1<<MON_UART_UDRE))) {
		}
		MON_UART_DATAREG = pkt->buf[pkt->cnt];
		pkt->cnt++;
	} while(pkt->cnt <= pkt->end);
	if(pkt->callback != NULL) {
		pkt->callback(pkt);
	}
	return 0;
}

int mon_uart_recv(mon_uart_pkt_t *pkt)
{
	if(mon_rx.flags & MON_UART_FL_BUSY) {
		return 1;
	} else {
		memcpy(&mon_rx, pkt, sizeof(mon_uart_pkt_t));
		mon_rx.flags |= MON_UART_FL_BUSY;
		MON_UART_STATREGB |= (1<<MON_UART_RXEN) | (1<<MON_UART_RXCIE);
		return 0;
	}
}
