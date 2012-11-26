/**
 * GLOBAL
 * 
 * This header file provides globally (accross all nodes) useful macro 
 * definitions. 
 * 
 * MatNr: 0326077
 * Date: 09.12.2011
 * @author Bernd Froemel <bf@catechsoftworks.biz>
 **/
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <avr/io.h>

/* target specific things */
#define SRAM_SIZE						0x1000 /* 4k SRAM */
#define CPU_CLK							14745600ULL

/* LEDs available on all nodes */
#define LED_RED							PF2
#define LED_GREEN						PF3
#define LED_DDR							DDRF
#define LED_PORT_REG					PORTF

/* buttons available on all nodes */
#define BTN_IRQ							SIG_INTERRUPT4
#define BTN_PIN							PE4


/* UART available on all nodes */
#define HWUART0_OPORT					PORTE
#define HWUART0_IPORT					PINE
#define HWUART0_DDR						DDRE
#define HWUART0_RXD						PE0
#define HWUART0_TXD						PE1

#define HWUART1_OPORT					PORTD
#define HWUART1_IPOR					PIND
#define HWUART1_DDR						DDRD
#define HWUART1_RXD						PD2
#define HWUART1_TXD						PD3

#define SWUART_OPORT					PORTE
#define SWUART_IPORT					PINE
#define SWUART_DDR						DDRE
#define SWUART_RXD						PE7
#define SWUART_TXD						PE1 /* no separate TX line for SW uart ? */

/* generally useful makros */

#define __atomic(block)	\
	__atomic_start(); \
	block \
	__atomic_end();

#define __atomic_end() \
	SREG = glb_sreg_cpy
#define __atomic_start() \
	glb_sreg_cpy = SREG; \
	cli()

/* So... how does that hopefully work? We don't actually try to access the pointer
 * pointing to 0xff, but use it only to calculate the correct offset of the struct
 * member at runtime. Be aware that this can only work for structs smaller than 
 * 0x1ff (maximum of our record limits).
 */
#define STR_POS_F(str, member) \
({ \
		uint8_t __res; \
		str* t = (str*)0x1ff; \
        __res = ((uint16_t)&(t->member)) - (uint16_t)t; \
        __res; \
})

/* by Christian's suggestion the *improved* version */
#define STR_POS(str, member) \
( \
		(uint8_t) &(((str*)NULL)->member) \
)

#define IFS_REC(str_p, member) \
		((STR_POS(str_p, member) + 4)/4)

#define IFS_REC_POS(str_p, member) \
		(STR_POS(str_p, member) % 4)


#ifndef NULL
	#define NULL						((void *) 0)
#endif

#define TRUE						1
#define FALSE						0
#define bool_t						uint8_t

/* ====================================================================== */
/* G L O B A L  V A R I A B L E S                                         */
/* ====================================================================== */

/**
 * Global sreg copy. We use this variable for the __atomic macros.
 **/
uint8_t glb_sreg_cpy;


#endif
