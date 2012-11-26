#ifndef TRIGGER_H
#define TRIGGER_H

#ifndef TRIGLEN
#define TRIGLEN 4
#endif /* TRIGLEN */

#if defined(USE_TRIGGER) && defined(TRIG)
#  define trig_init() \
do { \
	TRIG_PORT |= 1<<TRIG; \
	TRIG_DDR |= 1<<TRIG; \
} while(0)
#  define trig() \
do { \
	uint8_t _i; \
	TRIG_PORT &= ~(1<<TRIG); \
	for(_i=0;_i<TRIGLEN;_i++) { \
		asm volatile("nop\n\t" : : ); \
	} \
	TRIG_PORT |= 1<<TRIG; \
} while(0)
#else
#  define trig_init() do {} while(0)
#  define trig() do {} while(0)
#endif /* USE_TRIGGER ... */

#endif /* TRIGGER_H */
