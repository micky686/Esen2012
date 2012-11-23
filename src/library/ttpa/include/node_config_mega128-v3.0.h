#ifndef NODE_CONFIG_MEGA128_V3_0_H
#define NODE_CONFIG_MEGA128_V3_0_H
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
 * include/node_config_mega128-v3.0.h
 *      Node configuration file for board Mega128 V3.0
 *
 * $Id: node_config_mega128-v3.0.h,v 1.3 2007-01-18 19:18:47 bernd Exp $
 *
 * Revision History:
 *   2005/05/04: T. Mozgan <thomas.m@gmx.at>
 *     - based on file node_config_mega128-v2.2.h
 *     - pin assignment changed to Port E, swuart timer3c, second LED
 *   2005/07/29 (0.4): C. Trödhandl <chtr@vmars.tuwien.ac.at>
 *     - Transceiver definition
 */

// SW configurable options
#define SWCONF_IOPORT PORTE
#define SWCONF_IODDR DDRE
#define SWCONF_SWUART PE4   // CTL_SWHW in schematic
#define SWCONF_AVCC PE3     // CTL_AD in schematic
#define SWCONF_TRANSCVR PE6

#define TRANSCVR_IOPORT PORTE
#define TRANSCVR_IODDR DDRE
#define TRANSCVR_PORT PE2

#define SW_UART_USEOC

#if !defined(TRANSCVR)
#define TRANSCVR isok
#endif /* TRANSCVR */

#undef node_swconf_transcvr_isok
#define node_swconf_transcvr_isok() node_swconf_highen(SWCONF_TRANSCVR)
#undef node_swconf_transcvr_rs485
#define node_swconf_transcvr_rs485() node_swconf_lowen(SWCONF_TRANSCVR)
#undef node_swconf_swuart
#define node_swconf_swuart() node_swconf_highen(SWCONF_SWUART)
#undef node_swconf_hwuart
#define node_swconf_hwuart() node_swconf_lowen(SWCONF_SWUART)
#undef node_swconf_avcc_a
#define node_swconf_avcc_a() node_swconf_lowen(SWCONF_AVCC)
#undef node_swconf_avcc_d
#define node_swconf_avcc_d() node_swconf_highen(SWCONF_AVCC)

#define bus_transcvr_init() bus_transcvr_sw_init(TRANSCVR)
#define bus_transcvr_send() bus_transcvr_sw_send(TRANSCVR)
#define bus_transcvr_recv() bus_transcvr_sw_recv(TRANSCVR)


#if !defined (CLOCKRATE)
#  define CLOCKRATE 14745600
#endif /* CLOCKRATE */

#if !defined (MAXSPDUP)
#  define MAXSPDUP 3
#endif /* MAXSPDUP */

#if defined (SW_UART)
#  if !defined (SW_UART_RXPORT)
#    define SW_UART_RXIOPORT PORTE
#    define SW_UART_RXIODDR DDRE
#    define SW_UART_RXPORT PE7         // IC3 in schematic
#    define SW_UART_RXIOPIN PINE
#  endif /* SW_UART_RXPORT */
#  if !defined (BUS_SIG_IC)
#    define BUS_SIG_IC SIG_INPUT_CAPTURE3
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
#      define SW_UART_TXIOPORT PORTE
#      define SW_UART_TXIODDR DDRE
#      define SW_UART_TXPORT PE5       // OC3C in schematic
#    endif /* SW_UART_TXPORT */
#  endif /* SW_UART_TXUSEPORT */
#endif /* SW_UART */

#if !defined (TTPA_SIG_OC)
#  define TTPA_SIG_OC SIG_OUTPUT_COMPARE3C
#  define TTPA_COM0 COM3C0
#  define TTPA_COM1 COM3C1
#  define TTPA_OCR OCR3C
#  define TTPA_OCIE OCIE3C
#  define TTPA_FOC FOC3C
#  if defined (SW_UART_TXUSEOC)
#    if !defined (SW_UART_TXOCPORT)
#      define SW_UART_TXOCIOPORT PORTE
#      define SW_UART_TXOCIODDR DDRE
#      define SW_UART_TXOCPORT PE5     // OC3C in schematic
#    endif /* SW_UART_TXOCPORT */
#  endif /* SW_UART_TXUSEOC */
#endif /* TTPA_SIG_OC */

#if !defined (TTPA_TCNT)
#  define TTPA_TCNT TCNT3
#  define TTPA_TCCRA TCCR3A
#  define TTPA_TCCRB TCCR3B
#  define TTPA_TCCRC TCCR3C
#  define TTPA_CS0 CS30
#  define TTPA_ICES ICES3
#  define TTPA_ICR ICR3
#  define TTPA_TIMSK ETIMSK
#  define TTPA_TIFR ETIFR
#  define TTPA_TICIE TICIE3
#  define TTPA_TOIE TOIE3
#  define TTPA_SIG_TOV SIG_OVERFLOW3
#endif /* TTPA_TCNT */

#define LS_DOUBLE
#define LS_INV
#define LS_PORT0 PORTG
#define LS_DDR0 DDRG
#define LS_LED0 PG3     // green LED (D01 on schematic)
#define LS_PORT1 PORTG
#define LS_DDR1 DDRG
#define LS_LED1 PG4     // red LED (D02 on schematic)

#endif /* NODE_CONFIG_MEGA128_V3_0H */
