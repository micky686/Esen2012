/**======================================================================/
 * Project    : Exercise 2 
 * Driver     : SWUART - Software UART
 * Note       : specific functionalities added to support csma/ca
 * Revision   : 1.0
 * Author     : Ruhul Alam, 0125788 , e0125788@student.tuwien.ac.at
 *            : Bernhard Froemel, 0326077, e0326077@student.tuwien.ac.at
 *            : Gaisbauer David, 0225034 , e0125788@student.tuwien.ac.at
 * 
 * Date       : 2006/10/28 
 *              TU Vienna
 *======================================================================*/
#ifndef __SWUART_H__
#define __SWUART_H__

/* this macro enables bitwise readback on send and the csma_ca_arbitration
 * function
 */
#define SWUART_CSMA_CA_MODE

#define SWUART_CNT_ICR				ICR3
#define SWUART_CNT_OCR				OCR3A
#define SWUART_CNT_OCR_RX			OCR3B
#define SWUART_CNT_OCR_LST			OCR3C

#define SWUART_CNT_OC_IRQ			SIG_OUTPUT_COMPARE3A
#define SWUART_CNT_OC_RX_IRQ		SIG_OUTPUT_COMPARE3B

#define SWUART_CNT_IC_IRQ			SIG_INPUT_CAPTURE3

#define SWUART_CNT_IRQ_MASK_REG		ETIMSK
#define SWUART_CNT_IRQ_OC			OCIE3A
#define SWUART_CNT_IRQ_OC_RX		OCIE3B
#define SWUART_CNT_IRQ_IC			TICIE3
#define SWUART_CNT_IRQ_IC_FLAG		ICF3
#define SWUART_CNT_IRQ_OC_FLAG		OCF3A
#define SWUART_CNT_IRQ_OC_RX_FLAG	OCF3B
#define SWUART_CNT_IRQ_FLAG_REG		ETIFR
#define SWUART_CNT_CTRL_B_REG		TCCR3B
#define SWUART_CNT_IRQ_OC_LST		OCIE3C
#define SWUART_CNT_OC_LST_IRQ		SIG_OUTPUT_COMPARE3C

#define SWUART_IRQ_RX_LINE_IRQ		SIG_INTERRUPT7
#define SWUART_IRQ_RX_LINE_MASK_REG	EIMSK
#define SWUART_IRQ_RX_LINE			INT7
#define SWUART_IRQ_RX_LINE_CFG_REG	EICRB
#define SWUART_IRQ_RX_LINE_CFG_VAL	(_BV(ISC71))

void swuart_init(uint32_t baudrate, void (*rxcompl)(uint8_t byte));

#ifdef SWUART_CSMA_CA_MODE
int8_t swuart_csmaca_arbitration(uint8_t max_send_to, uint8_t max_listen_to, uint8_t msg_id);
#endif

uint8_t swuart_send_byte(uint8_t byte);

void swuart_disable(void);

void swuart_enable(void);

#endif
