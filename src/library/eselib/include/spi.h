#ifndef __SPI_H__
#define __SPI_H__
/*======================================================================*/
/* Project:	Atmega128 timer driver					*/
/**File:        spi.h						*/
/* MCU          Atmel AVR						*/
/* Revision     $Revision: 1.0 $					*/
/* Date:        $Date: 2009/10/13 $					*/
/* Author(s):   Nick Mayerhofer						*/
/*======================================================================*/
#include <avr/io.h>
#include <avr/pgmspace.h>


/*======================================================================*/
/* D I G I T  V A L U E S                                               */
/*======================================================================*/



/*======================================================================*/
/* H A R D W A R E  D E F I N I T I O N                                 */
/*======================================================================*/
#define SPI_DDR 	DDRB
#define SPI_PORT	PORTB
#define DD_SS		PB0
#define DD_SCK		PB1
#define DD_MOSI		PB2
#define DD_MISO		PB3


/*======================================================================*/
/* N U M B E R  &  B A S E                                              */
/*======================================================================*/

/**
 * Attaches/Detaches a Function to the SIG_SPI Interrupt
 * @par Usage: 
 	To Attach call Function \c spiAttach(myFunction)
	To Detach the Interrupt call \c spiAttach(0);
 * @param[in] pAttachThis 0 to Detach, Function to Attach
 * @post Global Interrupts have to be enabeled \c sei()
*/
void spiAttach(void (*pAttachThis)(void) );

/** Initialize the SPI in Master Mode
*/
void spi_MasterInit(void);

/** Sends a char using the spi and waits for completion of the transfer.<br>
 * TAKE CARE: This function is \a blocking
 * @pre SPI Module have to be initialized see: spi_MasterInit()
 * @param[in] cData A character for sending
 * @return Received byte
*/
uint8_t spi_MasterTransmit(uint8_t cData);

/**Sends a char using the spi without waiting for completion
 * @pre SPI Module have to be initialized see: spi_MasterInit()
 * @param[in] cData A character for sending
*/
void spi_MasterTransmit_noblock(uint8_t cData);



#endif
