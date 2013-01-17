// ================================================================================================
///	@file		DISPLAY.c
///	@author		Oliver Hechinger 	(Mat.Nr.: 0726503)
///	@author		Stefan Hell		 	(Mat.Nr.: 0725714)
///	@author		Wolfgang Wallner	(Mat.Nr.: 0725458)
///
///	@brief		Display functions
///
///
///
///	@note		Best viewed with tab size = 4.
// ================================================================================================

// standard includes
#include<avr/io.h>
#include<stdint.h>
#include<string.h>
#include<avr/delay.h>

// project include
#include "DISPLAY.h"
#include "FONT.h"


// ================================================================================================
// Module internal Definitions
// ================================================================================================

#define DISPLAY_WIDTH         	(360)
#define DISPLAY_HEIGHT          (640)

#define _CS_LO()				PORTG &= ~_BV(PG0);
#define _CS_HI()				PORTG |=  _BV(PG0);

#define _DDIR_DIS()  			PORTB &= ~_BV(PB4); 
#define _DDIR_CPU()     		PORTB |=  _BV(PB4);               

#define _RST_LO()				PORTG &= ~_BV(PG1);
#define _RST_HI()				PORTG |=  _BV(PG1);

#define DSEL_CMD()				PORTD &= ~_BV(PD7);
#define DSEL_DAT()				PORTD |=  _BV(PD7);

#define _WR_LO()				PORTD &= ~_BV(PD5);
#define _WR_HI()				PORTD |=  _BV(PD5);

#define _RD_LO()				DDRA = 0x00; PORTD &= ~_BV(PD6);
#define _RD_HI()				PORTD |=  _BV(PD6); DDRA = 0xFF;

#define WRITE_CMD_DATA(x)		(PORTA = x)
#define READ_CMD_DATA			(PINA)

#define DISPLAY_WRITE(param)	DSEL_DAT(); _WR_LO();NOP();	WRITE_CMD_DATA(param); _WR_HI()

// Commands

#define DISPLAY_NOP				0x00
#define DISPLAY_PIXEL_FORMAT 	0x3A
#define DISPLAY_ADDRESS_MODE	0x36

#define DISPLAY_SET_COL			0x2A
#define DISPLAY_SET_ROW			0x2B

// ================================================================================================
// Module internal Macros
// ================================================================================================

// ================================================================================================
// Module internal Type-Definitions
// ================================================================================================

// ================================================================================================
// Module internal Variables
// ================================================================================================

// ================================================================================================
// Module internal Function Prototypes
// ================================================================================================

