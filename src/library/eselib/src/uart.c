/**
 * DRIVER: USART
 * 
 * This module provides simple non blocking USART0 access (receive and transmit).
 * 
 * MatNr: 0326077
 * Date: 30.10.2006
 * @author Bernd Froemel <bf@catechsoftworks.biz>
 * 
 * Changelog:
 * 2006-11-16: added support for both usarts
 **/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <stdio.h>

#include "global.h"
#include "uart.h"

/**
 * Internal data structure for both UART ports.
 **/
struct uart_port
{
	/**
 	 * Receive handler. Gets called when we receive a byte from the serial line.
	 **/
	void 		(*receive_cb)(uint8_t byte);
	
	/**
 	 * Ring-Buffer implementation directly done into the uart driver.
 	 **/
	uint8_t		buffer[UART_BUFFER_SIZE_MAX];
	uint8_t		buffer_position_read;
	uint8_t		buffer_position_write;
	/**
 	 * Boolean, if true we are currently transmitting.
 	 **/
	uint8_t		uart_tx;
};

/* ====================================================================== */
/* G L O B A L  V A R I A B L E S                                         */
/* ====================================================================== */
volatile struct uart_port port[] = {
	{
		NULL,
	},
	{
		NULL,
	},
};

/* forward declarations */
uint8_t get_buffer(volatile struct uart_port* port);
uint8_t put_buffer(volatile struct uart_port* port, uint8_t byte);
uint8_t get_buffer_size(volatile struct uart_port* port);

/* ====================================================================== */
/* I N T E R R U P T S                                                    */
/* ====================================================================== */

#ifndef UARTDRV_UART0_ISR_DISABLED
/**
 * Transmit-complete interrupt handler. If the buffer isn't empty, let's
 * do the next byte which will gives us another transmit-complete interrupt
 * until the buffer is empty.
 **/
SIGNAL(UART0_TRANSMIT_IRQ)
{
	if(get_buffer_size(&port[UART0]) > 0)
	{
		uart_send_byte_b(UART0, get_buffer(&port[UART0]));	
	}
	else
	{
		port[UART0].uart_tx = FALSE;
	}
}

/**
 * Receive-complete interrupt handler. If we aren't listening to our own
 * nonsense, call the receive handler.
 **/
SIGNAL(UART0_RECEIVE_IRQ)
{
	/* save the sreg, in case someone wants to fuck around with it */
	//uint8_t sreg = SREG;
	
	if(! port[UART0].uart_tx)
	{
		port[UART0].receive_cb(UART0_DATA_REG);
	}
	
	//SREG = sreg;
}
#endif

#ifndef UARTDRV_UART1_ISR_DISABLED
/**
 * Transmit-complete interrupt handler. If the buffer isn't empty, let's
 * do the next byte which will gives us another transmit-complete interrupt
 * until the buffer is empty.
 **/
SIGNAL(UART1_TRANSMIT_IRQ)
{
	if(get_buffer_size(&port[UART1]) > 0)
	{
		uart_send_byte_b(UART1, get_buffer(&port[UART1]));	
	}
	else
	{
		port[UART1].uart_tx = FALSE;
	}
}

/**
 * Receive-complete interrupt handler. If we aren't listening to our own
 * nonsense, call the receive handler.
 **/
SIGNAL(UART1_RECEIVE_IRQ)
{
	/* save the sreg, in case someone wants to fuck around with it */
	uint8_t sreg = SREG;
	
	if(! port[UART1].uart_tx)
	{
		port[UART1].receive_cb(UART1_DATA_REG);
	}
	
	SREG = sreg;
}
#endif

/* ====================================================================== */
/* L O C A L  F U N C T I O N S                                           */
/* ====================================================================== */

/**
 * Ringbuffer put function.
 * 
 * Note: Those buffer functions could be generalized and moved to a 
 * separate module
 * 
 * @param byte The byte which should be put into the buffer.
 * @returns Returns true on success otherwise false.
 */
uint8_t put_buffer(volatile struct uart_port* port, uint8_t byte)
{
	__atomic
	(
		if(port->buffer_position_read == (port->buffer_position_write+1) % UART_BUFFER_SIZE_MAX)
		{
			return FALSE;
		}
		
		port->buffer_position_write = (port->buffer_position_write + 1) % UART_BUFFER_SIZE_MAX;
		port->buffer[port->buffer_position_write] = byte;
	)
	return TRUE;
}

