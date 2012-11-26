/*****************************************************/
/*Modulname: pushbutton.h                            */
/*Name: Florian Bartl                                */
/*MNr.: 0425594                                      */
/*Beschreibung:Headerfile fuer die Pushbutton */
/*Datum:12.12.2011                                   */
/*                                  */
/*****************************************************/

#ifndef __pushbutton_h__
#define __pushbutton_h__

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>


#if defined (__AVR_ATmega128__)

#define PUSHPORT 	PINE
#define EXTINT0		SIG_INTERRUPT5
#define EXTINT1         SIG_INTERRUPT6
#define MAX_VAL		36676 


#else
#  error "Wrong Target!\n"
#endif


/****************************************************************************/
/*Oeffentliche Prozeduren:                                                  */
/*void init_pushbutton(void (*callback)(void)): Initialisiert den Pushbutton*/
/****************************************************************************/

void init_pushbutton0(void (*callback)(void));
void init_pushbutton1(void (*callback)(void));

#endif
