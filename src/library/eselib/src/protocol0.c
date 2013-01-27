/*======================================================================*/
/* Application: Protokoll B                                             */
/*                                                                      */
/*                                                                      */
/*    Revision: 1.0                                                     */
/*        Date: 2006/12/04                                              */
/*      Author: Pelesic Igor                                            */
/*              e0006828@stud3.tuwien.ac.at                             */
/*======================================================================*/

#include "protocol0.h"
#include "global.h"

volatile uint8_t send_timeout = 0;


volatile uint8_t timestamp = 0;
volatile uint8_t msg_header = 0; // used instead of message_length
volatile uint8_t checksum=0;
volatile uint8_t help;
volatile uint8_t length;
volatile uint8_t msg_id = 0;
volatile uint8_t message[PROTCL_MAX_MESSAGE];
volatile uint8_t *msg_pointer;
volatile uint8_t msg_index;
volatile uint8_t node_id=0;
volatile uint8_t send_check=0;
volatile uint8_t timer3_done=0;
volatile uint8_t int_occured=0;
volatile uint8_t init_state=0;


SIGNAL(SIG_UART0_RECV) {

	PORTF &= ~(1 << PF3);
	/*if (PORTF &(1<<PF3)){
		PORTF&=~(1<<PF3);
	} else {
		PORTF|=(1<<PF3);
	}*/

	//

	if ((UCSR0A & (1 << FE0) ) || (UCSR0A & (1 << DOR0)) || (UCSR0A & (1 << FE0))  ) {
		//PORTF |= (1 << PF3);
		help=UDR0;
		return;
	} else {
		//PORTF &= ~(1 << PF3);
	}


	if (timestamp==0)
	{ /* Anfangsbyte?*/
		help=UDR0;
		checksum = help;

		length=(help&0x0f); /* die Länge der Message*/

		help=(help&0xf0);
		msg_id = (help>>4); /* Empfänger der Message*/

		timestamp=length+1; /* Wieviel Bytes zu Erwarten sind*/
		/* inklusive der checksumme*/
		msg_index=0; /*neue nachricht fängt an*/

	}

	/* der richtige targetnode ?? wir empfangen die nachricht*/
	/* correct destination, read next byte*/
	else if ((timestamp>1)&&(node_id==msg_id))
	{

		message[msg_index]=UDR0;
		checksum^=message[msg_index];
		msg_index++;
		timestamp--;

	}

	/* der falsche targetnode?? wir tun nur warten; timestamp verringern*/
	/*  if false destination, wait */
	else if ((timestamp>1)&&(node_id!=msg_id))
	{	help=UDR0;
		timestamp--;

	}

	/*das Checksummenbyte?? wenn am richtigen node berprfen wir die*/
	/* die checksumme, und falls sie bereinstimmt rufen wir die */
	/* recieve function, und wir enablen den transmitter*/
	/* if at end of message, check crc byte */
	else if (timestamp==1)
	{	help=UDR0;

		if (node_id==msg_id)
		{
			if (checksum==help)
			{ /* checksummen check*/
				PORTF|=(1<<PF3);
				msg_pointer = message;
				receive_handler(length, (uint8_t *) msg_pointer);
			}

		}

		timestamp--; /* letztes byte, andere drfen wieder*/
	}
	
	//PORTF|=(1<<PF3);
}





ISR(SIG_OUTPUT_COMPARE3A)
{
 disable_int7();
 stop_timer3();
 timer3_done=1;
}

ISR(SIG_INTERRUPT7)
{
  int_occured=1;
}

ISR(SIG_OUTPUT_COMPARE2) /*TIMER2 overflow interrupt*/
{
	timer2_done=1; /* call the handler*/

	TIMER_MASK&=~(1<<OCIE2);
	TIMER2_CON&=~(1<<CS22);
	TIMER2_CON&=~(1<<CS21);
	TIMER2_CON&=~(1<<CS20);
}

