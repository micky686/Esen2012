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
/*              hardly touched by Bernd Froemel                             */
/*==========================================================================*/

/////////////////////////////////////////////////////////////////////////////
// HEADER FILES                                                            //
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <avr/pgmspace.h>
#include "global.h"
#include "mem_eval.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTIONS                                                               //
/////////////////////////////////////////////////////////////////////////////

void initialize_memory()
{
   uint8_t *memptr;
   
   for(memptr = end_of_used_mem(); memptr < get_stack_ptr(); memptr++)
      *memptr = (uint16_t)memptr & 0xFF;   
}

uint16_t get_free_memory(void)
{
	uint8_t *largest_begin, *largest_end;
	uint16_t largest_size;
	
	analyze_memory(&largest_begin, &largest_size);
	largest_end = largest_begin + largest_size;
	
	return SRAM_SIZE - largest_size;
}

void analyze_memory(uint8_t **largest_begin, uint16_t *largest_size)
{
   uint8_t *memptr, *endptr, *section_begin;
   uint16_t section_size;
   
   *largest_size=0;
   memptr = *largest_begin = end_of_used_mem();
   endptr = get_stack_ptr();

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

void print_mem_stats(void)
{
	uint8_t *largest_begin, *largest_end;
	uint16_t largest_size;
	
	analyze_memory(&largest_begin, &largest_size);
	largest_end = largest_begin + largest_size;
	
	printf_P(PSTR("mem_eval:\n"));
	printf_P(PSTR("unused memory: 0x%04x - 0x%04x "), (uint16_t)largest_begin, (uint16_t)largest_end);
	printf_P(PSTR("(%u Bytes)\n"), largest_size);
	printf_P(PSTR("used memory: %u Bytes\n"), SRAM_SIZE - largest_size);
}
