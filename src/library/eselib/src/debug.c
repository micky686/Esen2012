/**
 * LIB: DEBUG
 * 
 * This module provides some useful debug macros. Use with care, they could be
 * an overkill for microcontrollers. TODO: optimize for AVR!
 * 
 * MatNr: 0326077
 * Date: 30.10.2006
 * @author Bernd Froemel <bf@catechsoftworks.biz>
 **/
#include <stdio.h>
#include <stdarg.h>
#include "debug.h"


void d_msg(int lvl, const char* format, ...)
{
	va_list ap;

	switch (lvl)
	{
		case DBG_FATAL:
		{
			(void)fprintf(stderr, "[FATAL] ");
			break;
		}
		case DBG_ERROR:
		{
			(void)fprintf(stderr, "[ERROR] ");
			break;
		}
		case DBG_WARN:
		{
			(void)fprintf(stderr, "[WARNING] ");
			break;
		}
		case DBG_INFO:
		{
			(void)fprintf(stderr, "[INFO] ");
			break;
		}
		case DBG_DEBUG:
		{
			(void)fprintf(stderr, "[DEBUG] ");
			break;
		}
		case DBG_DEBUG2:
		{
			(void)fprintf(stderr, "[DEBUG2] ");
			break;
		}
		default:
		{
			(void)fprintf(stderr, "[UNKNWN] ");
		}
	}
	va_start(ap, format);
	(void)vfprintf(stderr, format, ap);
	va_end(ap);
	(void)fprintf(stderr,"\n");
	//(void)fflush(stderr);
}

