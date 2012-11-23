/**
 * DRIVER: USART0
 * 
 * This module provides simple USART0 access (receive and transmit).
 * 
 * MatNr: 0326077
 * Date: 30.10.2006
 * @author Bernd Froemel <bf@catechsoftworks.biz>
 **/
#ifndef __UART_H__
#define __UART_H__

#include <avr/io.h>

#define UART0							0
#define UART1							1

#define UART_MODE_8N1					(0) /* b00 */
#define UART_MODE_8E1					((1<<UPM01)) /* b10 */
#define UART_MODE_8O1					((1<<UPM01)|(1<<UPM00)) /* b11 */

#define UART_BAUD_MIN					(2400)
#define UART_BAUD_MAX					(230400)

#define UART0_TRANSMIT_IRQ				SIG_USART0_TRANS
#define UART1_TRANSMIT_IRQ				SIG_USART1_TRANS
#define UART0_RECEIVE_IRQ				SIG_USART0_RECV
#define UART1_RECEIVE_IRQ				SIG_USART1_RECV

#define UART0_IRQ_MASK_REG				UCSR0B
#define UART1_IRQ_MASK_REG				UCSR1B
#define UART_IRQ_RX_MASK_VAL				(1<<RXCIE)
#define UART_IRQ_TX_MASK_VAL				(1<<TXCIE)
#define UART_TRANSMIT_IRQ				SIG_USART0_TRANS
#define UART_RECEIVE_IRQ				SIG_USART0_RECV
#define UART0_CONTROL_A					UCSR0A
#define UART0_CONTROL_B					UCSR0B
#define UART0_CONTROL_C					UCSR0C
#define UART0_DATA_REG					UDR0
#define UART0_BAUDRATE_H				UBRR0H
#define UART0_BAUDRATE_L				UBRR0L
#define UART1_CONTROL_A					UCSR1A
#define UART1_CONTROL_B					UCSR1B
#define UART1_CONTROL_C					UCSR1C
#define UART1_DATA_REG					UDR1
#define UART1_BAUDRATE_H				UBRR1H
#define UART1_BAUDRATE_L				UBRR1L
#define UART0_CONTROL_A_INIT				0x0 /* we enable the uart irq separately */
#define UART0_CONTROL_B_INIT				((1<<TXEN0)|(1<<RXEN0)) /* enable tx/rx, 8-bit mode */
#define UART0_CONTROL_C_INIT				((1<<UCSZ00)|(1<<UCSZ01)) /* 8-Bit mode */
#define UART1_CONTROL_A_INIT				0x0 /* we enable the uart irq separately */
#define UART1_CONTROL_B_INIT				((1<<TXEN1)|(1<<RXEN1)) /* enable tx/rx, 8-bit mode */
#define UART1_CONTROL_C_INIT				((1<<UCSZ10)|(1<<UCSZ11)) /* 8-Bit mode */

#define UART_CONTROL_UDRE				UDRE


/* calculate baudrate register value */
#define UART_BAUDRATE_MAGIC(x)				((CPU_CLK >> 4) / x - 1)

#define UART_TX_SUCCESS					0
#define UART_TX_ERR_NOT_READY				1

#define UART_BUFFER_SIZE_MAX				5

void uart_configure(uint8_t uart_port, uint8_t mode, uint32_t baud, void (*receivefunction)(uint8_t byte));

uint8_t uart_send_byte(uint8_t uart_port, uint8_t byte);
void uart_send_byte_b(uint8_t uart_port, uint8_t byte); /* blocking variant */

/**
 * immediately start flushing the buffer
 **/
void uart_flush(uint8_t uart_port);

void uart_disable(uint8_t uart_port);
void uart_enable(uint8_t uart_port);
void uart_reconfigure(uint8_t uart_port, uint8_t mode, uint32_t baud, void (*receivefunction)(uint8_t byte));

/* protection against my increasing insanity&forgetfullness */
#if UART_BUFFER_SIZE_MAX > 255
	#error UART_BUFFER_SIZE_MAX TOO HIGH!
#endif

#endif

