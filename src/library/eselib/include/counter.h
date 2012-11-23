/**
 * AVR ATmega128 counters
 * 
 * This currently incomplete macro library is supposed to simplify working
 * with the various available prescaler/wgm/compare modes available on
 * the ATmega128. Furthermore it provides a short "cheet-sheet" alike
 * description what the single modes for the single counters/timers mean.
 * 
 * MatNr: 0326077
 * Date: 30.10.2006
 * @author Bernd Froemel <bf@catechsoftworks.biz>
 **/
#ifndef __COUNTER_H__
#define __COUNTER_H__

#include <avr/io.h>


/**
 * Timer/Counter 0 is a 8-bit counter */
#define COUNTER0						0
/**
 * Timer/Counter 1 is a 16-bit counter */
#define COUNTER1						1
/**
 * Timer/Counter 2 is a 8-bit counter */
#define COUNTER2						2
/**
 * Timer/Counter 3 is a 16-bit counter */
#define COUNTER3						3

/**
 * Counter 1/3 channel A */
#define COUNTER_CHAN_A						A
/**
 * Counter 1/3 channel B */
#define COUNTER_CHAN_B						B
/**
 * Counter 1/3 channel C */
#define COUNTER_CHAN_C						C

/**
 * The following macros define all available prescaler/clock source modes */
 
#define CNT_CS_MODE0(CNT)			(0) /* TC: 0/1/2/3: no clock source */
#define CNT_CS_MODE1(CNT)			(1<<CS##CNT##0) /* TC: 0/1/2/3: 1 */
#define CNT_CS_MODE2(CNT)			(1<<CS##CNT##1) /* TC: 0/1/2/3: 8 */
#define CNT_CS_MODE3(CNT)			((1<<CS##CNT##1)|(1<<CS##CNT##0)) /* T/C 0: 32 T/C 1/2/3: 64 */
#define CNT_CS_MODE4(CNT)			(1<<CS##CNT##2) /* T/C 0: 64 T/C 1/2/3: 256 */
#define CNT_CS_MODE5(CNT)			((1<<CS##CNT##2)|(1<<CS##CNT##0)) /* T/C 0: 128 T/C 1/2/3: 1024 */
#define CNT_CS_MODE6(CNT)			((1<<CS##CNT##2)|(1<<CS##CNT##1)) /* T/C 0: 256 T/C 1/2/3: ext. clock on Tn pin. Falling edge */
#define CNT_CS_MODE7(CNT)			((1<<CS##CNT##2)|(1<<CS##CNT##1)|(1<<CS##CNT##0)) /* T/C 0: 1024 T/C 1/2/3: ext. clock on Tn pin. Rising edge */

#define CNT_CS_MODE_SELECT02(CNT, MODE) \
		TCCR##CNT &= ((uint8_t) ~(CNT_CS_MODE7(CNT))); \
		TCCR##CNT |= (CNT_CS_MODE##MODE(CNT))

#define CNT_CS_MODE_SELECT13(CNT, MODE) \
		TCCR##CNT##B &= ((uint8_t) ~(CNT_CS_MODE7(CNT))); \
		TCCR##CNT##B |= (CNT_CS_MODE##MODE(CNT))

/**
 * The following macros define all available compare modes */

/* TC: 0/2
 * nonPWM:			see phaseCorrectPWM
 * fastPWM:			see phaseCorrectPWM
 * phaseCorrectPWM:	normal, OCn disconnected
 */
/* TC: 1/3
 * nonPWM:			see phaseCorrectPWM
 * fastPWM:			see phaseCorrectPWM
 * phaseCorrectPWM:	normal, OCnA/OCnB/OCnC disconnected
 */
#define CNT_COM_MODE0(CNT)				(0)
/* TC: 0/2
 * nonPWM:			toogle OCn on compare match
 * fastPWM:			reserved
 * phaseCorrectPWM:	reserved
 */
