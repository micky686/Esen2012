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
 * ttpa/mon_statem.c
 *      state-machine for for UART-monitoring
 *
 * $Id: mon_statem.c,v 1.3 2007-01-18 09:09:01 bernd Exp $
 *
 * Revision History:
 *   2005/12/05 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - first version
 *   2007/01/10 (0.6): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - fixed bug with slave node handling
 *
 */
#include <string.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/io.h>

#include "ttpa.h"
#include "mon_uart.h"
#include "mon_statem.h"
#include "ttpa_sysfiles.h"
#include "ms_request.h"
#include "ms_master.h"
#include "ifs.h"
#include "ifs_bitvec.h"
#include "ms.h"
#include "schedule.h"

mon_buf_t mon_rb[MON_BUFLEN];

mon_buf_t mon_lb;

uint8_t mon_rid[MON_MAXRID];

uint8_t mon_freeind, mon_pendind, mon_curr_rid;

static void (* volatile timeout_cb)(void);

static volatile uint8_t timeout_cnt;

ADD_INITTASK(task_mon_init, mon_init, 7, (1<<TTPA_STATE_UNSYNC));

int mon_init(void)
{

	uint8_t i;
	uint16_t j;
	mon_uart_pkt_t uartpkt;

	mon_uart_init();
	mon_timer_init();
	// init structures
	for(i = 0; i < MON_BUFLEN; i++) {
		mon_rb[i].status = MON_BUF_FREE;
	}
	for(j = 0; j < MON_MAXRID; j++) {
		mon_rid[j] = MON_RID_NOTREADY;
	}
	mon_freeind = 0;
	mon_pendind = 0;
	// receive first msr
	uartpkt.buf = (uint8_t *) &mon_lb.req;
        uartpkt.end = sizeof(mon_req_t)-1;
        uartpkt.cnt = 0;
        uartpkt.flags = MON_PARITY;
        uartpkt.callback = mon_msa_cb;
        mon_uart_recv(&uartpkt);
	return 0;
}

void mon_timer_init(void)
{
	MON_TCNT = 0x00;
	MON_TCCR = MON_TPRESCALE;
	timeout_cnt = 0;
	MON_TIFR = (1<<MON_TOIE);
	MON_TIMSK &= ~(1<<MON_TOIE);
	return;
}

void mon_timer_set(uint8_t timeout, void (*cb)(void))
{
	timeout_cnt = timeout;
	timeout_cb = cb;
	MON_TCNT = 0x00;
	MON_TIFR = (1<<MON_TOIE);
	MON_TIMSK |= (1<<MON_TOIE);
	return;
}

INTERRUPT(MON_SIG_TOVF)
{
	timeout_cnt--;
	if(timeout_cnt == 0) {
		MON_TIMSK &= ~(1<<MON_TOIE);
		timeout_cb();
	}
	return;
}
	
static inline uint8_t mon_checksum(uint8_t fb, uint8_t *buf, uint8_t cnt)
{
	uint8_t i, ret;

	ret = fb;
	for(i = 0; i < cnt; i++) {
		ret ^= buf[i];
	}
	return ret;
}

static inline void mon_sendack(uint8_t *buf, uint8_t len)
{
	mon_uart_pkt_t uartpkt;
	uint8_t check, i;

	check = 0;
	for(i = 0; i < len; i++) {
		check ^= buf[i];
	}
	uartpkt.buf = &check;
	uartpkt.end = 0;
	uartpkt.cnt = 0;
        uartpkt.flags = MON_PARITY;
	uartpkt.callback = NULL;
        mon_uart_send(&uartpkt);
}

static inline void mon_senderr(uint8_t code, uint8_t rid, 
			       mon_uart_pkt_t *pktptr, mon_data_t *buf)
{
	uint8_t i;
	for(i = 0; i < 4; i++) {
		buf->data[i] = 0xff;
	}
	buf->rid = 0xff;
	buf->check = 0xf0 | code;
	pktptr->buf = (uint8_t *) buf;
	pktptr->end = sizeof(mon_data_t)-1;
	pktptr->cnt = 0;
	pktptr->flags = MON_PARITY;
	pktptr->callback = mon_msd_read_cb;
	mon_uart_send(pktptr);
	return;
}

