/**======================================================================/
 * Project    : Exercise 2 
 * Driver     : SWUART - Software UART
 * Note       : - specific functionalities added to support csma/ca
 * 			  : - supports only 8N1, adjustable baudrate
 *            : - timer3, all channels are in use.
 * Revision   : 1.0
 * Author     : Ruhul Alam, 0125788 , e0125788@student.tuwien.ac.at
 *            : Bernhard Froemel, 0326077, e0326077@student.tuwien.ac.at
 *            : Gaisbauer David, 0225034 , e0125788@student.tuwien.ac.at
 * 
 * Date       : 2006/10/28 
 *              TU Vienna
 *======================================================================*/
 
/**
 * SWUART DRIVER. Fixed to 8N1.
 * 
 * This is a special variant of an UART-driver to suit our needs for the 
 * CSMA/CA-protocol. E.g. Every transmitted high bit is immediatley read in
 * again to check if someone is crippeling our communication.
 **/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <stdio.h>

#define F_CPU 14745600
#include <delay.h>
#include "global.h"
#include "counter.h"
#include "swuart.h"

#define CPU_CLK_BAUD		(CPU_CLK >> 4) /* just CPU_CLK / 16 that we have
                                            a reasonable low number here */

/* ====================================================================== */
/* G L O B A L  V A R I A B L E S                                         */
/* ====================================================================== */

volatile uint16_t bitlength = 0;
/* toggle variable which indicates if we are in readback or tx mode
 * when we are about to tx a high bit */
volatile uint8_t readback = 0;

/* [E.T.7.6.5.4.3.2.1.0.S] E.. end(->1), T.. stop(->1) S.. start(->0) */
volatile uint16_t frame_in;
volatile uint16_t frame_out;

/* current bits count */
volatile uint8_t frame_in_bits;
/* sampled data in byte */
volatile uint8_t data_in;
/* rx complete flag */
volatile uint8_t rx_complete;
/* rx failed flag */
volatile uint8_t rx_failed;
/* rx failed flag */
volatile uint8_t tx_failed;
/* rx disabled flag */
volatile uint8_t rx_disabled;

/* send timeout in trys */
volatile uint8_t send_timeout;
/* listen timeout in bitlengths */
volatile uint8_t listen_timeout;

volatile uint8_t bus_activity;

/* rx complete callback fp */
void (*swuart_rx_cb)(uint8_t byte);

/* ====================================================================== */
/* I N T E R R U P T S                                                    */
/* ====================================================================== */

SIGNAL(SWUART_IRQ_RX_LINE_IRQ)
{
	bus_activity = 1;
	LED_PORT_REG &= ~_BV(LED_RED);
}

/**
 * Signal handler to increase the listen_timeout
 * 
 * @see swuart_csma_ca_arbitration
 **/
SIGNAL(SWUART_CNT_OC_LST_IRQ)
{
	listen_timeout++;
}

/**
 * Signal handler to sample a uart frame.
 **/
SIGNAL(SWUART_CNT_OC_RX_IRQ)
{
	uint16_t data = frame_in >> 1;
	uint8_t bits = frame_in_bits + 1;
		
	if(SWUART_IPORT & _BV(SWUART_RXD))
	{
			data |= (1<<9);
	}
	
	if(10 == bits) /* we are finished */
	{
		if((data & 1) == 0)
		{
			if(data >=(1<<9))
			{
				data_in = data >> 1;
				rx_complete = 1;
				if(frame_out == 0)
				{
					if(swuart_rx_cb != 0)
					{
						swuart_rx_cb(data_in);
					}
				}
			}
		}
		rx_failed = ! rx_complete;
			
		SWUART_CNT_IRQ_MASK_REG &= ~(_BV(SWUART_CNT_IRQ_OC_RX));
		SWUART_CNT_IRQ_FLAG_REG = _BV(SWUART_CNT_IRQ_IC_FLAG);
		if(!rx_disabled)
		{
			SWUART_CNT_IRQ_MASK_REG |= _BV(SWUART_CNT_IRQ_IC);
		}
	}
	else
	{
		frame_in_bits = bits;
		frame_in = data;
	}
}

/**
 * Signal handler to start a uart-frame receive.
 * 
 * @see swuart_init
 **/
SIGNAL(SWUART_CNT_IC_IRQ)
{
	uint16_t ocr = SWUART_CNT_OCR = bitlength;
	uint16_t tmp = SWUART_CNT_ICR + ocr/2;
	//printf(".");
	if(tmp >= ocr)
	{
		tmp -= ocr;
	}
	
	SWUART_CNT_OCR_RX = tmp;
	
	SWUART_CNT_IRQ_MASK_REG &= ~(_BV(SWUART_CNT_IRQ_IC));	
	SWUART_CNT_IRQ_MASK_REG |= _BV(SWUART_CNT_IRQ_OC_RX);
	SWUART_CNT_IRQ_FLAG_REG = _BV(SWUART_CNT_IRQ_OC_RX_FLAG);

	frame_in = 0;
	frame_in_bits = 0;	
}