/* TC 1/3
 * nonPWM:			toggle OCnA/OCnB/OCnC on compare match
 * fastPWM:			WGM15: Toggle OCnA on compare match, OCnB/OCnC normal, disc.
 * 					all other WGM modes: normal, OCnA/OCnB/OCnC disconnected
 * phaseCorrectPWM:	WGM9/14 Toogle OCnA on compare match, OCnB/OCnC normal, disc.
 * 					all other WGM modes: normal, OCnA/OCnB/OCnC disconnected
 */ 
#define CNT_COM_MODE1(CNT)			(1<<COM##CNT##0)
/* TC: 0/2
 * nonPWM:			clear OCn on compare match 
 * fastPWM:			clear OCn on compare match - set OCn at TOP 
 * phaseCorrectPWM:	clear OCn on compare match when upcounting
 * 					set OC0 on compare match when downcounting.
 */
/* TC 1/3
 * nonPWM:			clear OCnA/OCnB/OCnC on compare match (output to low)
 * fastPWM:			clear OCnA/OCnB/OCnC on compare match
 * 					set OCnA/OCnB/OCnC at TOP
 * phaseCorrectPWM:	clear OCnA/OCnB/OCnC on compare match when upcounting
 * 					set OCnA/OCnB/OCnC on compare match when downcounting
 */ 
#define CNT_COM_MODE2(CNT)			(1<<COM##CNT##1)
/* TC: 0/2
 * nonPWM:			set OCn on compare match 
 * fastPWM:			set OCn on compare match - clear OCn at TOP 
 * phaseCorrectPWM:	set OCn on compare match when upcounting
 * 					clear OC0 on compare match when downcounting.
 */
/* TC 1/3
 * nonPWM:			set OCnA/OCnB/OCnC on compare match (output to high)
 * fastPWM:			set OCnA/OCnB/OCnC on compare match
 * 					clear OCnA/OCnB/OCnC at TOP
 * phaseCorrectPWM:	set OCnA/OCnB/OCnC on compare match when upcounting
 * 					clear OCnA/OCnB/OCnC on compare match when downcounting
 */ 
#define CNT_COM_MODE3(CNT)			((1<<COM##CNT##1)|(1<<COM##CNT##0))

#define CNT_COM_MODE_SELECT02(CNT, MODE)  \
		TCCR##CNT &= (uint8_t) ~(CNT_COM_MODE3(CNT)); \
		TCCR##CNT |= (CNT_COM_MODE##MODE(CNT))

#define CNT_COM_MODE_SELECT13(CNT, MODE, CHAN)  \
		TCCR##CNT##A &= (uint8_t) ~(CNT_COM_MODE3(CNT##CHAN)); \
		TCCR##CNT##A |= (CNT_COM_MODE##MODE(CNT##CHAN))

/**
 * The following macros define all available wave generator module modes */