static inline void mon_recvdata(mon_uart_pkt_t *pktptr, mon_data_t *buf)
{
	pktptr->buf = (uint8_t *) buf;
	pktptr->end = sizeof(mon_data_t)-1;
	pktptr->cnt = 0;
	pktptr->flags = MON_PARITY;
	pktptr->callback = mon_msd_write_cb;
	mon_uart_recv(pktptr);
}

static inline void mon_senddata(mon_uart_pkt_t *pktptr, mon_data_t *buf)
{
	pktptr->buf = (uint8_t *) buf;
	pktptr->end = sizeof(mon_data_t)-1;
	pktptr->cnt = 0;
	pktptr->flags = MON_PARITY;
	pktptr->callback = mon_msd_read_cb;
	mon_uart_send(pktptr);
}

static inline void msa_read(void)
{
	mon_uart_pkt_t uartpkt;
	ms_req_t req;
	ms_data_t data;
	uint8_t i, ms_reqerr;

	// MON_OP_READ and OP_MSREAD are different!
	// -> convert to OP_MSREAD
	mon_lb.req.fn_op &= ~(MON_OP_MSK);
	mon_lb.req.fn_op |= OP_MSREAD;
	// check for cluster
	if(mon_lb.req.cluster != ttpa_conffile.ccn) {
		mon_senderr(IFS_ERR_NOCLSTR, mon_lb.req.rid, &uartpkt,
			    &mon_lb.data);
		return;
	} else if(mon_lb.req.ln == ttpa_conffile.cln) {
		
		req.ln = mon_lb.req.ln;
		req.fn_op = mon_lb.req.fn_op;
		req.rec = mon_lb.req.rec;
		ms_reqerr = ms_request(&req, &data);
		for(i = 0; i < 4; i++) {
			mon_lb.data.data[i] = data.data[i];
		}
		mon_lb.data.rid = mon_lb.req.rid;
		if(ms_reqerr == IFS_ERR_SUCC) {
			mon_lb.data.check = mon_checksum(MON_FB_MSD,
			      (uint8_t *) &mon_lb.data, sizeof(mon_data_t)-1);
		} else {
			mon_lb.data.check = data.check;
		}
		mon_senddata(&uartpkt, &mon_lb.data);
		return;
	} else {
		if(mon_rid[mon_lb.req.rid] != MON_RID_NOTREADY) {
			if(mon_rb[mon_rid[mon_lb.req.rid]].status ==
			   MON_BUF_READY) {
				mon_senddata(&uartpkt, 
				        &mon_rb[mon_rid[mon_lb.req.rid]].data);
				mon_rb[mon_rid[mon_lb.req.rid]].status =
					MON_BUF_FREE;
				mon_rid[mon_lb.req.rid] = MON_RID_NOTREADY;
				return;
			} else {
				mon_senderr(IFS_ERR_NOTREADY, 
					mon_lb.req.rid,&uartpkt, &mon_lb.data);
				return;
			}
		} else {
			if(mon_rid[mon_rb[mon_freeind].req.rid]==mon_freeind) {
				mon_rid[mon_rb[mon_freeind].req.rid] = 
					 MON_RID_NOTREADY;
			}
			mon_rb[mon_freeind].status = MON_BUF_PENDING;
			memcpy(&mon_rb[mon_freeind].req, &mon_lb.req, 
			        sizeof(mon_req_t));
			mon_rid[mon_lb.req.rid] = mon_freeind;
			mon_freeind++;
			mon_freeind &= MON_BUFMASK;
			if(mon_freeind == mon_pendind) {
				mon_pendind++;
				mon_pendind &= MON_BUFMASK;
			}
                        ifs_bitvec_setb(ms_datafile.ms_pending, 8, 3);
			mon_senderr(IFS_ERR_NOTREADY, mon_lb.req.rid, &uartpkt,
				&mon_lb.data);
			return;
		}
	}
}

static inline void msa_write(void)
{
	mon_uart_pkt_t uartpkt;

	uartpkt.buf = (uint8_t *) &mon_lb.data;
	uartpkt.end = sizeof(mon_data_t)-1;
	uartpkt.cnt = 0;
	uartpkt.flags = MON_PARITY;
	uartpkt.callback = mon_msd_write_cb;
	mon_uart_recv(&uartpkt);
	return;
}

