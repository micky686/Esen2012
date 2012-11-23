/***************************************************/
/*Modulname: thermometer.h                          */
/*Name: Florian Bartl                              */
/*MNr.: 0425594                                    */
/*Beschreibung:Headerfile des Thermometer-treibers */
/*Datum:19.12.2011                                 */
/*                               */
/***************************************************/

#ifndef _thermometer_h_
#define _thermometer_h_

#include <avr/io.h>


#if defined(__AVR_ATmega128__)

#else
#  error "Wrong Target!\n"
#endif

#define START 0x08
#define REPSTART 0x10
#define SLA_W 0b10011000
#define THERMOMETER1 0b10010001
#define THERMOMETER2 0b10010011
#define THERMOMETER3 0b10010101
#define MR_SLA_ACK 0x40
#define MR_DATA_ACK 0x50
#define MR_DATA_NACK 0x58

/*****************************************************************************/
/*Oeffentliche Prozeduren:                                                   */
/*void init_thermometer(): Initialisiert den thermometer                     */
/*void get_temp(): liefert die aktuelle temperatur zurueck*/
/*****************************************************************************/
void therm_init(void);
int16_t therm_get_temp(uint8_t name);
/* WARN: those functions below don't actually measure, but return only the
   last result */
uint8_t therm_get_temp_high(void);
uint8_t therm_get_temp_low(void);

#endif