/**
 * Ringbuffer size function.
 * 
 * Note: Those buffer functions could be generalized and moved to a 
 * separate module
 * 
 * @returns Returns the current size of the ring-buffer.
 */
uint8_t get_buffer_size(volatile struct uart_port* port)
{
	uint8_t retval = 0;
	
	__atomic
	(
		if (port->buffer_position_write > port->buffer_position_read)
		{
			retval = port->buffer_position_write - port->buffer_position_read;
		}
		else if (port->buffer_position_write < port->buffer_position_read)
		{
			retval = (UART_BUFFER_SIZE_MAX - port->buffer_position_read) + port->buffer_position_write;
		}
	)
	
	return retval;
}

/**
 * Ringbuffer get function.
 * 
 * Note: Those buffer functions could be generalized and moved to a 
 * separate module
 * 
 * @returns Returns the current size of the ring-buffer.
 */
uint8_t get_buffer(volatile struct uart_port* port)
{
	uint8_t retval;
	__atomic(
		if(port->buffer_position_write == port->buffer_position_read)
		{
			__atomic_end();
			return '*'; /* why an asterisk? Well we see immediately if we read 
			more than we are supposed to read on the console */
		}
		port->buffer_position_read = (port->buffer_position_read + 1) % UART_BUFFER_SIZE_MAX;
		retval = port->buffer[port->buffer_position_read];
	)
	return retval;
}


/* nice to have if we want to connect stdout/stderr to the uart * /
int uart_putchar(char c)
{
	if(c == '\n')
	{
		uart_putchar('\r');
	}
	loop_until_bit_is_set(UART_CONTROL_A, UART_CONTROL_UDRE);
	UART_DATA_REG = c;

	return 0;
} */

/* ====================================================================== */
/* E X P O R T E D   F U N C T I O N S                                    */
/* ====================================================================== */

void uart_send_byte_b(uint8_t uart_port, uint8_t byte)
{
	port[uart_port].uart_tx = TRUE;
	/* while( (!( UART_CONTROL_A & (1<<UART_CONTROL_UDRE1)) ) */
	switch(uart_port)
	{
		case UART0:

			UART0_DATA_REG = byte;
			loop_until_bit_is_set(UART0_CONTROL_A, UDRE);
			break;
		case UART1:
			
			UART1_DATA_REG = byte;
			loop_until_bit_is_set(UART1_CONTROL_A, UDRE);
			break;
		default:
		/* do nothing */
			break;
	}
}

/**
 * Reconfigure USART.
 * 
 * @param mode See mode macros in the header file.
 * @param baud The baud rate in Bauds per second
 * @param receivefunction Receivefunction function pointer which gets called
 * when a receive complete interrupt is received.
 **/
void uart_reconfigure(uint8_t uart_port, uint8_t mode, uint32_t baud, void (*receivefunction)(uint8_t byte))
{
	uint32_t rb = baud;
	port[uart_port].receive_cb = receivefunction;
	
	__atomic
	(
		
		port[uart_port].buffer_position_read = 0;
		port[uart_port].buffer_position_write = 0;
		
		/* not any baudrate is useful, so let's restrict that a bit.
		 * Of course we still allow unusal baud rates. */
		if(baud > UART_BAUD_MAX)
		{
			rb = UART_BAUD_MAX;
		}
		if(baud < UART_BAUD_MIN)
		{
			rb = UART_BAUD_MIN;
		}
		
		switch(uart_port)
		{
			case UART0:
			{
				UART0_CONTROL_A = UART0_CONTROL_A_INIT;
				UART0_CONTROL_B = UART0_CONTROL_B_INIT;
				UART0_CONTROL_C = UART0_CONTROL_C_INIT|mode;
				UART0_BAUDRATE_H = (UART_BAUDRATE_MAGIC(rb) >> 8) & 0xFF;
				UART0_BAUDRATE_L = UART_BAUDRATE_MAGIC(rb) & 0xFF;
			}
			case UART1:
			{
				UART1_CONTROL_A = UART1_CONTROL_A_INIT;
				UART1_CONTROL_B = UART1_CONTROL_B_INIT;
				UART1_CONTROL_C = UART1_CONTROL_C_INIT|mode;
				UART1_BAUDRATE_H = (UART_BAUDRATE_MAGIC(rb) >> 8) & 0xFF;
				UART1_BAUDRATE_L = UART_BAUDRATE_MAGIC(rb) & 0xFF;
				break;
			}
			default:
			{ /* do nothing */
			}
		}
	)
}