static inline void msa_exec(void)
{
	mon_uart_pkt_t uartpkt;
	ms_req_t req;
	ms_data_t data;

	mon_lb.req.fn_op &= ~(MON_OP_MSK);
	mon_lb.req.fn_op |= OP_MSEXEC;
	if(mon_lb.req.cluster == ttpa_conffile.ccn) {
		if(mon_lb.req.ln == ttpa_conffile.cln) {
			req.ln = mon_lb.req.ln;
			req.fn_op = mon_lb.req.fn_op;
			req.rec = mon_lb.req.rec;
			ms_request(&req, &data);
		} else {
			if(mon_rid[mon_rb[mon_freeind].req.rid]==mon_freeind) {
				mon_rid[mon_rb[mon_freeind].req.rid] = 
					 MON_RID_NOTREADY;
			}
			mon_rb[mon_freeind].status = MON_BUF_PENDING;
			memcpy(&mon_rb[mon_freeind].req, &mon_lb.req, 
			        sizeof(mon_req_t));
			mon_rid[mon_lb.req.rid] = mon_freeind;
			mon_freeind++;
			mon_freeind &= MON_BUFMASK;
			if(mon_freeind == mon_pendind) {
				mon_pendind++;
				mon_pendind &= MON_BUFMASK;
			}
                        ifs_bitvec_setb(ms_datafile.ms_pending, 8, 3);
		}
	}
	uartpkt.buf = (uint8_t *) &mon_lb.req;
        uartpkt.end = sizeof(mon_req_t)-1;
        uartpkt.cnt = 0;
        uartpkt.flags = MON_PARITY;
        uartpkt.callback = mon_msa_cb;
        mon_uart_recv(&uartpkt);
	return;
}

void mon_timeout_cb(void)
{
	mon_msd_read_cb(NULL); // do the same as on end of MSD read
	return;
}


void mon_msa_cb(mon_uart_pkt_t *pkt)
{
	uint8_t check, op;
	
	// check for errors
	if(pkt->error != UART_STAT_OK) {
		goto error;
	}
	mon_sendack((uint8_t *) &mon_lb.req, sizeof(mon_req_t));
	// check for data corruption
	check = mon_checksum(MON_FB_MSA, (uint8_t *) &mon_lb.req, sizeof(mon_req_t)-1);
	if(check != mon_lb.req.check) {
		goto error;
	}
	// check if READ, WRITE, or EXEC
	op = mon_lb.req.fn_op & MON_OP_MSK;
	if(op == MON_OP_READ) {
		msa_read();
	} else if(op == MON_OP_EXEC) {
		msa_exec();
	} else if(op == MON_OP_WRITE) {
		msa_write();
	} else {
		goto error;
	}
	return;
 error:
	// set timeout
	mon_timer_set(MON_TIMEOUTVAL, mon_timeout_cb);
	return;
}

void mon_msd_read_cb(mon_uart_pkt_t *pkt)
{
	mon_uart_pkt_t uartpkt;

	uartpkt.buf = (uint8_t *) &mon_lb.req;
        uartpkt.end = sizeof(mon_req_t)-1;
        uartpkt.cnt = 0;
        uartpkt.flags = MON_PARITY;
        uartpkt.callback = mon_msa_cb;
        mon_uart_recv(&uartpkt);
	return;
}

