/**======================================================================
 * Project    : Exercise 3
 * Driver     : Clock
 *
 * Note       : A simple time source.
 * Revision   : 1.0
 * Authors    : Ruhul Alam, 0125788, <e0125788@student.tuwien.ac.at>
 *            : Bernhard Froemel, 0326077, <bf@catechsoftworks.biz>
 *            : Gaisbauer David, 0225034 , <e0125788@student.tuwien.ac.at>
 *
 * Date       : 2006/12/20
 *              TU Vienna
 *======================================================================*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#include "global.h"
#include "counter.h"
#include "clock.h"

volatile uint32_t time; /* our clock can currently run about 50 days before overflow */

INTERRUPT(CLK_IRQ)
{
	__atomic
	(
		time++;
	)
}

void clk_init(void)
{
	CLK_COMPARE_REG = CLK_GRANUALITY_IN_TICKS;
	
	CNT_OVERFLOW_IRQ_ENABLE02(2);
			
	/* setup timer */
	CNT_CS_MODE_SELECT02(2, 5); /* counter2,  PS: 1024 */
	CNT_WGM_MODE_SELECT02(2, 2); /* counter2, CTC */
	CNT_COM_MODE_SELECT02(2, 0); /* counter2, normal */
}

void clk_reset(void)
{
	__atomic
	(
		time = 0;
	)
}

uint32_t clk_get_time(void)
{
	uint32_t ret_val;
	__atomic
	(
		ret_val = time;
	)
	return ret_val;
}
