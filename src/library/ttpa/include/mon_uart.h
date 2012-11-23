#ifndef MON_UART_H
#define MON_UART_H
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
 * include/mon_uart.h
 *      UART functionheaders for monitoring code
 *
 * $Id: mon_uart.h,v 1.2 2007-01-16 16:52:23 bernd Exp $
 *
 * Revision History:
 *   2005/12/05 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - first version
 *
 */
#include "node_config.h"

#if !defined(MON_BAUDRATE)
#define MON_BAUDRATE 57600
#endif /* MON_BAUDRATE */

#define MON_UBRH ((uint8_t) (((uint32_t) CLOCKRATE/((uint32_t)16*(uint32_t) \
        MON_BAUDRATE) - 1)>>8));
#define MON_UBRL ((uint8_t) (((uint32_t) CLOCKRATE/((uint32_t)16*(uint32_t) \
        MON_BAUDRATE) - 1)&0x000000ff));

#if !defined(MON_PARITY)
#define MON_PARITY MON_UART_FL_PAREVEN
#endif /* MON_PARITY */

typedef enum {UART_STAT_OK, 
	      UART_STAT_TIMEOUT,
	      UART_STAT_PARERR,
	      UART_STAT_FRERR,
	      UART_STAT_OVRERR
} mon_uart_stat_t;

typedef struct mon_uart_pkt_t{
	uint8_t *buf;
	uint8_t end;
	uint8_t cnt;
	uint8_t flags;
	uint8_t error;
	void (* callback)(struct mon_uart_pkt_t *);
} mon_uart_pkt_t;

#define MON_UART_FL_PAREVEN 0x00
#define MON_UART_FL_PARODD 0x01
#define MON_UART_FL_PARMSK  0x01
#define MON_UART_FL_BUSY 0x80

extern void mon_uart_init(void);

extern int mon_uart_send(mon_uart_pkt_t *pkt);

extern int mon_uart_send_poll(mon_uart_pkt_t *pkt);

extern int mon_uart_recv(mon_uart_pkt_t *pkt);

#if !defined(MON_UART_RX_SIG)
#define MON_UART_RX_SIG SIG_UART1_RECV
#endif /* MON_UART_RX_SIG */

#if !defined(MON_UART_TX_SIG)
#define MON_UART_TX_SIG SIG_UART1_DATA
#endif /* MON_UART_TX_SIG */

#if !defined(MON_UART_DATAREG)
#define MON_UART_DATAREG UDR1
#endif /* MON_UART_DATAREG */

#if !defined(MON_UART_STATREGA)
#define MON_UART_STATREGA UCSR1A
#endif /* MON_UART_STATREGA */

#if !defined(MON_UART_STATREGB)
#define MON_UART_STATREGB UCSR1B
#endif /* MON_UART_STATREGB */

#if !defined(MON_UART_STATREGC)
#define MON_UART_STATREGC UCSR1C
#endif /* MON_UART_STATREGC */

#if !defined(MON_UART_RXC)
#define MON_UART_RXC RXC1
#endif /* MON_UART_FE */

#if !defined(MON_UART_TXC)
#define MON_UART_TXC TXC1
#endif /* MON_UART_FE */

#if !defined(MON_UART_UDRE)
#define MON_UART_UDRE UDRE1
#endif /* MON_UART_FE */

#if !defined(MON_UART_FE)
#define MON_UART_FE FE1
#endif /* MON_UART_FE */

#if !defined(MON_UART_ORE)
#define MON_UART_ORE DOR1 
#endif /* MON_UART_ORE */

#if !defined(MON_UART_PE)
#define MON_UART_PE UPE1
#endif /* MON_UART_PE */

#if !defined(MON_UART_RXCIE)
#define MON_UART_RXCIE RXCIE1
#endif /* MON_UART_RXCIE */

#if !defined(MON_UART_TXCIE)
#define MON_UART_TXCIE TXCIE1
#endif /* MON_UART_TXCIE */

#if !defined(MON_UART_UDRIE)
#define MON_UART_UDRIE UDRIE1
#endif /* MON_UART_UDRIE */

#if !defined(MON_UART_RXEN)
#define MON_UART_RXEN RXEN1
#endif /* MON_UART_RXEN */

#if !defined(MON_UART_TXEN)
#define MON_UART_TXEN TXEN1
#endif /* MON_UART_TXEN */

#if !defined(MON_UART_UCSZ0)
#define MON_UART_UCSZ0 UCSZ10
#endif /* MON_UART_UCSZ0 */

#if !defined(MON_UART_UCSZ1)
#define MON_UART_UCSZ1 UCSZ11
#endif /* MON_UART_UCSZ1 */

#if !defined(MON_UART_UCSZ2)
#define MON_UART_UCSZ2 UCSZ12
#endif /* MON_UART_UCSZ2 */

#if !defined(MON_UART_PM0)
#define MON_UART_PM0 UPM10
#endif /* MON_UART_PM0 */

#if !defined(MON_UART_PM1)
#define MON_UART_PM1 UPM11
#endif /* MON_UART_PM1 */

#if !defined(MON_UART_USBS)
#define MON_UART_USBS USBS1
#endif /* MON_UART_USBS */

#if !defined(MON_UART_UBRREGL)
#define MON_UART_UBRREGL UBRR1L
#endif /* MON_UART_UBRREGL */

#if !defined(MON_UART_UBRREGH)
#define MON_UART_UBRREGH UBRR1H
#endif /* MON_UART_UBRREGH */

#endif /* MON_UART_H */
