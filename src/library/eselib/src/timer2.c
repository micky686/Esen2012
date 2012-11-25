/*======================================================================*/
/* Application: driver for the 8-bit timer2                             */
/*                                                                      */
/*                                                                      */
/*    Revision: 1.0                                                     */
/*        Date: 2006/10/21                                              */
/*      Author: Pelesic Igor                                            */
/*              e0006828@stud3.tuwien.ac.at                             */
/*======================================================================*/

#include "timer2.h"


/*======================================================================*/
/* G L O B A L  V A R I A B L E S                                       */
/*======================================================================*/



/*======================================================================*/
/* P R O C E D U R E S                                                  */
/*======================================================================*/
/* this function configures the mode, and the duration of timer2  */
/* it also does register a callback function that is called       */
/* every time an overflow interrupt is called                     */

void configure_timer2(uint8_t mode, uint16_t duration)
{


/* an interrupt is called every 1 ms, so we count them to achieve*/
/* the time we need                                              */
    
      timer2_done=0;
      OCR2=TCNT2+(14*duration)-1;
      TIMER_MASK|=(1<<OCIE2);
      //TIMER2_CON=0x0d;
     TIMER2_CON|=(1<<CS22);
     TIMER2_CON|=(1<<CS20);
     
    
}