/**
 * Signal handler to tx a uart frame in a timely accurate way.
 * 
 * @see swuart_send_byte
 **/
SIGNAL(SWUART_CNT_OC_IRQ)
{
	uint16_t data = frame_out;
	
	/* how does that work?
	 * If we are about to send a high bit, we wait half a bitcell length
	 * and read the bit back in. If we tx a low bit there is no sense in
	 * doing that too and we spare us a few irqs. Because we also loose
	 * ticks if we implement that (even inline) in functions its just spagethi
	 * here
	 */
	if(data & 1)
	{
#ifdef SWUART_CSMA_CA_MODE
		/* wait only half a bitcell */
		SWUART_CNT_OCR = bitlength / 2;
		
		if(readback)
		{
			if((SWUART_IPORT & _BV(SWUART_RXD)) == 0) /* What? failure... */
			{
				tx_failed = 1;
			}
			if(1 == data || tx_failed) /* frame tx complete  */
			{
				SWUART_CNT_IRQ_MASK_REG &= ~(_BV(SWUART_CNT_IRQ_OC));
				if(tx_failed)
				{
					data = 0;
				}
				SWUART_CNT_OCR = bitlength;
			}
			
			frame_out = data >> 1; 
		}
		else 
		{
			SWUART_OPORT |= _BV(SWUART_TXD);	
		}
		
		readback = !readback;
#else
		/* wait a full bitcell */
		SWUART_CNT_OCR = bitlength;
		
		SWUART_OPORT |= (_BV(SWUART_TXD));
		
		if(1 == data || tx_failed) /* frame tx complete */
		{	
			SWUART_CNT_IRQ_MASK_REG &= ~(_BV(SWUART_CNT_IRQ_OC));
			if(tx_failed)
			{
				data = 0;
			}
		}
		
		frame_out = data >> 1; 
#endif		
	}
	else
	{
		/* wait a full bitcell */
		SWUART_CNT_OCR = bitlength;
		
		SWUART_OPORT &= ~(_BV(SWUART_TXD));
		
		if(1 == data || tx_failed) /* frame tx complete */
		{	
			SWUART_CNT_IRQ_MASK_REG &= ~(_BV(SWUART_CNT_IRQ_OC));
			if(tx_failed)
			{
				data = 0;
			}
		}
		
		frame_out = data >> 1; 
	}
		
}

/* ====================================================================== */
/* L O C A L  F U N C T I O N S                                           */
/* ====================================================================== */

/**
 * Disable receiving. A currently frame receive is not aborted.
 **/
static void disable_rx(void)
{
	rx_disabled = 1;
	SWUART_CNT_IRQ_MASK_REG &= ~(_BV(SWUART_CNT_IRQ_IC));
	SWUART_CNT_IRQ_MASK_REG &= ~(_BV(SWUART_CNT_IRQ_OC_RX));
}

/**
 * Enable receiving.
 **/
static void enable_rx(void)
{
	if(swuart_rx_cb != 0)
	{
		SWUART_CNT_IRQ_MASK_REG |= (_BV(SWUART_CNT_IRQ_IC));
		SWUART_CNT_IRQ_FLAG_REG |= _BV(SWUART_CNT_IRQ_IC_FLAG);
	}
	rx_disabled = 0;
}

/* ====================================================================== */
/* E X P O R T E D   F U N C T I O N S                                    */
/* ====================================================================== */

/**
 * Initialize the software uart driver. Take special care if you also want to
 * use the hardware uart driver on the same lines (-> ISO-K Bus) you could
 * shorten the lines and/or drive them against each other.
 * 

 * @param rxcompl Receive complete handler.
 **/
void swuart_init(uint32_t baudrate, void (*rxcompl)(uint8_t byte))
{
	swuart_rx_cb = rxcompl;
	/* this is to show us bus activity */
	LED_DDR |= _BV(LED_RED) | _BV(LED_GREEN);
	LED_PORT_REG |= _BV(LED_RED);
	LED_PORT_REG |= _BV(LED_GREEN);

	__atomic
	(
		frame_out = 0;
		frame_in = 0;
		rx_complete = 0;
		rx_failed = 0;
		rx_disabled = 0;
		
		/* set tx as output */
		SWUART_OPORT |= _BV(SWUART_TXD);
		SWUART_DDR |= _BV(SWUART_TXD);
		
		CNT_WGM_MODE_SELECT13(3, 4); /* we have CTC so use CTC */		
		CNT_COM_MODE_SELECT13(3, 0, C); /* set disconnected mode: 
									working with ocu set/clear is VERY slow!! */
		CNT_CS_MODE_SELECT13(3, 1); /* 1:1 */
		
		SWUART_CNT_CTRL_B_REG |= _BV(ICNC3); /* IC on falling edge and noise filter */

		/* bitlength is (CPU_CLK/BAUD), we don't do -1 here */
		bitlength = (CPU_CLK_BAUD / baudrate) * 16;

		SWUART_CNT_OCR = bitlength;
		SWUART_CNT_OCR_LST = bitlength;
		
		/* set rx as input with pullup */
		SWUART_DDR &= ~(_BV(SWUART_RXD));
		SWUART_OPORT |= _BV(SWUART_RXD);
		
		/* wait until we have a defined state */
		while((SWUART_IPORT & _BV(SWUART_RXD)) == 1);
		
		/* enable ic and clear irqs */
		if(swuart_rx_cb != 0)
		{
			SWUART_CNT_IRQ_MASK_REG |= _BV(SWUART_CNT_IRQ_IC);
		}
		SWUART_CNT_IRQ_FLAG_REG = _BV(SWUART_CNT_IRQ_IC_FLAG)|_BV(SWUART_CNT_IRQ_OC_FLAG);
	)
}

