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
 * ttpa/bus_swuart.c
 *      general functions for SW UART
 *
 * $Id: bus_swuart.c,v 1.4 2007-01-18 19:18:46 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     TODO: - Test receive (depends on working slave node)
 *           - Values for SW_UART_RECVSUCORR, SW_UART_RECVCORR, and
 *             SW_UART_SENDSUCORR need to be specified
 *   2004/05/04 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added receive
 *   2004/06/14 (0.3): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added ADD_INITTASK
 *   2005/08/22 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added include node_swconfig.h
 *     - fixed hanging on startup (bug in handling of timer flags)
 *
 */
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <stdio.h>

#include "node_config.h"
#include "node_swconfig.h"
#include "ttpa.h"
#include "bus.h"
#include "bus_swuart.h"
#include "bus_transcvr.h"
#include "schedule.h"
#include "debug.h"

volatile bus_swuart_buf_t bus_swuart_buf;

volatile uint16_t bus_eff_bitlen; // calculated bitlength

volatile uint16_t bus_timer_ic;

// handler for input capture
void (* volatile bus_sig_ic) (void);

// callback: bus operation complete
void (* volatile bus_op_complete) (bus_iobuf_t *param); 

/*
 * void bus_init(void): Initialize SW UART
 */
int bus_init(void)
{
#ifdef SW_UART_TXUSEOC
	// set OC output high
	SW_UART_TXOCIODDR |= (1<<SW_UART_TXOCPORT);
	// Reset OC output
	TTPA_TCCRA |= ((1<<TTPA_COM0)|(1<<TTPA_COM1));
	//	TTPA_TCCRC = (1<<TTPA_FOC);
#endif /* SW_UART_TXUSEOC */

#ifdef SW_UART_TXUSEPORT
	// set TX output high
	SW_UART_TXIOPORT |= (1<<SW_UART_TXPORT);
	SW_UART_TXIODDR |= (1<<SW_UART_TXPORT);
#endif /* SW_UART_TXUSEPORT */

	// set RX as input with pullup
	SW_UART_RXIOPORT |= (1<<SW_UART_RXPORT);
	SW_UART_RXIODDR &= ~(1<<SW_UART_RXPORT);
	// initialize transceiver
	node_swconf_swuart();
	bus_transcvr_init();

	return 0;
}

ADD_INITTASK(task_bus_init, bus_init, 2, (1<<TTPA_STATE_UNSYNC));

/*
 * int bus_error(void): disable uart on error
 */
int bus_error(void)
{
	TTPA_TIMSK &= ~(1<<TTPA_TICIE);
	bus_transcvr_recv();
#ifdef SW_UART_TXUSEOC
	// disable OC output
	SW_UART_TXOCIODDR &= ~(1<<SW_UART_TXOCPORT);
	// Reset OC output
	TTPA_TCCRA |= ((1<<TTPA_COM0)|(1<<TTPA_COM1));
	//	TTPA_TCCRC = (1<<TTPA_FOC);
#endif /* SW_UART_TXUSEOC */

#ifdef SW_UART_TXUSEPORT
	// disable TX output
	SW_UART_TXIOPORT |= (1<<SW_UART_TXPORT);
	SW_UART_TXIODDR &= ~(1<<SW_UART_TXPORT);
#endif /* SW_UART_TXUSEPORT */

	return 0;
}

ADD_INITTASK(task_bus_error, bus_error, 2, (1<<TTPA_STATE_ERROR));

/*
 *  SIGNAL(BUS_SIG_IC): signal handler for output compare match interrupt
 */
SIGNAL(BUS_SIG_IC)
{
	DEBUG(1, 25);
	// call handler code
	(*bus_sig_ic)();
}

/*
 * void bus_recvbyte_init(bus_iobuf_t *param): Initialize SW UART for receive
 * operation in next slot
 */
