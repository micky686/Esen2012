/****************************************************************/
/*Modulname: pushbutton.c                                       */
/*Name: Florian Bartl                                           */
/*MNr.: 0425594                                                 */
/*Beschreibung:Treiber zur ansteuerung der Pushbutton*/
/*Datum:12.11.2011                                              */
/*                                             */
/****************************************************************/


#include  <avr/interrupt.h>
#include "pushbutton.h"

#define TIMER2_OCI_COUNT 10 

// disable pushbutton interrupts and start timer
#define DEBOUNCE \
  EIMSK &= ~((1<<INT5)|(1<<INT6)); \
  TCCR2 |= ((1<<CS22)|(1<<CS20)); \
  TCCR2 &= ~(1<<CS21); \

// configure timer to CTC mode
#define TIMER_INIT \
  TCCR2 |= (1<<WGM21); \
  TCCR2 &= ~(1<<WGM20); \
  OCR2 = 0xff; \
  TIMSK |= (1<<OCIE2);

/**************************************************************/
/*Prozedur: void (*pushbuttoncallback0)(void): Rueckruffunktion*/
/*fuer Interrupt. Funktionsname wird uebergeben               */
/**************************************************************/

void (*pushbuttoncallback0)(void);

/**************************************************************/
/*Prozedur: void (*pushbuttoncallback1)(void): Rueckruffunktion*/
/*fuer Interrupt. Funktionsname wird uebergeben               */
/**************************************************************/

void (*pushbuttoncallback1)(void);

/*************************************************************/
/*Interrupthandler:  Timer2: timer abstellen danach wird ueberprueft */
/*welcher pushbutton gedrueckt wurde und letztendlich werden */
/*externen Interrupts wieder aktiviert                       */
/*************************************************************/

SIGNAL(TIMER2_COMP_vect){
  static volatile uint16_t count; 
  if (count < TIMER2_OCI_COUNT) {
    count ++;
  } else {
    count = 0;
    TCCR2 &= ~((1<<CS22)|(1<<CS21)|(1<<CS20));  // stop timer
    EIMSK |= ((1<<INT5)|(1<<INT6));  // enable pushbutton interrupts
  }
}

/*************************************************************/
/*Interrupthandler: Behandelt auftretenden externen Interrupt*/
/*fuer pushbutton0 und loest Rueckrufprozedur aus.           */
/*************************************************************/

SIGNAL(INT5_vect){
  static volatile uint8_t i;
  if (i == 1) {
    pushbuttoncallback0();
    i = 0;
  } else {
    i = 1;
  }
  DEBOUNCE;
}

/*************************************************************/
/*Interrupthandler: Behandelt auftretenden externen Interrupt*/
/*fuer pushbutton0 und loest Rueckrufprozedur aus.           */
/*************************************************************/


SIGNAL(INT6_vect){
  static volatile uint8_t i;
  if (i == 1) {
    pushbuttoncallback1();
    i = 0;
  } else {
    i = 1;
  }
  DEBOUNCE;
}

/***********************************************************************/
/*Prozedur: void init_pushbutton0(void (*callback)(void)): Initialisiert*/
/* den virtuellen Pushbutton                                           */
/*Parameter: pushbuttoncallback: Rueckruffunktion fuer Interrupt       */
/*DDRE: Data Direction Register: Port E wird als Eingang definieren.   */
/*EICRB: Control Register: Interrupt wird auf fallende Kante gestellt  */
/*EIMSK: Interrupt mask: Externer Interrupt fuer Pin 5 wird aktiviert  */
/***********************************************************************/

void init_pushbutton0(void (*callback)(void)){
  pushbuttoncallback0=callback;
  DDRE  &= ~(1<<DDE5);
  PORTE |= (1<<PE5);				
  EICRB |=(1<<ISC50);	
  EICRB &= ~(1<<ISC51);		
  EIMSK |= (1<<INT5);
  TIMER_INIT;
}

/***********************************************************************/
/*Prozedur: void init_pushbutton1(void (*callback)(void)): Initialisiert*/
/* den virtuellen Pushbutton                                           */
/*Parameter: pushbuttoncallback: Rueckruffunktion fuer Interrupt       */
/*DDRE: Data Direction Register: Port E wird als Eingang definieren.   */
/*EICRB: Control Register: Interrupt wird auf fallende Kante gestellt  */
/*EIMSK: Interrupt mask: Externer Interrupt fuer Pin 6 wird aktiviert  */
/***********************************************************************/

void init_pushbutton1(void (*callback)(void)){
  pushbuttoncallback1=callback;
  DDRE &= ~(1<<DDE6);
  PORTE |= (1<<PE6);
  EICRB |= (1<<ISC60);
  EICRB &= ~(1<<ISC61);
  EIMSK |= (1<<INT6);
  TIMER_INIT;
}

