/*======================================================================*/
/* Application: driver for the 8-bit timer2                             */
/*                                                                      */
/*                                                                      */
/*    Revision: 1.0                                                     */
/*        Date: 2006/10/21                                              */
/*      Author: Pelesic Igor                                            */
/*              e0006828@stud3.tuwien.ac.at                             */
/*======================================================================*/


#ifndef __TIMER2_H
#define __TIMER2_H

#include <avr/io.h>

#if defined (__AVR_ATmega128__)

//#  define CPU_CLK           14745600

#define TIMER_MASK    TIMSK
#define TIMER2_CON    TCCR2
#define STOP       1
#define ONESHOT    2
#define PERIODIC   3



#else
#  error "Wrong Target!\n"
#endif


#endif  /* ifndef __TIMER2_H */

extern void configure_timer2(uint8_t mode, uint16_t duration);
volatile uint8_t timer2_done;
