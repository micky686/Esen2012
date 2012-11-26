/***************************************************/
/*Modulname: heater.h                          */
/*Name: Florian Bartl                              */
/*MNr.: 0425594                                    */
/*Beschreibung:Headerfile des heater-treibers */
/*Datum:09.12.2011                                 */
/*                               */
/***************************************************/

#ifndef _heater_h_
#define _heater_h_

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
void heater_init(void);
void heater_set(uint8_t duty_cycle);

#endif
