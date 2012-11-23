/*==================================\
| ttpasignal  						|
\===================================/

File:      ttpasignal.h
Author:    Gernot Klingler
E-Mail:    gernotk@gmx.at
Version:   0.1
Datum:     08.01.2008
      
****************************************/


#ifndef __TTPASIGNAL_H
#define __TTPASIGNAL_H

#include "ttpa.h"

#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

//========== configurartion begin =============================================

#define MAX_MONITORING_SLOTS 10

//MCU specific configuration
#if defined (__AVR_ATmega128__)

#  define CPU_CLK       	14745600
#  define BAUDRATE_TO_PC	115200
#  define BAUDRATE_MAGIC ((CPU_CLK >> 4) / BAUDRATE_TO_PC - 1)

#  define MY_UART_RECEIVE_SIGNAL	SIG_UART1_RECV

// 19200 8E1, TX enable, RX disable
#  define MY_UART_CONTROL_A			UCSR1A
#  define MY_UART_CONTROL_B			UCSR1B
#  define MY_UART_CONTROL_C			UCSR1C
#  define MY_UART_DATA_REGISTER		UDR1
#  define MY_UART_BAUDRATE_H		UBRR1H
#  define MY_UART_BAUDRATE_L		UBRR1L
#  define MY_UART_CONTROL_A_INIT	0x00
#  define MY_UART_CONTROL_B_INIT	(0x18|_BV(RXCIE))
#  define MY_UART_CONTROL_C_INIT	0x26
#  define MY_UART_BAUDRATE_H_INIT	(BAUDRATE_MAGIC >> 8)
#  define MY_UART_BAUDRATE_L_INIT	(BAUDRATE_MAGIC & 0xFF)
#  define MY_UART_CONTROL_UDRE		UDRE

#  define HWUART1_OPORT	PORTD
#  define HWUART1_IPORT	PIND
#  define HWUART1_DDR   DDRD
#  define HWUART1_RXD	PD2
#  define HWUART1_TXD	PD3

#else
#  error "Wrong Target!\n"
#endif

//========== configurartion end ===============================================



//========== interface to ttpa - do not change ================================
	
//----- public function prototypes -----
void init_ttpasignal(void);
void send_monitored_signals(void);
//--------------------------------------

#define SIGNAL_FN 0x33
#define SIGNAL_SEC ifs_int_0

struct signalfile_struct {
	ifs_int8_t slots[MAX_MONITORING_SLOTS];
};

#define IFS_RE_RECV_SIG(SLOT)  IFS_RE_RECV(SLOT,IFS_ADDR_I(SIGNAL_FN,(((SLOT-1)/4)+1),(SLOT-1)%4),0)
#define IFS_RE_SEND_SIG(SLOT)  IFS_RE_SEND(SLOT,IFS_ADDR_I(SIGNAL_FN,(((SLOT-1)/4)+1),(SLOT-1)%4),0)
//========== interface to ttpa end ============================================
	

#endif
