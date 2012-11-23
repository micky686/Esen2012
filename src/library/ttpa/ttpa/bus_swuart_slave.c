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
 * ttpa/bus_swuart_slave.c
 *      SW UART functions specific for slave
 *
 * $Id: bus_swuart_slave.c,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - Only empty functions are defined
 *     TODO: - startup synchronization
 *   2004/04/30 (0.2): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - added startup sync
 *   2004/08/22 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - fixed hanging on startup (bug in handling of timer flags)
 *
 */
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <stdio.h>

#include "node_config.h"
#include "ttpa.h"
#include "ttpa_fb.h"
#include "bus.h"
#include "bus_swuart.h"
#include "bus_swuart_slave.h"
#include "bus_transcvr.h"

/*
 * void bus_sync(void): startup synchronization for slave
 */
void bus_sync(void)
{
	// poll until bus is high
	while(!(SW_UART_RXIOPIN & (1<<SW_UART_RXPORT)));

	// set IC mode to falling edge
	TTPA_TCCRB = (1<<TTPA_CS0);
	// set OC to wait for one slot at maximum baudrate
	TTPA_OCR += SLOTLENGTH_MIN;
	// set OC handler to bus_foundirg
	ttpa_sig_oc = (void (*)(void)) bus_foundirg;
	// set IC handler to bus_sync
	bus_sig_ic = bus_sync;
	// enable OC and IC interrupt
	TTPA_TIFR = (1<<TTPA_OCIE) | (1<<TTPA_TICIE);
	TTPA_TIMSK |= (1<<TTPA_TICIE) | (1<<TTPA_OCIE);
}

/*
 * void bus_foundirg(void): Timeout occured, IRG has been found;
 * wait for next falling edge
 */
void bus_foundirg(void)
{
	// set IC handler to bus_foundstart
	bus_sig_ic = bus_foundstart;
	// disable OC
//	TTPA_TIFR = (1<<TTPA_TICIE);
	TTPA_TIMSK = (TTPA_TIMSK & ~(1<<TTPA_OCIE)) | (1<<TTPA_TICIE);
}

/*
 * void bus_foundstart(void): Falling edge received, start with synchronization
 */
void bus_foundstart(void)
{
	bus_timer_ic = TTPA_ICR;
	// set IC mode to rising edge
	TTPA_TCCRB = (1<<TTPA_CS0) | (1<<TTPA_ICES);
	bus_swuart_buf.cnt = 0;
	// set IC handler to bus_mesuresync
	bus_sig_ic = bus_measuresync;
}

/*
 * void bus_measuresync(void): Measure the BC length
 */
void bus_measuresync(void)
{
	uint8_t cnt;
	uint16_t slotlength, bitlength, new_bitlength;

	slotlength = TTPA_ICR - bus_timer_ic;
	cnt = ++bus_swuart_buf.cnt;
	if(cnt == 1) {
		if((slotlength < BITLENGTH_MIN) || 
		   (slotlength > BITLENGTH_MAX)) {
			bus_sync();
			return;
		}
		bus_eff_bitlen = slotlength;
		// set OC to 1.5 times of measured bitlength
		TTPA_OCR = TTPA_ICR + slotlength + (slotlength>>1);
		// set OC handler to bus_sync
		ttpa_sig_oc = (void (*)(void)) bus_sync;
		// enable OC and IC interrupt
		TTPA_TIFR = (1<<TTPA_OCIE) | (1<<TTPA_TICIE);
		TTPA_TIMSK |= (1<<TTPA_TICIE) | (1<<TTPA_OCIE);
	} else {
		new_bitlength = slotlength - ttpa_slotlength;
		bitlength = bus_eff_bitlen;
		if((new_bitlength < bitlength - BIT_TOLERANCE) ||
		   (new_bitlength > bitlength + BIT_TOLERANCE)) {
			bus_sync();
			return;
		}
		if(cnt == 9) {
			uint16_t slotlength_2;
			bus_iobuf_t buf;
			slotlength = slotlength - bitlength;
			slotlength_2 = slotlength>>1;
			bitlength = slotlength_2>>2;
			slotlength = slotlength + slotlength_2 + bitlength;
			ttpa_slotlength = slotlength;
			ttpa_bitlength = bitlength;
			buf.buf = TTPA_FB_MSA;
			buf.par = 1;
			buf.error = BUS_IO_OK;
			buf.slotstart = bus_timer_ic;
			// sync. ready -> call op_complete callback
			(*bus_op_complete)(&buf);
		} else {
			// set OC
			TTPA_OCR += bitlength;
		}
	}
	// toggle edge
	TTPA_TCCRB = (1<<TTPA_ICES) ^ TTPA_TCCRB;
	ttpa_slotlength = slotlength;
	// clear pending timeout
	TTPA_TIFR = (1<<TTPA_OCIE);
	return;
}
