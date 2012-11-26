/****************************************************************/
/*Modulname: thermometer.c                                       */
/*Name: Florian Bartl                                           */
/*MNr.: 0425594                                                 */
/*Beschreibung:Treiber zur ansteuerung der thermometer     */
/*Datum:19.11.2011                                              */
/*                                             */
/****************************************************************/


#include "thermometer.h"
#include  <avr/interrupt.h>
#include <util/twi.h>

static uint16_t temperatur;

#define fehler(err) \
  do { TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);  /* stop bit senden */ } while (0)


#define send_start() \
  do { TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWEA); } while (0)

#define send_stop() \
  do { TWCR |= (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); } while (0)

#define write_addr(tempsensor) \
  do {\
    TWDR = (tempsensor); \
    TWCR = (1<<TWINT)|(1<<TWEN); } while(0)

#define read_ack() \
  do { TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN); } while (0)

#define read_nack()\
  do {\
    TWCR &= ~((1<<TWEA)|(1<<TWSTO)|(1<<TWSTA));\
    TWCR |= (1<<TWINT)|(1<<TWEN); } while (0)

/**************************************************************/
/* started temperaturmessung*/
/* und liefert die temperatur zurueck                          */
/* WARN: This call blocks until a temperature is read from the
 * requested sensor.
 * @return Returns the temperature in fixed-point:
      upper 8 bit, radix, upper 3 bit (of lower 8 bit)
 */
/* Variable: name: uebergebene sensoradresse                   */
/**************************************************************/

int16_t therm_get_temp(uint8_t name){
  uint8_t status; 
 
  send_start(); //startbyte 

  while  (!(TWCR & (1<<TWINT)));  //warten auf interrupt
  status = TWSR & 0xF8;
  if ((status != START) && (status != REPSTART)){     //statusabfrage
  fehler(0x81);
  }

  write_addr(name);  //adresse auf bus

  while  (!(TWCR & (1<<TWINT)));   //warten auf interrupt

  status = TWSR & 0xF8;
  if ((status != MR_SLA_ACK)){    //statusabfrage
  fehler((TWSR & 0xF8));
  }

  read_ack();   //naechstes byte empfangen und ack

  while  (!(TWCR & (1<<TWINT)));   //warten auf interrupt

  if ((TWSR & 0xF8) != MR_DATA_ACK){    //statusabfrage
  fehler(TWSR & 0xF8);
  }

  temperatur = (TWDR<<8);       //daten auslesen 1.datenbyte
  read_nack();      //naechstes byte empfangen und nack

  while  (!(TWCR & (1<<TWINT)));   //warten auf interrupt

  if ((TWSR & 0xF8) != MR_DATA_NACK){    //statusabfrage
  fehler(0x84);
  }

  temperatur |= TWDR;       //daten auslesen 2.datenbyte

  send_stop();  //stop bit senden

  return temperatur;
}

/**************************************************************/
/* liefert die temperatur als ganze*/
/*Zahl zurueck              */
/**************************************************************/

uint8_t therm_get_temp_high(void){
  return temperatur>>8;
}

/**************************************************************/
/* liefert die Nachkommastelle der */
/*temperatur zurueck                 */
/**************************************************************/

uint8_t therm_get_temp_low(void){
  return temperatur&0xff;
}

/***********************************************************************/
/* Initialisiert*/
/* das thermometer                                          */
/*TWSR: Prescaler der clk Leitung wird auf 64 eingestellt   */
/*TWBR: Bit Rate register. Diffisionsfaktor fuer Bitrate  */
/*EIMSK: Interrupt mask: Externer Interrupt fuer Pin 5 wird aktiviert  */
/***********************************************************************/
void therm_init(void){
 
  TWSR |= (1<<TWPS1)|(1<<TWPS0);
  TWCR |= (1<<TWINT)|(1<<TWEN);
  TWBR = 0xFF;
}

