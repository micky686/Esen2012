/*======================================================================*/
/* Application: Protokoll B                                             */
/*                                                                      */
/*                                                                      */
/*    Revision: 1.0                                                     */
/*        Date: 2006/12/04                                              */
/*      Author: Pelesic Igor                                            */
/*              e0006828@stud3.tuwien.ac.at                             */
/*======================================================================*/

#include "protocoll.h"

uint8_t send_timeout = 0;

volatile uint8_t enable_transmitter = 1;
volatile uint8_t timestamp = 0;
volatile uint8_t msg_header = 0; // used instead of message_length
volatile uint8_t checksum=0;
volatile uint8_t help;
volatile uint8_t length;
volatile uint8_t msg_id = 0;
volatile uint8_t message[15];
volatile uint8_t *msg_pointer;
volatile uint8_t msg_index;
volatile uint8_t node_id=0;
volatile uint8_t send_check=0;
volatile uint8_t timer3_done=0;
volatile uint8_t int_occured=0;
volatile uint8_t init_stat=0;


INTERRUPT(UART_BUS_INTERRUPT_RECV) {

  if (timestamp==0)
  {                           			 //Start byte 
    help=UDR0;
    checksum = help;
    
    length=(help&0x0f);                  //Length of the message 

    help=(help&0xf0);
    msg_id = (help>>4);                  //Message Reciever

    timestamp=length+1;                  //For how many bytes to wait 
                                         // including the checksum
    msg_index=0;                         //new message inidcator

   }
	/**
	 * Only right node can recieve message.In case of false id timestamp will be lowerd.
	 * If node id is equivalent , checksum will be controled. If checksum is right recive message 
	 * function can be called.
	 */
	 /
	else if ((timestamp>1)&&(node_id==msg_id))
	{
	      message[msg_index]=UDR0;
	      checksum^=message[msg_index];
	      msg_index++;
	      timestamp--;
	}
	else if ((timestamp>1)&&(node_id!=msg_id))
	{ 
		help=UDR0;
        timestamp--;
	}
	else if (timestamp==1)
	{ 
		help=UDR0;
	    if (node_id==msg_id)
	    {
             if (checksum==help)
             {         /* checksummen check*/
	           msg_pointer = message;
	           receive_handler(length, (uint8_t *) msg_pointer);
	         }
	    }
	    timestamp--;              /* letztes byte, andere drfen wieder*/
 	}
}

/**
 * Interrupt routine is used for bus control.
 * If interrupt 7 ocures on the pin , it signal that the bus is used from other Node.
 */

SIGNAL(SIG_OUTPUT_COMPARE3A)
{
     disable_int7();
     stop_timer3();
     timer3_done=1;
}

SIGNAL(SIG_INTERRUPT7)
{
  	int_occured=1;
}

/**
 * Simple delay function. 
 */
void wait_ms(uint8_t timeout)
{
	configure_timer2(ONESHOT,timeout);      //delay generated trough timer 2
	while(timer2_done!=1);                 
}

/*
 * Transmiting the data on the bus.
 */ 
uint8_t send_byte(uint8_t byte) {

  	while ( !(UCSR0A&(1<<UDRE0))); 
 		 UDR0=byte;

return byte;

}
/*
 * Initialise timer 3 for control of the bus.
 */
void init_timer3(uint8_t bit_time)
{
      bittime=bit_time;
      timer3_done=0;
      ETIMSK|=(1<<OCIE3A);
      OCR3A=TCNT3+bit_time;
      TCCR3B|=(1<<CS32);
}

/*
 * Disable timer 3
 */
void stop_timer3(void)
{
  ETIMSK&=~(1<<OCIE3A);
  TCCR3B&=~(1<<CS32);

}
/*
 * Enable interupt 7
 */
void enable_int7(void)
{
 int_occured=0;
 EICRB|=(1<<ICS71);          // falling edge of int7
 EIMSK|=(1<<INT7);             
}
/*
 * Disable interrupt 7
 */
void disable_int7(void)
{
 EIMSK&=~(1<<INT7);             
 EICRB&=~(1<<ICS71);
}

/**
 * Function is used for define send priority and to send data on bus.
 * Bus will be listened for changes , if any ocures that means the bus is used by
 * onother node.After that pririty will be checked , if the node have superior 
 * priority it will get bus for transmiting the message.
 */

int8_t send_msg(uint8_t message_header, uint8_t *msg_body)
{
		//variable used for function
		uint8_t anzahl_versuche = 0;					
		int i;
        uint8_t ready = OFF;
        init_state = 0;

	   //waiting for sending rights 
       while (ready==OFF)
	   {  
	   	    anzahl_versuche++;  //it counts every send try 

        	switch(init_state)
    		{

              case 0:  
                    while(!(SW_PIN&(1<<SW_RX)));  // wait until bus is high
                    init_state=1;				 
              case 1:   
                    enable_int7();
                    init_timer3(12);
                    while(timer3_done!=1);		  //wait timer if any bus changes brake else go forward
		            if (int_occured==1)
                    {
                    	init_state=0; break;
                    }
                    else{
                    	init_state=2;
                    	}
              case 2: 						 // when bus is free , none of the other nodes are using it 
          		   SET_BUS_LOW;				 // priority check will be done 
                   wait_ms(send_timeout);	 // if no other node with the higer priority is trying to send
                   SET_BUS_HIGH;			 // node is able to transmit data
                   init_timer3(1);
		           while(timer3_done!=1);   
                   if (SW_PIN&(1<<SW_RX))
		           {
		           		ready=ON;
		           }
		           else
		           { 
		           		init_state=0; }
                   break;

	          default: break;
      }
  }
/*
 * Message is containing of three parts header, body, checksum
 */
              UCSR0B|=(1<<TXEN0);
	          send_byte(message_header);
              send_check=message_header;
	          for(i = 0; i < (msg_header & 0x0F); i++)
              {
		             send_byte(msg_body[i]);
		             send_check ^= msg_body[i];
		      }
              send_byte(send_check);
              UCSR0B&=~(1<<TXEN0);
}

/**
 * Comincation protocol initialisation.For comunication protocol uses hardware UART 
 */ 

void protocol_init(uint8_t timeout, void (*receive_msg)(uint8_t msg_header, uint8_t *msg_body)) {

   uint16_t baudrate;

   //Initialise PORTS
   HW0_PORT|=(1<<HW0_TX)|(1<<HW0_RX);
   HW0_DDR|=(1<<HW0_TX);
   HW0_DDR&=~(1<<HW0_RX);

   SW_PORT|=(1<<SW_TX)|(1<<SW_RX);
   SW_DDR&=~((1<<SW_TX)|(1<<SW_TX));

   HW1_PORT|=(1<<HW1_TX)|(1<<HW1_RX);
   HW1_DDR&=~((1<<HW1_TX)|(1<<HW1_RX);

   //Set UART 
   baudrate=((CPU_CLK >>4)/BAUD)-1;
   UBRR0H=(baudrate>>8);
   UBRR0L= (baudrate & 0xff);
   UCSR0C=MODE_8E1;
   UCSR0B|=(1<<RXCIE0)|(1<<RXEN0);

   timestamp = 0;
   msg_header = 0;
   send_timeout = timeout;
   node_id=timeout;
   receive_handler = receive_msg;

}