void wait_ms(uint8_t timeout) {

	configure_timer2(ONESHOT, timeout);
	while (timer2_done != 1);
}


uint8_t send_byte(uint8_t byte) {

	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = byte;
	return byte;
}

void init_timer3(uint8_t bit_time)
{    
      timer3_done=0;
      ETIMSK|=(1<<OCIE3A);
      OCR3A=TCNT3+bit_time;
      TCCR3B|=(1<<CS32);
}

void stop_timer3(void)
{
  ETIMSK&=~(1<<OCIE3A);
  TCCR3B&=~(1<<CS32);
}

void enable_int7(void)
{
 int_occured=0;
 EICRB|=(1<<ISC71);             /*falling edge of int7*/
 EIMSK|=(1<<INT7);             /* enable int 7*/
}

void disable_int7(void)
{
 EIMSK&=~(1<<INT7);             /* disable int 7*/
 EICRB&=~(1<<ISC71);
}

/**
   Function: 
   send_msg  
   - Sends data to a node over serial line. 
   - Global interrupt flag needs to be enabled. 
   Parameters: 
   message_header - format 0b rrrr llll
   - rrrr -destination node id
   - llll  body length; 
   msg_body - payload; maximum length is 14;   
 */
int8_t send_msg(uint8_t message_header, uint8_t *msg_body) {
	uint8_t anzahl_versuche = 0;
	int i;
	uint8_t ready = OFF;
	init_state = 0;
	PORTF &= ~(1 << PF2);
	

	while (ready == OFF) {
		anzahl_versuche++;

		switch (init_state) {

		case 0:
			while (!(SW_PIN & (1 << SW_RX)))
				;
			init_state = 1;
			break;

		case 1:
			enable_int7();
			init_timer3(12);
			while (timer3_done != 1)
				;

			if (int_occured == 1) {
				init_state = 0;
				break;
			} else {
				init_state = 2;
			}
		
		case 2:
			SET_BUS0_LOW;
			wait_ms(send_timeout);
			SET_BUS0_HIGH;
			init_timer3(1);
			while (timer3_done != 1)
				;
			if (SW_PIN & (1 << SW_RX)) {
				ready = ON;
			} else {
				init_state = 0;
			}
			break;
	

		default:
			break;

		}
	}
	UCSR0B |= (1 << TXEN0);

	send_byte(message_header);

	send_check = message_header;

	for (i = 0; i < (message_header & 0x0F); i++) {

		send_byte(msg_body[i]);
		send_check ^= msg_body[i];
	}
	send_byte(send_check);
	UCSR0B &= ~(1 << TXEN0);
	init_timer3(2);
	while (timer3_done != 1)
		;
	PORTF |= (1 << PF2);
	return anzahl_versuche;
}

void protocol_init(uint8_t timeout, void(*receive_msg)(uint8_t msg_header,
		uint8_t *msg_body)) {

	uint16_t baudrate;

	HW0_PORT |= (1 << HW0_TX) | (1 << HW0_RX);
	HW0_DDR |= (1 << HW0_TX);
	HW0_DDR &= ~(1 << HW0_RX);

	SW_PORT |= (1 << SW_TX) | (1 << SW_RX);
	SW_DDR &= ~((1 << SW_TX) | (1 << SW_RX));

	PORTF |= (1 << PF3) | (1 << PF2);
	DDRF |= (1 << PF3) | (1 << PF2);

	baudrate = ((CPU_CLK >>4) / BAUD) - 1;
	UBRR0H = (baudrate >> 8);
	UBRR0L = (baudrate & 0xff);
	UCSR0C = MODE_8E1;
	UCSR0B |= (1 << RXCIE0) | (1 << RXEN0);

	timestamp = 0;
	msg_header = 0;
	send_timeout = timeout;
	node_id = timeout;
	receive_handler = receive_msg;

}
