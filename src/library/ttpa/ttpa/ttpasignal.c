/*==================================\
| ttpasignal  						|
\===================================/

File:      ttpasignal.c
Author:    Gernot Klingler
E-Mail:    gernotk@gmx.at
Version:   0.1
Datum:     08.01.2008
      
****************************************/


#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#include "ttpasignal.h"
#include "linkedlist.h"

#include "ifs_rodl.h"

#define MODE_READ   1
#define MODE_WRITE  2
#define MODE_DELETE 3

struct signalfile_struct IFS_LOC(IO_SEC) signalfile;
IFS_ADDAPPLFILE(SIGNAL_FN, &signalfile, NULL, IFS_FILELEN(struct signalfile_struct), SIGNAL_SEC, 066);
list_t monitor_slot_list;

volatile uint8_t receive_state, bytecount;
volatile uint8_t num_sig, mode, rodl_nr, slot_nr, debug_cnt;

int uart_putchar(char c){

  loop_until_bit_is_set(MY_UART_CONTROL_A, MY_UART_CONTROL_UDRE);
  MY_UART_DATA_REGISTER = c;

  return 0;
}

void monitor_signal_add(uint8_t rodl, uint8_t slot)
{	data_t data;
	
	data.slot = (int)slot;
	data.rodl = (int)rodl;
	
	InsertAtEnd(data, &monitor_slot_list);
}

void monitor_signal_delete(uint8_t rodl, uint8_t slot)
{	item_t *aktitem;
	
	aktitem = monitor_slot_list.start;
	
	while(aktitem)
	{	
		if( ((aktitem->data).slot == slot) && ((aktitem->data).rodl == rodl) ) 
		{
			aktitem = DeleteItem(aktitem, &monitor_slot_list);
		}
		else
		{	aktitem = aktitem->next;
		}
	}
}

SIGNAL(MY_UART_RECEIVE_SIGNAL)
{	
	uint8_t data;// checksum;
	data = MY_UART_DATA_REGISTER;
	//ifs_rodlentry_t *myrodlentry;
	//ifs_rodl_op_t myrodloperation;

	switch(receive_state)
	{
		case 0: // receive_state: receive sync 
			if(data==0x00) //sync
			{	receive_state=1;
			}
		break;
		
		case 1: // receive_state: receive rodl number
			switch ( data & 0xF0 )
			{	
				case 0x00: //receive_state remains 1
				break;
				
				case 0xF0: //(sync complete) received number of rodl
					rodl_nr = data & ~0xF0;
					receive_state=2;
				break;
				
				default:
					receive_state=0;
				break;
			}
		break; 
		
		case 2: // receive state: receive mode (read/write/delete), number of signals 
			num_sig = data & ~0xE0;
			mode = (data & 0xE0) >> 5;
			receive_state = 3;
		break;
		
		case 3: // receive state: receive byte with mode (read, write, delete) and slotnumber
			
			switch(mode)
			{	
				case MODE_READ:
					if( bytecount < num_sig )
					{	slot_nr = data;
						monitor_signal_add(0, slot_nr);
						bytecount++;
					}
					
					if( bytecount == num_sig )
					{	bytecount = 0;
						receive_state = 4;
					}
				break;
				
				case MODE_WRITE:
				
			 		if( bytecount < num_sig )
					{	
						if( bytecount % 2 == 0 ) // received number of slot
						{
							slot_nr = data;				
							// change rodl-operation to SEND
							/*myrodlentry = ifs_get_re_ptr(0, slot_nr-1);
							myrodlentry->op = RODL_OP_SEND;*/
						}
						else // received content of slot
						{	signalfile.slots[slot_nr-1] = data;
						}
						
						bytecount++;
					}
					
					if( bytecount == num_sig )
					{	bytecount=0;
						receive_state=4;
					}
					
				break;
				
				case MODE_DELETE:
					if( bytecount < num_sig )
					{	slot_nr=data;
						monitor_signal_delete(rodl_nr, slot_nr);
						bytecount++;
					}
					
					if( bytecount == num_sig )
					{	bytecount=0;
						receive_state=4;
					}
					
				break;
				
				default: //ERROR
					receive_state=0;
				break;
			}
			
		break;
		
		case 4: //receive checksum, TODO
			/*
			if( mode == MODE_WRITE )
			{	myrodlentry = ifs_get_re_ptr(0, slot_nr-1);
				myrodlentry->op = RODL_OP_RECV;
			}
			*/
			receive_state=0;
		break;
	}
}

void send_monitored_signals()
{	
	int ii;
	item_t *aktitem;
	
	aktitem = monitor_slot_list.start;
	
	uart_putchar(0x00);							//sync
	uart_putchar(aktitem->data.rodl | 0xF0);	//rodlnummer | 0xF0
	
	uart_putchar(monitor_slot_list.itemcount);	//send number of signals
	
	for(ii=0; ii < monitor_slot_list.itemcount; ii++)
	{	
		uart_putchar(aktitem->data.slot);	//slotnumber ii+1
		uart_putchar(signalfile.slots[aktitem->data.slot-1]); 	//data
		aktitem=aktitem->next;
	}

	uart_putchar(0x56); //checksum
}

void init_ttpasignal()
{
	InitList(&monitor_slot_list);

	receive_state=0;
	bytecount=0;
	// config Uart
	MY_UART_CONTROL_A = MY_UART_CONTROL_A_INIT;
	MY_UART_CONTROL_B = MY_UART_CONTROL_B_INIT;
	MY_UART_CONTROL_C = MY_UART_CONTROL_C_INIT;
	MY_UART_BAUDRATE_H = MY_UART_BAUDRATE_H_INIT;
	MY_UART_BAUDRATE_L = MY_UART_BAUDRATE_L_INIT;
	
	// connect stdout & stderr with put-function "uart_putchar"
	(void)fdevopen((void*)uart_putchar,NULL,0);
	
	HWUART1_OPORT |= (_BV(HWUART1_RXD) | _BV(HWUART1_TXD));
	HWUART1_DDR   &= ~_BV(HWUART1_RXD);
	HWUART1_DDR   |=  _BV(HWUART1_TXD);
}
