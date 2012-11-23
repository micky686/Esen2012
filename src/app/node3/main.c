/** 
 * blinking_leds (N1-3)
 *
 * (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>
 **/
 
#include "main.h"
#include "global.h"
#include "node3.h"

int main(void)
{	
	/* setting up hw-uart0 */
	HWUART0_OPORT |= ( _BV(HWUART0_RXD) | _BV(HWUART0_TXD) );
	HWUART0_DDR |= _BV(HWUART0_TXD);
	SWUART_OPORT |= ( _BV(SWUART_RXD) | _BV(SWUART_TXD) );
	
	LED_DDR |= (1<<LED_GREEN) | (1<<LED_RED);
	LED_PORT_REG |= (1<<LED_RED);
	
	while(TRUE)
	{
		if(HWUART0_IPORT & (1<<HWUART0_RXD))
		{
			LED_PORT_REG |= (1<<LED_RED);
			LED_PORT_REG &= ~(1<<LED_GREEN);
		}
		else
		{
			LED_PORT_REG |= (1<<LED_GREEN);
			LED_PORT_REG &= ~(1<<LED_RED);
		}		
	}


	while(TRUE); /* the day will never come when we remove this redundancy */
	return 1;
}