void bus_recvbyte_init(bus_iobuf_t *param)
{
	bus_eff_bitlen = ttpa_bitlength>>param->spdup;
	bus_swuart_buf.cnt = 0;
	bus_swuart_buf.timeout = param->timeout;
	// set OC handler
	ttpa_sig_oc = (void (*)(void)) bus_recvbyte_setup;
	// set timer
#ifdef SW_UART_RECVSUCORR
	TTPA_OCR = param->slotstart + SW_UART_RECVSUCORR;
#else
	TTPA_OCR = param->slotstart;
#endif /* SW_UART_RECVSUCORR */
	// enable OC, disable IC interrupt
	TTPA_TIFR = (1<<TTPA_OCIE);
	TTPA_TIMSK = (TTPA_TIMSK & ~(1<<TTPA_TICIE)) | (1<<TTPA_OCIE);

	// set transceiver direction to recv
	bus_transcvr_recv();
}

/* 
 * bus_recvbyte_setup(): setup of receive one BC before expected start
 */
void bus_recvbyte_setup(void)
{
	// set timeout to slotstart + 2BC
	TTPA_OCR += ttpa_bitlength<<1;
	// set IC handler
	bus_sig_ic = bus_recvbyte_startb;
	// set IC to falling edge
	TTPA_TCCRB = (1<<TTPA_CS0);
	if(bus_swuart_buf.timeout) {
		// set OC handler
		ttpa_sig_oc = (void (*)(void)) bus_recvbyte_to;
		// enable OC and IC interrupt
		TTPA_TIFR = (1<<TTPA_OCIE) | (1<<TTPA_TICIE);
		TTPA_TIMSK |= (1<<TTPA_TICIE) | (1<<TTPA_OCIE);
	} else {
		TTPA_TIFR = (1<<TTPA_TICIE);
		TTPA_TIMSK = (TTPA_TIMSK & ~(1<<TTPA_OCIE)) | (1<<TTPA_TICIE);
	}
}

/*
 * void bus_recvbyte_startb(void): Falling edge of start bit has just been
 * received
 */
void bus_recvbyte_startb(void)
{
	// save capture time
	bus_timer_ic = TTPA_ICR;
	// set OC handler
	ttpa_sig_oc = (void (*)(void)) bus_recvbyte;
#ifdef SW_UART_RECVCORR
	TTPA_OCR = bus_timer_ic + bus_eff_bitlen + (bus_eff_bitlen>>1) + 
		SW_UART_RECVCORR;
#else
	TTPA_OCR = bus_timer_ic + bus_eff_bitlen + (bus_eff_bitlen>>1);
#endif /* SW_UART_RECVCORR */
	// enable OC, disable IC interrupt
	TTPA_TIFR = (1<<TTPA_OCIE);
	TTPA_TIMSK = (TTPA_TIMSK & ~(1<<TTPA_TICIE)) | (1<<TTPA_OCIE);
}

/*
 * void bus_recvbyte(void): sample bits
 */
void bus_recvbyte(void)
{
	uint8_t sample, cnt, bitbuf;
	DEBUG(0,1);
	// sample here
	sample = SW_UART_RXIOPIN;
	sample &= (1<<SW_UART_RXPORT);
	bitbuf = sample ? 0xff : 0x00;
	// increase cnt
	cnt = bus_swuart_buf.cnt;
	cnt++;
	bus_swuart_buf.cnt = cnt;

	if(cnt < 9) {
		bus_swuart_buf.buf = 
			(uint8_t) ((uint8_t) (bus_swuart_buf.buf>>1) | 
				   ((uint8_t) bitbuf & 0x80));
		TTPA_OCR += bus_eff_bitlen;
	} else {
		bus_iobuf_t buf;

		buf.error = BUS_IO_OK;
		buf.slotstart = bus_timer_ic;
		buf.par = (uint8_t)bitbuf & 0x01;
		buf.buf = bus_swuart_buf.buf;
		// UART ready -> call op_complete callback
		(*bus_op_complete)(&buf);
	}
}

/*
 * void bus_recvbyte_to(void): Receive operation just timed out; set error
 * code and jump to callback
 */
void bus_recvbyte_to(void)
{
	bus_iobuf_t buf;

	buf.error = BUS_IO_TIMEOUT;
	// enable OC, disable IC interrupt
	TTPA_TIFR = (1<<TTPA_OCIE);
	TTPA_TIMSK = (TTPA_TIMSK & ~(1<<TTPA_TICIE)) | (1<<TTPA_OCIE);
	// UART ready -> call op_complete callback
	(*bus_op_complete)(&buf);
}