void DISPLAY_writeCommandLast(uint8_t cmd);
void DISPLAY_writeCommand(uint8_t cmd);
void DISPLAY_writeParameterLast(uint8_t param);
void DISPLAY_writeParameter(uint8_t param);
void DISPLAY_writeByte(uint8_t cmd, uint8_t data);
uint8_t DISPLAY_readByte(uint8_t cmd);
void DISPLAY_delayMs(uint32_t count);
extern uint8_t font_0[94][6];
// ================================================================================================
// Function-Implementations
// ================================================================================================

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void DISPLAY_init() {

	PORTB |= _BV(PB5);		// set BL_EN output PULL
	DDRB  |= _BV(PB5);

	PORTB &= ~_BV(PB4);		// set DDIR output NOPULL
	DDRB  |=  _BV(PB4);

	PORTD &= ~_BV(PD4);		// set TEAR input NOPULL
	DDRD  &= ~_BV(PD4);

	PORTG |= _BV(PG1);		// set RST output PULL
	DDRG  |= _BV(PG1);

	PORTG |= _BV(PG0);		// set CS output PULL
	DDRG  |= _BV(PG0);

	PORTD |= _BV(PD7);		// set DSEL output PULL
	DDRD  |= _BV(PD7);

	PORTD |= _BV(PD6);		// set RD output PULL
	DDRD  |= _BV(PD6);

	PORTD |= _BV(PD5);		// set WR output PULL
	DDRD  |= _BV(PD5);

	PORTA  = 0xFF;			// set I/O port output PULL
	DDRA   = 0xFF;


	uint32_t i = 0;

	_RST_LO();
	DISPLAY_delayMs(10);
	_RST_HI();
	DISPLAY_delayMs(10);

	DISPLAY_writeCommand(0xB0);
	DISPLAY_writeParameterLast(0x04);

	DISPLAY_writeCommand(0xE0);
	DISPLAY_writeParameter(0x00);
	DISPLAY_writeParameterLast(0x38);

	DISPLAY_delayMs(5);

	DISPLAY_writeCommand(0xB3);
	DISPLAY_writeParameter(0x82);
	DISPLAY_writeParameterLast(0x00);

	DISPLAY_writeCommand(0xC0);
	DISPLAY_writeParameterLast(0x03);

	DISPLAY_writeCommand(0xC1);
	DISPLAY_writeParameter(0x00);
	DISPLAY_writeParameter(0x00);
	DISPLAY_writeParameter(0x13);
	DISPLAY_writeParameter(0x08);
	DISPLAY_writeParameterLast(0x0A);

	DISPLAY_writeCommand(0xC4);
	DISPLAY_writeParameter(0x11);
	DISPLAY_writeParameter(0x01);
	DISPLAY_writeParameter(0x23);
	DISPLAY_writeParameterLast(0x01);

	DISPLAY_writeCommand(0xC8);
	DISPLAY_writeParameter(0x08);
	DISPLAY_writeParameter(0x10);
	DISPLAY_writeParameter(0x66);
	DISPLAY_writeParameter(0xCB);
	DISPLAY_writeParameter(0x0B);
	DISPLAY_writeParameter(0x89);
	DISPLAY_writeParameter(0x36);
	DISPLAY_writeParameter(0x06);
	DISPLAY_writeParameter(0x05);
	DISPLAY_writeParameter(0x08);
	DISPLAY_writeParameter(0x06);
	DISPLAY_writeParameter(0x63);
	DISPLAY_writeParameter(0x99);
	DISPLAY_writeParameter(0x0B);
	DISPLAY_writeParameter(0xAC);
	DISPLAY_writeParameter(0x66);
	DISPLAY_writeParameter(0x10);
	DISPLAY_writeParameterLast(0x05);

	DISPLAY_writeCommand(0xC9);
	DISPLAY_writeParameter(0x08);
	DISPLAY_writeParameter(0x10);
	DISPLAY_writeParameter(0x66);
	DISPLAY_writeParameter(0xCB);
	DISPLAY_writeParameter(0x0B);
	DISPLAY_writeParameter(0x89);
	DISPLAY_writeParameter(0x36);
	DISPLAY_writeParameter(0x06);
	DISPLAY_writeParameter(0x05);
	DISPLAY_writeParameter(0x08);
	DISPLAY_writeParameter(0x06);
	DISPLAY_writeParameter(0x63);
	DISPLAY_writeParameter(0x99);
	DISPLAY_writeParameter(0x0B);
	DISPLAY_writeParameter(0xAC);
	DISPLAY_writeParameter(0x66);
	DISPLAY_writeParameter(0x10);
	DISPLAY_writeParameterLast(0x05);

	DISPLAY_writeCommand(0xCA);
	DISPLAY_writeParameter(0x08);
	DISPLAY_writeParameter(0x10);
	DISPLAY_writeParameter(0x66);
	DISPLAY_writeParameter(0xCB);
	DISPLAY_writeParameter(0x0B);
	DISPLAY_writeParameter(0x89);
	DISPLAY_writeParameter(0x36);
	DISPLAY_writeParameter(0x06);
	DISPLAY_writeParameter(0x05);
	DISPLAY_writeParameter(0x08);
	DISPLAY_writeParameter(0x06);
	DISPLAY_writeParameter(0x63);
	DISPLAY_writeParameter(0x99);
	DISPLAY_writeParameter(0x0B);
	DISPLAY_writeParameter(0xAC);
	DISPLAY_writeParameter(0x66);
	DISPLAY_writeParameter(0x10);
	DISPLAY_writeParameterLast(0x05);

	DISPLAY_writeCommand(0xD0);
	DISPLAY_writeParameter(0x00);
	DISPLAY_writeParameter(0x73);
	DISPLAY_writeParameter(0xC0);
	DISPLAY_writeParameterLast(0x9F);

	DISPLAY_writeCommand(0xD2);
	DISPLAY_writeParameter(0x01);
	DISPLAY_writeParameter(0x00);
	DISPLAY_writeParameterLast(0x00);

	// additional parameters here

	DISPLAY_writeCommand(0x35);
	DISPLAY_writeParameterLast(0x00);

	DISPLAY_writeCommand(0x44);
	DISPLAY_writeParameter(0x00);
	DISPLAY_writeParameterLast(0x00);

	// first image on DISPLAY
	
	DISPLAY_writeCommand(0x2C);
	for(i=0; i<230399; i++) {
		DISPLAY_WRITE(0x00);
		DISPLAY_WRITE(0x00);
		DISPLAY_WRITE(0x00);
	}

	DISPLAY_WRITE(0x00);
	DISPLAY_WRITE(0x00);
	DISPLAY_writeParameterLast(0x00);

	// end image

	DISPLAY_writeCommandLast(0x11);

	DISPLAY_delayMs(120);

	DISPLAY_writeCommandLast(0x29);

	DISPLAY_delayMs(100);


	DISPLAY_writeCommand(DISPLAY_PIXEL_FORMAT);		// 16bpp
	DISPLAY_writeParameterLast(0x05);

	DISPLAY_writeCommand(DISPLAY_ADDRESS_MODE);		// normal address mode
	DISPLAY_writeParameterLast(0x00);
}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void DISPLAY_drawDot(uint8_t row, uint8_t col, uint16_t rgb, uint8_t grid) {
	uint32_t i = 0;
	uint8_t j = 0;
	uint16_t set_row = 0;
	uint16_t set_col = 0;

	if		(row*grid >= 640) 		return (1);
	else if	(col*grid >= 360) 		return (2);

	set_row = row*grid;

	DISPLAY_writeCommand(DISPLAY_SET_ROW);		
	DISPLAY_writeParameter((uint8_t)((set_row>>8)&0x03));
	DISPLAY_writeParameter((uint8_t)(set_row));	

	set_row = (row*grid)+grid-1;

	DISPLAY_writeParameter((uint8_t)((set_row>>8)&0x03));
	DISPLAY_writeParameterLast((uint8_t)(set_row));	


	set_col = col*grid;

	DISPLAY_writeCommand(DISPLAY_SET_COL);		
	DISPLAY_writeParameter((uint8_t)((set_col>>8)&0x03));
	DISPLAY_writeParameter((uint8_t)(set_col));	

	set_col = (col*grid)+grid-1;

	DISPLAY_writeParameter((uint8_t)((set_col>>8)&0x03));
	DISPLAY_writeParameterLast((uint8_t)(set_col));	


		DISPLAY_writeCommand(0x2C);
		for(i=0; i<(grid*grid); i++) {
			DISPLAY_WRITE((uint8_t)(rgb>>8));
			DISPLAY_WRITE((uint8_t)rgb);
		}

}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void DISPLAY_drawBg(uint16_t rgb) {

	uint32_t i = 0;

	DISPLAY_writeCommand(0x2C);
	for(i=0; i<230399; i++) {
		DISPLAY_WRITE((uint8_t)(rgb>>8));
		DISPLAY_WRITE((uint8_t)rgb);
	}

	DISPLAY_WRITE((uint8_t)(rgb>>8));
	DISPLAY_writeParameterLast((uint8_t)rgb);
}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void DISPLAY_writeByte(uint8_t cmd, uint8_t data) {

  	_DDIR_DIS();
	DSEL_CMD();
	_CS_LO();
	_WR_LO();
	WRITE_CMD_DATA(cmd);
	_WR_HI();

	DSEL_DAT();
	_WR_LO();
	WRITE_CMD_DATA(data);
	_WR_HI();

	_CS_HI();
}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
uint8_t DISPLAY_readByte(uint8_t cmd) {

	uint8_t tmp=0;

	_CS_LO();
	DSEL_CMD();

	_WR_LO();
	WRITE_CMD_DATA(cmd);
	_WR_HI();
	DSEL_DAT();

	_RD_LO();
	_DDIR_CPU();
	tmp = READ_CMD_DATA;
	_RD_HI();

	_RD_LO();
	_DDIR_CPU();
   	tmp = READ_CMD_DATA;
	_RD_HI();

	_CS_HI();

	return (tmp);
}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void DISPLAY_writeCommandLast(uint8_t cmd) {

  	_DDIR_DIS();
	DSEL_CMD();
	_CS_LO();
	_WR_LO();
	WRITE_CMD_DATA(cmd);
	_WR_HI();
	_CS_HI();
}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void DISPLAY_writeCommand(uint8_t cmd) {

  	_DDIR_DIS();
	DSEL_CMD();
	_CS_LO();
	_WR_LO();
	WRITE_CMD_DATA(cmd);
	_WR_HI();
}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void DISPLAY_writeParameterLast(uint8_t param) {

	DSEL_DAT();
	_WR_LO();
	WRITE_CMD_DATA(param);
	_WR_HI();
	_CS_HI();
}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void DISPLAY_writeParameter(uint8_t param) {

	DSEL_DAT();
	_WR_LO();
	WRITE_CMD_DATA(param);
	_WR_HI();
}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void DISPLAY_string(uint8_t x, uint8_t y, uint16_t font_color, uint16_t bg_color, uint8_t pixel_size, char *ptr){
	uint8_t cx, cy;
	cx = x; cy = y;
	while (*ptr != '\0'){
		DISPLAY_draw_char(cx, cy, font_color, bg_color, pixel_size, *ptr++);
		cx = cx + pixel_size*(FONT_WIDTH - 3);
	}
}

void DISPLAY_draw_char(uint8_t x, uint8_t y, uint16_t font_color, uint16_t bg_color, uint8_t pixel_size, char c){
	uint8_t bit_index = 0;
	uint8_t byte_index = 0;
	uint8_t cy, cx;
	int index = c - 30;
		for(cy = y + FONT_HEIGHT; cy > y; cy--){
			for(cx = x; cx < x + FONT_WIDTH; cx++){
				if (FONT8x16[index][byte_index]>>(7-bit_index) & 01)
					DISPLAY_drawDot(cx,cy,font_color,  pixel_size);
				else
					DISPLAY_drawDot(cx,cy, bg_color, pixel_size);
				bit_index++;
				if(bit_index >= 8){
					bit_index = 0;
					byte_index++;
				}
			}
		}
}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void DISPLAY_delayMs(uint32_t count){
	uint16_t i=0;

	while(count) {
		for(i=0; i<7500; i++) {
			NOP();
		}
		count--;
	}
}

