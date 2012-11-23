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
#ifndef _CLOCK_H__
#define _CLOCK_H__


#define CLK_IRQ_MASK_REG				TIMSK
#define CLK_IRQ_MASK_VAL				(1<<OCIE2)
#define CLK_IRQ							SIG_OUTPUT_COMPARE2
#define CLK_COUNT_REG					TCNT2
#define CLK_COMPARE_REG					OCR2
#define CLK_GRANUALITY					(5)
#define CLK_GRANUALITY_IN_TICKS			(((CPU_CLK >> 10) * CLK_GRANUALITY)/1000)

void clk_init(void);
void clk_reset(void);
uint32_t clk_get_time(void);

#endif