/* 8bit: normal mode, 0xff, immediate, MAX */
/* 16bit: normal mode, 0xffff, immediate, MAX */
#define CNT_WGM_MODE0(CNT)			(0)
/* 8bit: PWM, phase correct, 0xff, TOP, BOTTOM */
/* 16bit: PWM, phase correct, 8-bit, 0xff, TOP, BOTTOM */
#define CNT_WGM_MODE1(CNT)			(1<<WGM##CNT##0)
/* 8bit: CTC, OCR, immediate, MAX */
/* 16bit: PWM, phase correct, 9-bit, 0x1ff, TOP, BOTTOM */
#define CNT_WGM_MODE2(CNT)			(1<<WGM##CNT##1)
/* 8bit: Fast PWM, 0xff, TOP, MAX */
/* 16bit: PWM, phase correct, 10-bit, 0x3ff, TOP, BOTTOM */
#define CNT_WGM_MODE3(CNT)			((1<<WGM##CNT##1)|(1<<WGM##CNT##0))
/* 16bit: CTC, OCRnA, immediate, MAX */
#define CNT_WGM_MODE4(CNT)			(1<<WGM##CNT##2)
/* 16bit: Fast PWM, 8-bit, 0xff, TOP, TOP */
#define CNT_WGM_MODE5(CNT)			((1<<WGM##CNT##2)|(1<<WGM##CNT##0))
/* 16bit: Fast PWM, 9-bit, 0x1ff, TOP, TOP */
#define CNT_WGM_MODE6(CNT)			((1<<WGM##CNT##2)|(1<<WGM##CNT##1))
/* 16bit: Fast PWM, 10-bit, 0x3ff, TOP, TOP */
#define CNT_WGM_MODE7(CNT)			((1<<WGM##CNT##2)|(1<<WGM##CNT##1)|(1<<WGM##CNT##0))
/* 16bit: PWM, phase & frequency correct, ICRn, BOTTOM, BOTTOM */
#define CNT_WGM_MODE8(CNT)			(1<<WGM##CNT##3)
/* 16bit: PWM, phase & frequency correct, OCRnA, BOTTOM, BOTTOM */
#define CNT_WGM_MODE9(CNT)			((1<<WGM##CNT##3)|(1<<WGM##CNT##0))
/* 16bit: PWM, phase correct, ICRn, TOP, BOTTOM */
#define CNT_WGM_MODE10(CNT)			((1<<WGM##CNT##3)|(1<<WGM##CNT##1))
/* 16bit: PWM, phase correct, OCRnA, TOP, BOTTOM */
#define CNT_WGM_MODE11(CNT)			((1<<WGM##CNT##3)|(1<<WGM##CNT##1)|(1<<WGM##CNT##0)
/* 16bit: CTC, ICRn, Immediate, MAX */
#define CNT_WGM_MODE12(CNT)			((1<<WGM##CNT##3)|(1<<WGM##CNT##2))
#define CNT_WGM_MODE13(CNT)			((1<<WGM##CNT##3)|(1<<WGM##CNT##2)|(1<<WGM##CNT##0) /* !#reserved#! */
/* 16bit: Fast PWM, ICRn, TOP, TOP */
#define CNT_WGM_MODE14(CNT)			((1<<WGM##CNT##3)|(1<<WGM##CNT##2)|(1<<WGM##CNT##1))
/* 16bit: Fast PWM, OCRnA, TOP, TOP */
#define CNT_WGM_MODE15(CNT)			((1<<WGM##CNT##3)|(1<<WGM##CNT##2)|(1<<WGM##CNT##1)|(1<<WGM##CNT##0))

/**
 * This macro allows easy WGM mode selection for counter 0/2. 
 * See the WGM_MODEXX definitions above.
 **/
#define CNT_WGM_MODE_SELECT02(CNT, MODE) \
		TCCR##CNT &= (uint8_t) ~(CNT_WGM_MODE3(CNT)); /* clear the wgm bits first */ \
		TCCR##CNT |= (CNT_WGM_MODE##MODE(CNT) & CNT_WGM_MODE3(CNT))
/**
 * This macro allows easy WGM mode selection for counter 1/3. 
 * See the WGM_MODEXX definitions above.
 **/
#define CNT_WGM_MODE_SELECT13(CNT, MODE) \
		TCCR##CNT##A &= (uint8_t) ~(0x3); /* clear the wgm bits first */ \
		TCCR##CNT##A |= (CNT_WGM_MODE##MODE(CNT) & 0x3); \
		TCCR##CNT##B &= (uint8_t) ~(0x18); /* clear the wgm bits first */ \
		TCCR##CNT##B |= (CNT_WGM_MODE##MODE(CNT) & 0x18) \

#define CNT_OVERFLOW_IRQ_ENABLE02(CNT) \
		TIMSK |= (1<<OCIE##CNT)

#define CNT_OVERFLOW_IRQ_DISABLE02(CNT) \
		TIMSK &= ((uint8_t) ~(1<<OCIE##CNT))

#define CNT_COMPARE_IRQ_ENABLE02(CNT) \
		TIMSK |= (1<<TOIE##CNT)

#define CNT_COMPARE_IRQ_DISABLE02(CNT) \
		TIMSK &= ((uint8_t) ~(1<<TOIE##CNT))
		
#endif
