/*==========================================================================*/
/* Project:     AVR-GCC SRAM Memory Evaluation                              */
/* Title:       Memory Pointer Functions                                    */
/* Purpose:     SRAM memory ressource analysis of Atmel programs            */
/*                                                                          */
/* File:        mem_eval.h                                                  */ 
/* Revision     0.1                                                         */
/* Date:        10/2003                                                     */
/*                                                                          */
/* Description: Provides macros for reading the stackpointer and the end of */
/*              variable data segment                                       */
/* Software:    AVR-GCC                                                     */
/* Hardware:    AVR AT90 and ATmega series                                  */
/*                                                                          */
/* Authors:     Wilfried Elmenreich, Wolfgang Haidinger                     */
/*              TU Vienna, Real Time Group                                  */
/*              hardly touched by Bernd Froemel                             */
/*==========================================================================*/

#ifndef __MEMEVAL_H__
 #define __MEMEVAL_H__

 #include <avr/io.h>
 
 #ifdef __AVR_AT90S4433__
  #undef SPH
 #endif
 
 #ifdef SPH
  #define get_stack_ptr()({uint8_t *__stack_ptr;	\
        asm volatile("in\t%A0,__SP_L__\n\t"		\
                     "in\t%B0,__SP_H__"		\
                     : "=r" (__stack_ptr)	\
                     : /*"I" (SPL), "I" (SPH)*/	\
		     );				\
        __stack_ptr;})
 #else
  #define GetStackPtr() ({uint8_t *__stack_ptr;     \
        asm volatile("in\t%A0,__SP_L__\n\t"        \
                     "clr\t%B0"              \
                     : "=r" (__stack_ptr)     \
                     : /* "I" (SPL) */ );           \
        __stack_ptr;})
 #endif

 #define end_of_used_mem()({uint8_t *__end_of_used_mem_ptr;	\
        asm volatile("ldi\t%A0,lo8(__bss_end)\n\t"		\
                     "ldi\t%B0,hi8(__bss_end)"		\
                     : "=r" (__end_of_used_mem_ptr)	\
		     );				\
        __end_of_used_mem_ptr;})
#endif


void initialize_memory(void);
void analyze_memory(uint8_t **largest_begin, uint16_t *largest_size);
uint16_t get_free_memory(void);
void print_mem_stats(void);

