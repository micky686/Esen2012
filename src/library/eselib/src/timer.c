/**
 * DRIVER: TIMER (0/2)
 * 
 * This module provides a simple but accurate timer. Currently both 8bit 
 * counters (2 and 0) are supported and can be individually enabled/disabled
 * through the macros: TIMER_COUNTER2_ENABLED and TIMER_COUNTER0_ENABLED.
 * 
 * MatNr: 0326077
 * Date: 30.10.2006
 * @author Bernd Froemel <bf@catechsoftworks.biz>
 **/
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/interrupt.h>

#include "counter.h"
#include "timer.h"

/* ====================================================================== */
/* G L O B A L  V A R I A B L E S                                         */
/* ====================================================================== */

#ifdef TIMER_COUNTER2_ENABLED
volatile struct timer_str timer_cnt2;
#endif

#ifdef TIMER_COUNTER0_ENABLED
volatile struct timer_str timer_cnt0;
#endif

static void timer_stop(struct timer_str* t);
static void timer_handle_of_irq(struct timer_str* t);

/* ====================================================================== */
/* I N T E R R U P T S                                                    */
/* ====================================================================== */

#ifdef TIMER_COUNTER0_ENABLED
/**
 * Interrupthandler for counter 2 overfl
 * @see timer_handle_of_irq
 **/
SIGNAL(TIMER0_IRQ)
{
	timer_handle_of_irq((struct timer_str*) &timer_cnt0);
}
#endif

#ifdef TIMER_COUNTER2_ENABLED
/**
 * Interrupthandler for counter 2 overflow
 * @see timer_handle_of_irq
 **/
SIGNAL(TIMER2_IRQ)
{
	timer_handle_of_irq((struct timer_str*) &timer_cnt2);
}
#endif

/* ====================================================================== */
/* L O C A L  F U N C T I O N S                                           */
/* ====================================================================== */

/**
 * Handles timer0/2 overflows.
 * 
 * @param t The timer where we received an overflow interrupt.
 **/
static void timer_handle_of_irq(struct timer_str* t)
{
	uint16_t l_irq_cnt = 0;

	__atomic
	(
		(t->irq_cnt)++;
		l_irq_cnt = t->irq_cnt;
	)
	
	if(l_irq_cnt >= t->irq_wait)
	{
		if(l_irq_cnt == t->irq_wait && t->remaining_ticks > 0)
		{
			/* okay now we need to set the OCR to the final
			   value */
			TIMER2_COMPARE_REG = t->remaining_ticks;
			return;
		}
		else
		{	/* reset the OCR XXX: can be optimized  */
			TIMER2_COMPARE_REG = TIMER02_GRANUALITY_IN_TICKS;
		}

		/* We keep track if the handler is finished in time.
		 * if not, an exclamation mark is sent out .. in hope
	         * to be found by an observative eye ot the programmer... */
		if(t->running_flag)
		{
			printf_P(PSTR("!"));
			__atomic
			(
				t->irq_cnt = 0; /* we missed it this time, try on next slot */
			)
			return;
		}
		
		__atomic
		(
			t->running_flag = 1;
			t->irq_cnt = 0;
		)
		
		__atomic
		(
			t->running_flag = 0;		
	
			switch(t->mode)
			{
				case PERIODIC:
					t->irq_cnt = 0;
					break;
				case ONESHOT: /* intended fall through */
				default:
					timer_stop(t);
					break;
			}
		)
		
		if(t->handler != NULL)
		{ /* only  execute if this is valid */
			t->handler();
		}
		

	}
}

/**
 * Stops the timer by disabling its overflow IRQ and setting the
 * prescaler to no clock source.
 * 
 * @param t The timer where we received an overflow interrupt.
 **/
