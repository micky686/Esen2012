/*****************************************************/
/*Modulname: heater.c                            */
/*Name: Florian Bartl                                */
/*MNr.: 0425594                                      */
/*Beschreibung:Treiber zur ansteuerung des Heizelements*/
/*Datum:12.12.2011                                   */
/*                                 */
/*****************************************************/


#include "heater.h"
#include "counter.h"

/****************************************************************************/
/*Prozedur: void heater_init(void): Initialisiert das Heizelement           */
/*Parameter: DDRB: Gibt die Datenrichting von Port B an. Port B4 wird als   */
/* Ausgang gesetzt                                                          */
/*TCCR1A & TCCR1B: Timer Control Register. Werden fuer phase correct PWM mit*/
/*Prescaler clk/256 eingestellt.                                           */
/*OCR1A: Umschaltpunkt bei PWM                                              */
/****************************************************************************/

void heater_init(void){
  //TCCR0 |= ((1<<WGM00)|(1<<COM01)|(1<<COM00));
  //TCCR0 &= ~(1<<WGM01);
  //TCCR0 |= (1<<CS02) | (1<<CS01);   
  //TCCR0 &= ~(1<<CS00);        

  CNT_WGM_MODE_SELECT02(0, 1); /* phase correct PWM */
  CNT_COM_MODE_SELECT02(0, 3); /* Set OC0 on compare match, clear OC0 at BOTTOM, (inverting mode) */
  CNT_CS_MODE_SELECT02(0, 6); /* counter2,  PS: 256 */

  DDRB |= (1<<DDB4);

  heater_set(0);
}

/****************************************************************************/
/*Prozedur: void heater_set(uint8_t duty_cycle): Setzt Tastverhaeltnis der PWM*/
/*Parameter: duty_cycle: gibt Tastverhaeltnis in Prozent an                 */
/*OCR1A: Umschaltpunkt bei PWM                                              */
/****************************************************************************/


void heater_set(uint8_t duty_cycle){

  if((duty_cycle <= 100) ){

    OCR0 = ((0xFFUL*(uint16_t)duty_cycle)/100UL);

  }
}