/**
 * Send a byte. The byte is bitwise read back. This function will return after the
 * transfer has been completed successfully or a read-back indicates that someone else
 * is using the bus.
 * 
 * @param byte The byte to transmit.
 * @returns Returns true on success otherwise false.
 **/
uint8_t swuart_send_byte(uint8_t byte)
{	
	__atomic
	(
		disable_rx();
		/* init out frame */
		frame_out = (3<<9) | ((byte)<<1);
		tx_failed = 0;
		SWUART_CNT_OCR = bitlength;
		/* enable OC */
		SWUART_CNT_IRQ_MASK_REG |= _BV(SWUART_CNT_IRQ_OC);
	 	SWUART_CNT_IRQ_FLAG_REG = _BV(SWUART_CNT_IRQ_OC_FLAG); /* no we don't want 
	 															an immediate oc irq */
	)
	
	while(frame_out != 0); /* spinlock until transfer is finished */
	
	__atomic
	(
		enable_rx();
	)
	
	if(tx_failed)
	{
		return 0;
	}
	return 1;
}

void swuart_disable(void)
{
	SWUART_DDR &= ~_BV(SWUART_TXD);
	disable_rx();
}

void swuart_enable(void)
{
	SWUART_DDR |= _BV(SWUART_TXD);
	enable_rx();
}

#ifdef SWUART_CSMA_CA_MODE
/**
 * Do bus arbitration, the csma/ca way.
 * This specific function has nothing to do with a software uart but is found
 * nevertheless here because direct control of hardware is required and an 
 * implementation in csma_ca with appropriate interfaces to the swuart driver 
 * would decrease performance considerably.
 * 
 * @param send_to Send timeout in trys.
 * @param listen_to Listen timeout in bit-cell length. 
 * @param msg_id The message id, which also indicates the message priority. The 
 * higher the message id, the lower the priority (if 2 nodes simultaneously 
 * start to arbitrate the bus, the arbitration only with the lowest msg_id will
 * be successful)
 * @returns Returns the number of trys or -1 on failure.
 **/
int8_t swuart_csmaca_arbitration(uint8_t max_send_timeout, uint8_t max_listen_timeout, uint8_t msg_id)
{
	uint8_t send_timeout = 0;
	LED_PORT_REG &= ~_BV(LED_GREEN);
	/* we need to wait until the bus is free or our sending attempts time out */
	while(send_timeout < max_send_timeout)
	{
		listen_timeout = 0;
		/* activate listen timeout */
		SWUART_CNT_IRQ_MASK_REG |= _BV(SWUART_CNT_IRQ_OC_LST);
		/* activate line IRQ */
		bus_activity = 0;
		SWUART_IRQ_RX_LINE_CFG_REG |= SWUART_IRQ_RX_LINE_CFG_VAL;
		EIFR |= _BV(INTF7);
		
		SWUART_IRQ_RX_LINE_MASK_REG |= _BV(SWUART_IRQ_RX_LINE);
		
		while(listen_timeout < max_listen_timeout); /* spinlock */

		SWUART_IRQ_RX_LINE_MASK_REG &= ~_BV(SWUART_IRQ_RX_LINE);
		
		SWUART_CNT_IRQ_MASK_REG &= ~_BV(SWUART_CNT_IRQ_OC_LST);

		if(!bus_activity)
		{
			LED_PORT_REG |= _BV(LED_RED);

			if(!swuart_send_byte(msg_id))
			{
				send_timeout++;
			}
			else
			{
				LED_PORT_REG |= _BV(LED_GREEN);
				return send_timeout;
			}
		}
		else
		{
			send_timeout++;
		}
	}

	if(send_timeout == max_send_timeout)
	{
		LED_PORT_REG |= _BV(LED_GREEN);
		return -1;
	}
	LED_PORT_REG |= _BV(LED_GREEN);
	return send_timeout;
}
#endif
