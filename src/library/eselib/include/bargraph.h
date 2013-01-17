/***************************************************/
/*Modulname: bargraph.h                            */
/*Name: Florian Bartl                              */
/*MNr.: 0425594                                    */
/*Beschreibung:Headerfile des Bargraph-treibers    */
/*Datum:09.12.2011                                 */
/*                            */
/***************************************************/

#ifndef __BARGRAPH_H__
#define __BARGRAPH_H__

#include <avr/io.h>


#if defined(__AVR_ATmega128__)

#define BARGRAPH_DDR                   DDRA
#define BARGRAPH_PORT                  PORTA
#define TIMER_CONTROL_A                TCCR3A
#define TIMER_CONTROL_B                TCCR3B
#define TIMER_INTERRUPT                ETIMSK
#define OUTPUT_COMP_INTERRUPT          OCIE3A
#define TIMER_COMPARE                  OCR3A

#else
#	error "Wrong Target!\n"
#endif

/**********************************************************/
/*oeffentliche Prozeduren:                                */
/* void init_bargraph(void): Initialisiert Bargraph       */
/*void set_bargraph(uint8_t value):Die Bargraph gibt eine */
/*Dezimalzahl in Binaer aus.                              */
/**********************************************************/

void bargraph_init(void);
void set_bargraph(uint8_t value);

#endif
