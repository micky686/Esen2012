#ifndef NODE_CONFIG_MEGA128_SER_V0_1_H
#define NODE_CONFIG_MEGA128_SER_V0_1_H
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
 * include/node_config_mega_128-ser_0.1.h
 *      Node configuration file for Mega128 serial master board 
 *
 * $Id: node_config_mega128-ser_v0.1.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2005/12/09 (0.5): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - first version, adapted from node_config_mega_128-v2.2.h
 */

// SW configurable options
#define SWCONF_IOPORT PORTD
#define SWCONF_IODDR DDRD
#define SWCONF_SWUART PD6
#define SWCONF_AVCC PD7

#undef node_swconf_swuart
#define node_swconf_swuart() node_swconf_highen(SWCONF_SWUART) 
#undef node_swconf_hwuart
#define node_swconf_hwuart() node_swconf_lowen(SWCONF_SWUART) 
#undef node_swconf_avcc_a
#define node_swconf_avcc_a() node_swconf_lowen(SWCONF_AVCC) 
#undef node_swconf_avcc_d
#define node_swconf_avcc_d() node_swconf_highen(SWCONF_AVCC) 

#if !defined (CLOCKRATE)
#  define CLOCKRATE 14745600
#endif /* CLOCKRATE */

#if !defined (MAXSPDUP)
#  define MAXSPDUP 3
#endif /* MAXSPDUP */

#if defined (SW_UART)
#  if !defined (SW_UART_RXPORT)
#    define SW_UART_RXIOPORT PORTD
#    define SW_UART_RXIODDR DDRD
#    define SW_UART_RXPORT PD4
#    define SW_UART_RXIOPIN PIND
#  endif /* SW_UART_RXPORT */
#  if !defined (BUS_SIG_IC)
#    define BUS_SIG_IC SIG_INPUT_CAPTURE1
#  endif /* BUS_SIG_IC */
#  if !defined (SW_UART_TXUSEOC)
#    define SW_UART_SENDCORR -79
#    if !defined (SW_UART_TXUSEPORT)
#      define SW_UART_TXUSEPORT
#    endif /* SW_UART_TXUSEPORT */
#  endif /* SW_UART_TXUSEOC */
#  define SW_UART_RECVCORR -77
#  if defined (SW_UART_TXUSEPORT)
#    if !defined (SW_UART_TXPORT)
#      define SW_UART_TXIOPORT PORTD
#      define SW_UART_TXIODDR DDRD
#      define SW_UART_TXPORT PD5
#    endif /* SW_UART_TXPORT */
#  endif /* SW_UART_TXUSEPORT */
#  define bus_transcvr_init() bus_transcvr_isok_init()
#  define bus_transcvr_send() bus_transcvr_isok_send()
#  define bus_transcvr_recv() bus_transcvr_isok_recv()
#endif /* UART_SW */

#if !defined(TRANSCVR)
#  define TRANSCVR isok
#endif /* TRANSCVR */

#if !defined (TTPA_SIG_OC)
#  define TTPA_SIG_OC SIG_OUTPUT_COMPARE1A
#  define TTPA_COM0 COM1A0
#  define TTPA_COM1 COM1A1
#  define TTPA_OCR OCR1A
#  define TTPA_OCIE OCIE1A
#  define TTPA_FOC FOC1A
#  if defined (SW_UART_TXUSEOC)
#    if !defined (SW_UART_TXOCPORT)
#      define SW_UART_TXOCIOPORT PORTB
#      define SW_UART_TXOCIODDR DDRB
#      define SW_UART_TXOCPORT PB5
#    endif /* SW_UART_TXOCPORT */
#  endif /* SW_UART_TXUSEOC */
#endif /* TTPA_SIG_OC */

#if !defined (TTPA_TCNT)
#  define TTPA_TCNT TCNT1
#  define TTPA_TCCRA TCCR1A
#  define TTPA_TCCRB TCCR1B
#  define TTPA_TCCRC TCCR1C
#  define TTPA_CS0 CS10
#  define TTPA_ICES ICES1
#  define TTPA_ICR ICR1
#  define TTPA_TIMSK TIMSK
#  define TTPA_TIFR TIFR
#  define TTPA_TICIE TICIE1
#  define TTPA_TOIE TOIE1
#  define TTPA_SIG_TOV SIG_OVERFLOW1
#endif /* TTPA_TCNT */

/* serial monitoring configuration */
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

#define LS_DOUBLE
#define LS_PORT0 PORTD
#define LS_DDR0 DDRD
#define LS_LED0 PD0
#define LS_PORT1 PORTD
#define LS_DDR1 DDRD
#define LS_LED1 PD1


#endif /* NODE_CONFIG_MEGA128_SER_V0_1_H */