/**
 * Initialize USART.
 * 
 * @param mode See mode macros in the header file.
 * @param baud The baud rate in Bauds per second
 * @param receivefunction Receivefunction function pointer which gets called
 * when a receive complete interrupt is received.
 **/
void uart_configure(uint8_t uart_port, uint8_t mode, uint32_t baud, void (*receivefunction)(uint8_t byte))
{
	/* reconfigure */
	uart_reconfigure(uart_port, mode, baud, receivefunction);
	
	/* enable irqs */
	switch(uart_port)
	{
		case UART0:
		{
			/* enable receive complete interrupt */
			//UART0_IRQ_MASK_REG |= UART_IRQ_RX_MASK_VAL;
			/* enable transmit complete interrupt */
			//UART0_IRQ_MASK_REG |= UART_IRQ_TX_MASK_VAL;	
			break;
		}
		case UART1:
		{
			/* enable receive complete interrupt */
			//UART1_IRQ_MASK_REG |= UART_IRQ_RX_MASK_VAL;
			/* enable transmit complete interrupt */
			UART1_IRQ_MASK_REG |= UART_IRQ_TX_MASK_VAL;
			break;
		}
		default:
		{ /* do nothing */
		}
	}
	
	sei();
}

/**
 * Starts flushing the buffer.
 **/
void uart_flush(uint8_t uart_port)
{
	uart_send_byte_b(uart_port, get_buffer(&port[uart_port]));
}

void uart_disable(uint8_t uart_port)
{
	switch(uart_port)
	{
		case UART0:
			UART0_CONTROL_B &= ~(_BV(TXEN0)|_BV(RXEN0));
			HWUART0_DDR &= ~(_BV(HWUART0_TXD));
			break;
		case UART1:
			UART1_CONTROL_B &= ~(_BV(TXEN1)|_BV(RXEN1));
			HWUART1_OPORT &= ~(_BV(HWUART1_TXD));
			break;
		default:
			break;
	}
}

void uart_enable(uint8_t uart_port)
{
	switch(uart_port)
	{
		case UART0:			
			HWUART0_DDR |= (_BV(HWUART0_TXD));
			UART0_CONTROL_B |= (_BV(TXEN0)|_BV(RXEN1));
			/* enable receive complete interrupt */
			UART0_IRQ_MASK_REG |= UART_IRQ_RX_MASK_VAL;
			/* enable transmit complete interrupt */
			UART0_IRQ_MASK_REG |= UART_IRQ_TX_MASK_VAL;	
			break;
		case UART1:
			HWUART1_DDR |= (_BV(HWUART1_TXD));
			UART1_CONTROL_B |= (_BV(TXEN1)|_BV(RXEN1));
			/* enable receive complete interrupt */
			UART1_IRQ_MASK_REG |= UART_IRQ_RX_MASK_VAL;
			/* enable transmit complete interrupt */
			UART1_IRQ_MASK_REG |= UART_IRQ_TX_MASK_VAL;	
			break;
		default:
			break;
	}
}

/**
 * Send a byte. Actually the byte is added to the buffer first.
 * 
 * @param byte The byte which is going to be transmitted.
 **/
uint8_t uart_send_byte(uint8_t uart_port, uint8_t byte)
{	
	if(get_buffer_size(&port[uart_port]) == UART_BUFFER_SIZE_MAX-1) /* start flushing if we are full */
	{
		if(port[uart_port].uart_tx == TRUE)
		{
			return UART_TX_ERR_NOT_READY;
		}
		else
		{
			uart_flush(uart_port); /* that *will* block us, until ONE byte is ready */
		}
	}
	put_buffer(&port[uart_port], byte);
	
	if(byte == '\n')
	{
		uart_send_byte_b(uart_port, get_buffer(&port[uart_port]));
	}	
	return UART_TX_SUCCESS;
}
