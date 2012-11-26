/*****************************************************/
/*Modulname: cooler.c                            */
/*Name: Florian Bartl                                */
/*MNr.: 0425594                                      */
/*Beschreibung:Treiber zur ansteuerung des Kuehlelements*/
/*Datum:09.12.2011                                   */
/*                                 */
/*****************************************************/


#include "cooler.h"

/****************************************************************************/
/*Prozedur: void init_cooler(void): Initialisiert das heizelement           */
/*Parameter: DDRB: Gibt die Datenrichting von Port B an. Port B7 wird als   */
/* Ausgang gesetzt                                                          */
/*TCCR1A & TCCR1B: Timer Control Register. Werden fuer phase correct PWM mit*/
/*Prescaler clk/1024 eingestellt.                                           */
/*OCR1A: Umschaltpunkt bei PWM                                              */
/****************************************************************************/

void init_cooler(void){

  DDRB |= (1<<DDB7);    

  TCCR1A |= ((1<<WGM11)|(1<<WGM10)|(1<<COM1C1));
  TCCR1A &= ~( (1<<COM1C0));

  TCCR1B |= (1<<CS12) | (1<<CS10);           
  TCCR1B &= ~((1<<WGM13) | (1<<WGM12) | (1<<CS11));

OCR1C = 0x000;       

}

/****************************************************************************/
/*Prozedur: void set_cooler(uint8_t duty_cycle): Setzt Tastverhaeltnis der PWM*/
/*Parameter: duty_cycle: gibt Tastverhaeltnis in Prozent an                 */
/*OCR1A: Umschaltpunkt bei PWM                                              */
/****************************************************************************/


void set_cooler(uint8_t duty_cycle){

  if((duty_cycle <= 100) ){
        OCR1C = (uint16_t) ((0x3ff * ((uint32_t) duty_cycle))/100);
  }
}
