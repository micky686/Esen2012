/***************************************************/
/*Modulname: cooler.h                          */
/*Name: Florian Bartl                              */
/*MNr.: 0425594                                    */
/*Beschreibung:Headerfile des kuehler-treibers */
/*Datum:12.12.2011                                 */
/*                               */
/***************************************************/

#ifndef _COOLER_H_
#define _COOLER_H_

#include <avr/io.h>


#if defined(__AVR_ATmega128__)

#else
#  error "Wrong Target!\n"
#endif


/*****************************************************************************/
/*Oeffentliche Prozeduren:                                                   */
/*void init_bulb(): Initialisiert die Gluehbirne                             */
/*void set_bulb(uint8_t duty_cycle): Setzt PWM-Tastverhaeltnis der Gluehbirne*/
/*****************************************************************************/
void init_cooler(void);
void set_cooler(uint8_t duty_cycle);

#endif
