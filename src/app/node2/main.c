/** 
 * blinking_leds (N0)
 *
 * (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>
 **/
#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#define F_CPU CPU_CLK
#include <util/delay.h>

#include "main.h"

#include "uart.h"

/**
 * Connect stdout/stderr to USAT. Recommended function for connecting 
 * stdout/stderr to the USART.
 * 
 * @param c Byte to be sent over UART
 */
static int put_char(const char c, FILE* ign)
{
	if(c == '\n')
	{
		uart_send_byte_b(UART1, '\r');
	}
	uart_send_byte_b(UART1, c);

	return 0;
}

int main(void)
{
  uart_configure(UART1, UART_MODE_8N1, 9600UL, NULL);
  (void)fdevopen(put_char, NULL);
  
	/* setting up hw-uart0 */
	HWUART0_OPORT |= ( _BV(HWUART0_RXD) | _BV(HWUART0_TXD) );
	HWUART0_DDR |= _BV(HWUART0_TXD);
	SWUART_OPORT |= ( _BV(SWUART_RXD) | _BV(SWUART_TXD) );
	LED_DDR |= (1<<LED_GREEN) | (1<<LED_RED);
	LED_PORT_REG |= (1<<LED_GREEN);
	
	while(TRUE)
	{
		uint8_t delay;
		for(delay=0; delay < 5; ++delay)
		{
			_delay_ms(100);
		}
    printf("Hello world!\n");
		HWUART0_OPORT ^= (1<<HWUART0_TXD);
		LED_PORT_REG ^= (1<<LED_RED) | (1<<LED_GREEN);		
	}


	while(TRUE); /* the day will never come when we remove this redundancy */
	return 1;
}
