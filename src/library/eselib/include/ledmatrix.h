
/**
 * lib: ledmatrix
 *
 * this library contains the dot matrix display.
 * 
 * matnr: 0201623
 * date: 16.1.2012
 * @author kyrill winkler <kyrill.winkler@student.tuwien.ac.at>
 */

#ifndef _LEDMATRIX_H_
#define _LEDMATRIX_H_

#include <avr/io.h>
//#include <avr/delay.h>
#include "spi.h"

#define ROWSEL_DDR DDRA
#define ROWSEL_PORT PORTA
#define ROW_EN PA7
#define SELECT0 PA0
#define SELECT1 PA1
#define SELECT2 PA2
#define BLANK   PB4

// length of the scroll buffer
#define SCROLL_BUFFER_LEN 32 
#define SCROLLING_SPEED 500

/**
 * Initialize the Dotmatrix Display
 **/
void init_dotmatrix(void);

/**
 * Writes the first 6 characters of a string to the send-buffer.
 *
 * @param data The null-terminated string that should be written.
 **/
void dotmatrix_send(char *data);

/**
 * Writes a character to the send-buffer.
 *
 * @param chr The character that should be written.
 * @param pos The position of the character.
 **/
void dotmatrix_send_char(uint8_t chr, uint8_t pos);

/**
 * Writes an integer <1000 to the send-buffer.
 *
 * @param integer The integer that should be written (modulo 1k).
 * @param pos The position of the integer.
 **/
void dotmatrix_send_int(uint16_t integer, uint8_t pos);

/**
 * Writes an comma (dot) at a specified position.
 *
 * @param pos The position of the comma.
 **/
void dotmatrix_send_comma(uint8_t pos);

/**
 * Writes a "degree celsius" at a specified position.
 *
 * @param pos The position of the "degree celsius".
 **/
void dotmatrix_send_degree(uint8_t pos);


/**
 * Sends a scrolling text to the display.
 *
 * @param string the string that will be displayed.
 **/
void dotmatrix_send_scrolling_text(char* string);

/**
 * Enables scrolling mode.
 *
 **/
void dotmatrix_enable_scrolling(void);

/**
 * Disables scrolling mode.
 *
 **/
void dotmatrix_disable_scrolling(void);

/**
 * Clears the sendbuffer of the display.
 *
 **/
void dotmatrix_clear(void);

/**
 * Writes an integer <1000 to the send-buffer with leading zeroes.
 *
 * @param integer The integer that should be written (modulo 1k).
 * @param pos The position of the integer.
 **/
void dotmatrix_send_threedigits(uint16_t integer, uint8_t pos);
#endif

