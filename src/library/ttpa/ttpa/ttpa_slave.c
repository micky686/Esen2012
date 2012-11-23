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
 * ttpa/ttpa_slave.c
 *      Functions specific for TTP/A slave
 *
 * $Id: ttpa_slave.c,v 1.2 2007-01-16 16:52:22 bernd Exp $
 *
 * Revision History:
 *   2004/04/30 (0.1): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     TODO: - ttpa_slave_init: call startup sync function
 */
#include <avr/parity.h>

#include "ttpa.h"
#include "ttpa_fb.h"
#include "ttpa_sysfiles.h"
#include "ttpa_slave.h"
#include "bus.h"
#include "debug.h"

/*
 * void ttpa_eor_slave(void): End of round; wait for next FB
 */
void ttpa_eor_slave(void)
{
	bus_iobuf_t buf;

	buf.spdup = 0;
	// start with receive on BC before slotstart
	ttpa_slotstart += ttpa_slotlength;
	buf.slotstart = ttpa_slotstart - ttpa_bitlength;
	// no timeout
	buf.timeout = 0;
	bus_op_complete = (void (*) (bus_iobuf_t *)) ttpa_recvfb;
	bus_recvbyte_init(&buf);
}

/*
 * void ttpa_recvfb(bus_iobuf_t *busbuf): Just received a FB, check if valid
 */
void ttpa_recvfb(bus_iobuf_t *busbuf)
{
	uint8_t par;
	if(busbuf->error != BUS_IO_OK) {
		ttpa_setstate(TTPA_STATE_UNSYNC);
		return;
	} else {
		par = ~parity_even_bit(busbuf->buf)&0x01;
		if(par == busbuf->par) {
			if(busbuf->buf != ttpa_getfb(busbuf->buf&0x07)) {
				ttpa_setstate(TTPA_STATE_UNSYNC);
				return;
			}				
		} else {
			ttpa_setstate(TTPA_STATE_UNSYNC);
			return;
		}
	}
	ttpa_conffile.crnd = busbuf->buf;
	ttpa_slotstart = busbuf->slotstart;
	ttpa_newround();
}

/*
 * void ttpa_slave_init(void): Slave specific startup code
 */
void ttpa_slave_init(void)
{
	bus_op_complete = (void (*) (bus_iobuf_t *)) ttpa_recvfb;
	bus_sync();
}