void mon_msd_write_cb(mon_uart_pkt_t *pkt)
{
	mon_uart_pkt_t uartpkt;
	uint8_t check;
	ms_req_t req;
	ms_data_t data;	

	if(pkt->error != UART_STAT_OK) {
		goto error;
	}
	// if ok, send ACK
	mon_sendack((uint8_t *) &mon_lb.data, sizeof(mon_data_t));
	// check for data corruption
	check = mon_checksum(MON_FB_MSD, (uint8_t *) &mon_lb.data, 
			     sizeof(mon_data_t)-1);
	if(check != mon_lb.data.check) {
		goto error;
	}
	// MON_OP_WRITE and OP_MSWRITE are different!
	// -> convert to OP_MSWRITE
	mon_lb.req.fn_op &= ~(MON_OP_MSK);
	mon_lb.req.fn_op |= OP_MSWRITE;
	if(mon_lb.req.cluster == ttpa_conffile.ccn) {
		if(mon_lb.req.ln == ttpa_conffile.cln) {
			memcpy(data.data, mon_lb.data.data, 4);
			req.ln = mon_lb.req.ln;
			req.fn_op = mon_lb.req.fn_op;
			req.rec = mon_lb.req.rec;
			ms_request(&req, &data);
		} else {
			if(mon_rid[mon_rb[mon_freeind].req.rid]==mon_freeind) {
				mon_rid[mon_rb[mon_freeind].req.rid] = 
					 MON_RID_NOTREADY;
			}
			mon_rb[mon_freeind].status = MON_BUF_PENDING;
			memcpy(&mon_rb[mon_freeind].req, &mon_lb.req, 
			        sizeof(mon_req_t));
			memcpy(mon_rb[mon_freeind].data.data, mon_lb.data.data, 4);
			mon_rid[mon_lb.req.rid] = mon_freeind;
			mon_freeind++;
			mon_freeind &= MON_BUFMASK;
			if(mon_freeind == mon_pendind) {
				mon_pendind++;
				mon_pendind &= MON_BUFMASK;
			}
                        ifs_bitvec_setb(ms_datafile.ms_pending, 8, 3);
		}
	}
	uartpkt.buf = (uint8_t *) &mon_lb.req;
        uartpkt.end = sizeof(mon_req_t)-1;
        uartpkt.cnt = 0;
        uartpkt.flags = MON_PARITY;
        uartpkt.callback = mon_msa_cb;
        mon_uart_recv(&uartpkt);
	return;
error:
	// set timeout
	mon_timer_set(MON_TIMEOUTVAL, mon_timeout_cb);
	return;
}


// handler functions for handling slave nodes

void ms_setup_gw0(void) // overwrites weak dummy function in ms_master
{
	mon_buf_t *curr;
	uint8_t i;

	curr = &mon_rb[mon_pendind];
	mon_curr_rid = curr->req.rid;
	mon_pendind++;
	mon_pendind &= MON_BUFMASK;
	if(mon_rb[mon_pendind].status != MON_BUF_PENDING) {
		ifs_bitvec_clearb(ms_datafile.ms_pending, 8, 3);
	}
	ms_datafile.msa.ln = curr->req.ln;
	ms_datafile.msa.fn_op = curr->req.fn_op;
	ms_datafile.msa.rec = curr->req.rec;
	if((curr->req.fn_op & OP_MSMSK) == OP_MSWRITE) {
		for(i = 0; i < 4; i++) {
			ms_datafile.msd.data[i] = curr->data.data[i];
		}
		ms_msd_check(&ms_datafile.msd.data);
	}
	return;
}

void ms_callback_gw0(void) // overwrites weak dummy function in ms_master
{
	mon_buf_t *curr;
	uint8_t i;

	if(mon_rid[mon_curr_rid] != MON_RID_NOTREADY) {
		curr = &mon_rb[mon_rid[mon_curr_rid]];
		if((curr->req.fn_op & OP_MSMSK) == OP_MSREAD) {
			if(!ms_msdtimeout) {
				if(mon_checksum(MON_FB_MSD, 
				   (uint8_t *) ms_datafile.msd.data, 4) == 
				   ms_datafile.msd.check) {
					for(i = 0; i < 4; i++) {
						curr->data.data[i] =
						       ms_datafile.msd.data[i];
					}
					curr->data.rid = mon_curr_rid;
					curr->data.check = mon_checksum(
						MON_FB_MSD,
						(uint8_t *) &curr->data, 
						sizeof(mon_data_t)-1);
				} else {
					for(i = 0; i < 4; i++) {
						curr->data.data[i] = 0xff;
					}
					curr->data.rid = 0xff;
					curr->data.check = 
						ms_datafile.msd.check;
				}
			} else {
				for(i = 0; i < 4; i++) {
					curr->data.data[i] = 0xff;
				}
				curr->data.rid = 0xff;
				curr->data.check = 0xf0 | IFS_ERR_NONODE;
			}
		}
		curr->status = MON_BUF_READY;
	}
}

