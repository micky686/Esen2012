/** 
 * blinking_leds (N1-3)
 *
 * (c) 2007 by Bernhard Froemel <bf@catechsoftworks.biz>
 **/
 
#include "main.h"
#include "global.h"
#include "node1.h"
#include "protocoll.h"

void rcv_handler(uint8_t msg_length, uint8_t *msg_body);

void init_bargraph();
void wr_to_bargraph(uint8_t val);

int main(void) {

  DDRF |= (1 << PF3);
  PORTF &= ~(1 << PF3);
  DDRF |= (1 << PF2);
  PORTF &= ~(1 << PF2);  

  init_bargraph();

  protocol_init(7,rcv_handler);
  sei();

  while(TRUE);
  return 1;
}

int main2(void)
{
	VENT_OFF();

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

void rcv_handler(uint8_t msg_length, uint8_t *msg_body) {
  if (msg_length > 0) {
    PORTF ^= (1 << LED_GREEN);
    wr_to_bargraph(msg_body[0]);
  } else {
    PORTF ^= (1 << LED_RED);
  }
} 


void init_bargraph() {
  LEDS_DDR_ND0 = 0xff; 
  LEDS_PORT_ND0 = 0xff;
}

void wr_to_bargraph(uint8_t val) {
  LEDS_PORT_ND0 = ~(val);
}



