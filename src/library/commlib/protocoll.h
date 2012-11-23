/*======================================================================*/
/* Application: Header file for Protocoll B                             */
/*                                                                      */
/*        File: protocoll.h                                             */
/*    Revision: 1.0                                                     */
/*        Date: 2006/10/21                                              */
/*      Author: Pelesic Igor                                            */
/*              e0006828@stud3.tuwien.ac.at                             */
/*======================================================================*/


#ifndef __PROTOCOLL_H__
#define __PROTOCOLL_H__

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#define F_CPU 14745600
#include <avr/delay.h>

#include "timer2.h"

#if defined (__AVR_ATmega128__)

#  define CPU_CLK           14745600



#  define HW0_PORT             PORTE
#  define HW0_PIN              PINE
#  define HW0_DDR              DDRE
#  define HW0_RX               PE0
#  define HW0_TX               PE1

#  define HW1_PORT             PORTD
#  define HW1_PIN              PIND
#  define HW1_DDR              DDRD
#  define HW1_RX               PD2
#  define HW1_TX               PD3

#  define SW_PORT              PORTE
#  define SW_PIN               PINE
#  define SW_DDR               DDRE
#  define SW_RX                PE7
#  define SW_TX                PE5

#define SET_BUS_HIGH (HW0_PORT |= (1<<HW0_TX))
#define SET_BUS_LOW  (HW0_PORT &= ~(1<<HW0_TX))


# define BAUD                57600
# define MODE_8E1            0x26       /*8 bits, even parity, 1 stop bit*/



#define ON        1
#define OFF       0


extern void protocol_init(uint8_t timeout, void (*receive_msg)(uint8_t msg_header, uint8_t *msg_body));
extern int8_t send_msg(uint8_t message_header, uint8_t *msg_body);








#else
#  error "Wrong Target!\n"
#endif

#endif  /* ifndef __PROTOCOLL__ */


extern void protocol_init(uint8_t timeout, void (*receive_msg)(uint8_t msg_header, uint8_t *msg_body));
extern int8_t send_msg(uint8_t msg_length, uint8_t *msg_body);
void (*receive_handler)(uint8_t msg_length, uint8_t *msg_body); 
