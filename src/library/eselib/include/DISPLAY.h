
// ================================================================================================
///	@file		display.h
///	@author		Oliver Hechinger 	(Mat.Nr.: 0726503)
///	@author		Stefan Hell		 	(Mat.Nr.: 0725714)
///	@author		Wolfgang Wallner	(Mat.Nr.: 0725458)
///
///	@note		Best viewed with tab size = 4.
// ================================================================================================

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

// ================================================================================================
// Includes
// ================================================================================================

// standard includes
#include <stdint.h>

// project includes

// ================================================================================================
//	Definitions
// ================================================================================================

#define COLOR_P0				RGB(255,000,000)
#define COLOR_P1				RGB(000,255,000)
#define COLOR_P2				RGB(000,000,255)
#define COLOR_P3				RGB(85 ,000, 85)

#define COLOR_WALL				RGB(000,000,000)
#define COLOR_BLANK				RGB(255,255,255)
#define COLOR_FOOD				RGB(200,100, 50)

#define NOP()             asm volatile ("nop" "\t\n")

// ================================================================================================
//	Macros
// ================================================================================================

#define RGB(r,g,b)          	(((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //5 red | 6 green | 5 blue

// ================================================================================================
// Type-Definitions
// ================================================================================================

typedef enum
{
        DispObj_Head,
        DispObj_Body,
        DispObj_Food,
        DispObj_Wall,
        DispObj_Blank,
}
DisplayObject_t;

// ================================================================================================
// Function-Definitions
// ================================================================================================

void DISPLAY_init(void);
void DISPLAY_drawBg(uint16_t rgb);
uint8_t DISPLAY_drawElement(uint8_t row, uint8_t col, uint16_t rgb, DisplayObject_t object);
void DISPLAY_drawBorder(void);
uint8_t DISPLAY_drawPoints(uint8_t player_id, uint8_t points);


#endif // _DISPLAY_H_

