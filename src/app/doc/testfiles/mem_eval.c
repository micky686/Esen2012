/*==========================================================================*/
/* Project:     AVR-GCC SRAM Memory Evaluation                              */
/* Title:       Memory Init and Anlyze Functions                            */
/* Purpose:     SRAM memory ressource analysis of Atmel programs            */
/*                                                                          */
/* File:        mem_eval.c                                                  */ 
/* Revision     0.1                                                         */
/* Date:        10/2003                                                     */
/*                                                                          */
/* Description: Provides an initialization and memory analyzation function  */
/* Software:    AVR-GCC                                                     */
/* Hardware:    AVR AT90 and ATmega series                                  */
/*                                                                          */
/* Authors:     Wilfried Elmenreich                                         */
/*              TU Vienna, Real Time Group                                  */
/*==========================================================================*/

/////////////////////////////////////////////////////////////////////////////
// HEADER FILES                                                            //
/////////////////////////////////////////////////////////////////////////////

#include <io.h>
#include "mem_eval.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTIONS                                                               //
/////////////////////////////////////////////////////////////////////////////

int x
shit gf

void InitializeMemory()
{
   uint8_t *memptr;
   
   for(memptr = EndOfUsedMem(); memptr < GetStackPtr(); memptr++)
      *memptr = (uint16_t)memptr & 0xFF;   
}

void AnalyzeMemory(uint8_t **largest_begin,uint16_t *largest_size)
{
   uint8_t *memptr, *endptr, *section_begin;
   uint16_t section_size;
   
   *largest_size=0;
   memptr = *largest_begin = EndOfUsedMem();
   endptr = GetStackPtr();

   while(memptr < endptr)
   {
     if (*memptr == ((uint16_t)memptr & 0xFF))
     {
       section_begin = memptr;
       while(1)
    {
           memptr++;
           //*largest_begin = (uint8_t *) 1;
       if ((*memptr != ((uint16_t)memptr & 0xFF)) || (memptr == endptr-1)) 
        {
            //*largest_begin = (uint8_t *) 2;
           if ((section_size =  (memptr - section_begin)) > *largest_size)
        {
           *largest_size = section_size;
           *largest_begin = section_begin;
        }
           break;
        }
    } /* while(1) */
      }
      memptr++;
    } /* while(memptr < endptr) */
}