static void timer_stop(struct timer_str* t)
{
	/* disable overflow irq */
	switch(t->counter)
	{
		case 0:
			CNT_OVERFLOW_IRQ_DISABLE02(0);
			CNT_CS_MODE_SELECT02(0, 0);
			break;
		case 2:
			CNT_OVERFLOW_IRQ_DISABLE02(2);
			CNT_CS_MODE_SELECT02(2, 0);
			break;
		default:
			/* ??? */
			return;
	}
}

/* ====================================================================== */
/* E X P O R T E D   F U N C T I O N S                                    */
/* ====================================================================== */

#ifdef TIMER_COUNTER2_ENABLED
/**
 * Initialize-function for timer2.
 * 
 * @param mode The mode we want to use. See the macros STOP, ONESHOT, PERIODIC.
 * @param duration The duration in ms of the timer. Any value from 1 to 65535.
 * @param handler A function pointer which, if not null, gets called after the
 * timer expires.
 **/
void configure_timer2(uint8_t mode, uint16_t duration, void (*handler)(void))
{
	timer_cnt2.counter = 2;
	timer_cnt2.mode = mode;
	timer_cnt2.handler = handler;
	
	switch(mode)
	{
		case STOP:
		{
			timer_stop((struct timer_str*) &timer_cnt2);
			break;
		}
		case ONESHOT: /* intended fall through */
		case PERIODIC:
		{
			
			TIMER2_COMPARE_REG = TIMER02_GRANUALITY_IN_TICKS;
			
			timer_cnt2.irq_wait = duration / TIMER02_GRANUALITY;
			timer_cnt2.remaining_ticks = (((duration % TIMER02_GRANUALITY) * 100) * 
							TIMER02_GRANUALITY_IN_TICKS)/(TIMER02_GRANUALITY * 100);

			/* enable overflow interrupt on counter2 */
			CNT_OVERFLOW_IRQ_ENABLE02(2);
			
			/* setup timer */
			CNT_CS_MODE_SELECT02(2, 5); /* counter2,  PS: 1024 */
			CNT_WGM_MODE_SELECT02(2, 2); /* counter2, CTC */
			CNT_COM_MODE_SELECT02(2, 0); /* counter2, normal */
			break;
		}
		default:
			return; /* illegal mode, do nothing */
	}
	sei();
}
#endif

#ifdef TIMER_COUNTER0_ENABLED
/**
 * Initialize-function for timer2.
 * 
 * @param mode The mode we want to use. See the macros STOP, ONESHOT, PERIODIC.
 * @param duration The duration in ms of the timer. Any value from 1 to 65535.
 * @param handler A function pointer which, if not null, gets called after the
 * timer expires.
 **/
void configure_timer0(uint8_t mode, uint16_t duration, void (*handler)(void))
{
	timer_cnt0.counter = 0;
	timer_cnt0.mode = mode;
	timer_cnt0.handler = handler;
	
	switch(mode)
	{
		case STOP:
		{
			timer_stop((struct timer_str*) &timer_cnt0);
			break;
		}
		case ONESHOT: /* intended fall through */
		case PERIODIC:
		{
			
			TIMER0_COMPARE_REG = TIMER02_GRANUALITY_IN_TICKS;
			
			timer_cnt0.irq_wait = duration / TIMER02_GRANUALITY;
			timer_cnt0.remaining_ticks = (((duration % TIMER02_GRANUALITY) * 100) * 
							TIMER02_GRANUALITY_IN_TICKS)/(TIMER02_GRANUALITY * 100);

			/* enable overflow interrupt on counter2 */
			CNT_OVERFLOW_IRQ_ENABLE02(0);
			
			/* setup timer */
			CNT_CS_MODE_SELECT02(0, 7); /* counter2,  PS: 1024 */
			CNT_WGM_MODE_SELECT02(0, 2); /* counter2, CTC */
			CNT_COM_MODE_SELECT02(0, 0); /* counter2, normal */
			break;
		}
		default:
			return; /* illegal mode, do nothing */
	}
	sei();
}
#endif