/*
 * void bus_sendbyte_init(bus_iobuf_t *param): Initialize SW UART foe sending
 * a byte
 */
void bus_sendbyte_init(bus_iobuf_t *param)
{
	bus_eff_bitlen = ttpa_bitlength>>param->spdup;

	bus_swuart_buf.buf = param->buf;
	bus_swuart_buf.par = param->par;
	bus_swuart_buf.cnt = 0;
	
	// set OC handler
	ttpa_sig_oc = (void (*)(void)) bus_sendbyte;
	// set timer
#ifdef SW_UART_SENDCORR
	TTPA_OCR = param->slotstart + SW_UART_SENDCORR;
#else
	TTPA_OCR = param->slotstart;
#endif /* SW_UART_SENDCORR */

	// enable OC, disable IC interrupt
	TTPA_TIFR = (1<<TTPA_OCIE);
	TTPA_TIMSK = (TTPA_TIMSK & ~(1<<TTPA_TICIE)) | (1<<TTPA_OCIE);

	// set bitbuffer = 0, if OC is used, set bit
	bus_swuart_buf.bitbuf = 0;
#ifdef SW_UART_TXUSEOC
	TTPA_TCCRA = (TTPA_TCCRA & ~(1<<TTPA_COM0)) | (1<<TTPA_COM1);
		// debug
	//		TTPA_TCCRC = (1<<TTPA_FOC);
#endif /* SW_UART_TXUSEOC */
	// set transceiver direction to send
	bus_transcvr_send();
}

/*
 * void bus_sendbyte(void): transmit bit to bus
 */
void bus_sendbyte(void)
{
#ifdef SW_UART_TXUSEOC
	uint8_t tccra;
#endif /* SW_UART_TXUSEOC */
	uint8_t cnt, bitbuf;
	// transmit bit in uart_buf
#ifdef SW_UART_TXUSEPORT
	SW_UART_TXIOPORT = (uint8_t) (((uint8_t)
	        (bus_swuart_buf.bitbuf<<SW_UART_TXPORT)) |
		(~(1<<SW_UART_TXPORT)&SW_UART_TXIOPORT));
#endif /* SW_UART_TXUSEPORT */

	// setup timer for next bit (OCR1 = OCR1 + bitlength)
	TTPA_OCR += bus_eff_bitlen;

	// increase cnt
	cnt = bus_swuart_buf.cnt;
	cnt++;
	bus_swuart_buf.cnt = cnt;

	if(cnt < 9) {
		bus_swuart_buf.bitbuf = bitbuf = bus_swuart_buf.buf & 0x01;
		bus_swuart_buf.buf = bus_swuart_buf.buf>>1;
		// if OC is used, set next bit
#ifdef SW_UART_TXUSEOC
		tccra = TTPA_TCCRA;
		TTPA_TCCRA = ((uint8_t) tccra & 
			~((uint8_t)((1<<TTPA_COM1)|(1<<TTPA_COM0))))|
			((bitbuf==1) ? ((1<<TTPA_COM1)|(1<<TTPA_COM0)) 
			: (1<<TTPA_COM1));
#endif /* SW_UART_TXUSEOC */
	} else if(cnt == 9) {
		bus_swuart_buf.bitbuf = bitbuf = bus_swuart_buf.par;
#ifdef SW_UART_TXUSEOC
		tccra = TTPA_TCCRA;
		TTPA_TCCRA = ((uint8_t) tccra & 
			~((uint8_t)((1<<TTPA_COM1)|(1<<TTPA_COM0))))|
			((bitbuf==1) ? ((1<<TTPA_COM1)|(1<<TTPA_COM0)) 
			: (1<<TTPA_COM1));
#endif /* SW_UART_TXUSEOC */
	} else if(cnt == 10) {
		bus_swuart_buf.bitbuf = 1;
#ifdef SW_UART_TXUSEOC
		TTPA_TCCRA |= (1<<TTPA_COM1)|(1<<TTPA_COM0);
#endif /* SW_UART_TXUSEOC */
	} else {
		bus_iobuf_t buf;
		buf.slotstart = TTPA_OCR + bus_eff_bitlen;
		// set transceiver direction to receive
		bus_transcvr_recv();
		// UART ready -> call op_complete callback
		(*bus_op_complete)(&buf);
	}
}
