/**
 * LIB: TEST
 *
 * This library contains the test cases.
 * 
 * MatNr: 0201623
 * Date: 2.1.2012
 * @author Kyrill Winkler <kyrill.winkler@student.tuwien.ac.at>
 */

#ifndef __TEST_H__
#define __TEST_H__

#include <avr/io.h>
#include "cooler.h"
#include "heater.h"
#include "thermometer.h"

uint8_t test_all (void);
uint8_t test_cooler_config (void);
uint8_t test_cooler_set (void);
uint8_t test_heater_config (void);
uint8_t test_heater_set (void);
uint8_t test_thermometer_config(void);
#endif
