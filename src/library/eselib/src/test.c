/**
* LIB: TEST
*
* This library contains the test cases.
* 
* MatNr: 0201623
* Date: 1.2.2012
* @author Kyrill Winkler <kyrill.winkler@student.tuwien.ac.at>
*/

#include "test.h"

/**
 * Run all test cases.
 *
 * @return: 0 on success, 1 on failure
 **/
uint8_t test_all() {
	return
		test_cooler_config() || 
		test_cooler_set();
}

/**
 * Run the cooler configuration tests
 *
 * @return: 0 on success or a bitwise or of the following
 *          1 on ddr error
 *          2 on tccr1a error
 *          4 on tccr1b error
 *          8 on ocr1c error
 **/
uint8_t test_cooler_config () {
  uint8_t retval = 0;
	uint8_t ddrb = DDRB;
	uint8_t tccr1a = TCCR1A;
	uint8_t tccr1b = TCCR1B;

	
	init_cooler();

	if (DDRB != (ddrb | (1<<DDB7))) {
		retval |= 0x1;
  }
	if (TCCR1A != ((tccr1a | ((1<<WGM11) | (1<<WGM10) | (1<<COM1C1))) & ~(1<<COM1C0))) {
		retval |= 0x2;
  }
	if (TCCR1B != ((tccr1b | ((1<<CS12) | (1<<CS10))) & ~((1<<WGM13) | (1<<WGM12) | (1<<CS11)))) {
		retval |= 0x4;
  }
  if (OCR1C != 0x000) {
    retval |= 0x8;
  }
  return retval;
}

/**
 * Run the cooler set value tests
 *
 * @return: 0 on success or a bitwise or of a one, shifted by the number of the test that failed.
 **/
uint8_t test_cooler_set () {
	uint16_t ocr1c = OCR1C;
	uint8_t retval = 0;

	set_cooler(0);
	retval |= ((OCR1C != 0x00)<<0);

	ocr1c = OCR1C;
	set_cooler(-1);
	retval |= ((OCR1C != ocr1c)<<1);

	set_cooler(1);
	retval |= ((OCR1C != 0xA)<<2);

	set_cooler(42);
	retval |= ((OCR1C != 0x1AD)<<3);

	set_cooler(100);
	retval |= ((OCR1C != 0x3FF)<<4); //XXX why is OCR1C 0x16f
#if 0
  #if 1
	return (uint8_t) (OCR1C & 0x00ff);
  #else
	return (uint8_t) (OCR1C >> 8 );
  #endif
#endif

	
  ocr1c = OCR1C;
	set_cooler(101);
	retval |= ((OCR1C != ocr1c)<<5);

	set_cooler(0);
	return retval;
}
/*
uint8_t test_cooler_config_rigorous (void) {
	uint8_t retval = 0;
	
	DDRB = 0x00;
	retval |= test_cooler_config();
	DDRB |= 0xFF;
	retval |= test_cooler_config();
	TCCR1A = 0x00;
	retval |= test_cooler_config();
	TCCR1A = 0xFF;
	retval |= test_cooler_config();
	TCCR1B = 0x00;
	retval |= test_cooler_config();
	TCCR1B = 0xFF;
	retval |= test_cooler_config();
	return retval;
}
*/

/**
 * Run the heater configuration tests
 *
 * @return: 0 on success or a bitwise or of the following
 *          1 on ddr error
 *          2 on tccr0 error
 *          4 on ocr0 error
 **/
uint8_t test_heater_config () {
  uint8_t retval = 0;
	uint8_t ddrb = DDRB;
	uint8_t tccr0 = TCCR0;

	
	heater_init();

	if (DDRB != (ddrb | (1<<DDB4))) {
		retval |= 0x1;
  }
	if (TCCR0 != ((tccr0 | 1<<WGM00 | 1<<COM01 |  1<<COM00 | 1<<CS02 | 1<<CS01 ) &
                  ~(1<<WGM01 | 1<<CS00))) {
		retval |= 0x2;
  }
  if (OCR0 != 0x000) {
    retval |= 0x4;
  }
  return retval;
}


/**
 * Run the heater set value tests
 *
 * @return: 0 on success or a bitwise or of a one, shifted by the number of the test that failed.
 **/
uint8_t test_heater_set() {
	uint8_t ocr0 = OCR0;
	uint8_t retval = 0;

	heater_set(0);
	retval |= ((OCR0 != 0x00)<<0);

	ocr0 = OCR0;
	heater_set(-1);
	retval |= ((OCR0 != ocr0)<<1);

	heater_set(1);
	retval |= ((OCR0 != 0x2)<<2);

  ocr0 = OCR0;
	heater_set(42);
	//retval |= ((OCR0 != 0x6b)<<3);
	retval |= ((OCR0 != ocr0)<<5);
	
  ocr0 = OCR0;
	heater_set(100);
	//retval |= ((OCR0 != 0xFF)<<4);
	retval |= ((OCR0 != ocr0)<<5);
	
  ocr0 = OCR0;
	heater_set(101);
	retval |= ((OCR0 != ocr0)<<5);
  
	heater_set(0);
	return retval;
}


/**
 * Check the temperature sensor configuration for correctness
 *
 * @return: 0 on success or a bitwise or of
 *          1 on an error in TWSR configuration
 *          2 on an error in TWCR configuration
 *          4 on an error in TWBR
 **/

uint8_t test_thermometer_config() {
  uint8_t retval = 0;

  therm_init();

  if ((TWSR & ((1<<TWPS1)|(1<<TWPS0))) != ((1<<TWPS1)|(1<<TWPS0))) {
    retval |= 0x1;
  }
  if ((TWCR & (1<<TWEN)) != (1<<TWEN)) {
    retval |= 0x2;
  }
  if (TWBR != 0xff) {
    retval |= 0x04;
  }

  return retval;
}
