/**
 * DRIVER: TIMER (0/2)
 * 
 * This module provides a simple but accurate timer.
 * 
 * MatNr: 0326077
 * Date: 30.10.2006
 * @author Bernd Froemel <bf@catechsoftworks.biz>
 **/
#ifndef __TIMER_H__
#define __TIMER_H__

#include <avr/io.h>
#include "global.h"
#include "counter.h"

#define PERIODIC					0
#define ONESHOT						1
#define STOP						2


#define TIMER0_IRQ_MASK_REG				TIMSK
#define TIMER0_IRQ_MASK_VAL				(1<<OCIE0)
#define TIMER0_IRQ						SIG_OUTPUT_COMPARE0
#define TIMER0_COUNT_REG				TCNT0
#define TIMER0_COMPARE_REG				OCR0

#define TIMER2_IRQ_MASK_REG				TIMSK
#define TIMER2_IRQ_MASK_VAL				(1<<OCIE2)
#define TIMER2_IRQ						SIG_OUTPUT_COMPARE2
#define TIMER2_COUNT_REG				TCNT2
#define TIMER2_COMPARE_REG				OCR2

#define TIMER02_SIZE					0xff
#define TIMER02_TICKS_PER_SEC				(CPU_CLK >> 10)
#define TIMER02_TICKS_PER_MS				(CPU_CLK >> 20)
#define TIMER02_TICKS_PER_MS_REST			((CPU_CLK >> 10) - (TIMER2_TICKS_PER_MS * 1000))



//#define TIMER_CFG_16(CNT) /* not implemented */

/**
 * This macro defines our resolution. Actually 5 is a very good value,
 * because the default prescaler and CPU_CLK for this specific target
 * results in an integer (see the calculation macro below).
 * 
 * The granuality doesn't matter anymore - it only helps performance:
 * This timer implementation is very accurate with times divideable by 5.
 * Resolution is always 1ms.
 **/
#define TIMER02_GRANUALITY				(5) /* in ms */

#define TIMER02_GRANUALITY_IN_TICKS			((TIMER02_GRANUALITY * TIMER02_TICKS_PER_SEC)/1000)

#if(TIMER02_GRANUALITY_IN_TICKS > 1<<8)
	#error TIMER02_GRANUALITY TOO HIGH
#endif

struct timer_str
{
	uint8_t		counter;
	uint8_t		mode;
	uint16_t	irq_wait;
	uint16_t 	irq_cnt;
	uint16_t	remaining_ticks;
	uint8_t 	running_flag;
	void		(*handler)(void);
};

void configure_timer2 (uint8_t mode, uint16_t duration, void (*handler)(void));
void configure_timer0 (uint8_t mode, uint16_t duration, void (*handler)(void));

//#define TIMER_COUNTER2_ENABLED
//#define TIMER_COUNTER0_ENABLED

#endif /*TIMER0_H_*/
