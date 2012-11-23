#ifndef BAPTIZE_H
#define BAPTIZE_H

typedef union {
	struct {
		uint32_t serial;
		uint32_t series;
	};
	uint8_t b[8];
	uint64_t ll;
} nodeid_t;

static nodeid_t lo;
static nodeid_t hi;
static nodeid_t ci;

#define calc_ci_c() \
do { \
	ci.ll = ((uint8_t) ((lo.ll|hi.ll)&0x01)) + (lo.ll>>1) + (hi.ll>>1); \
} while(0)

 #define calc_ci_asm() \
do { \
	asm volatile ("ld r24, X+" "\n\t" \
		      "ld __tmp_reg__, Y+" "\n\t" \
		      "mov r25, __tmp_reg__" "\n\t" \
		      "eor r25, %2" "\n\t" \
		      "andi r25, 0x01" "\n\t" \
		      "lsl r25" "\n\t" \
		      "add r24, r25" "\n\t" \
		      "clt" "\n\t" \
		      "brcc .L%_0" "\n\t" \
		      "set" "\n" \
		      ".L%_0:" "\n\t" \
		      "add r24, __tmp_reg__" "\n\t" \
		      "st Z+, r24" "\n\t" \
		      "ldi r25, 0x06" "\n\t" \
		      "brtc .L%_1" "\n\t" \
		      "sec" "\n" \
		      ".L%_1:" "\n\t" \
		      "ld r24, X+" "\n\t" \
		      "ld __tmp_reg__, Y+" "\n\t" \
		      "adc r24, __tmp_reg__" "\n\t" \
		      "st Z+, r24" "\n\t" \
		      "dec r25" "\n\t" \
		      "brne .L%_1" "\n\t" \
		      "ld r24, X+" "\n\t" \
		      "ld __tmp_reg__, Y+" "\n\t" \
		      "adc r24, __tmp_reg__" "\n\t" \
		      "ldi r25, 0x07" "\n" \
		      ".L%_2:" "\n\t" \
		      "ror r24" "\n\t" \
		      "st Z, r24" "\n\t" \
		      "ld r24, -Z" "\n\t" \
		      "dec r25" "\n\t" \
		      "brne .L%_2" "\n\t" \
		      "ror r24" "\n\t" \
		      "st Z, r24" "\n\t" \
		      : \
		      : "x" (&lo.b), "y" (&hi.b), "z" (&ci.b) \
		      : "r24", "r25" ); \
} while(0)

#define calc_ci() calc_ci_asm()

#define comp_ci_ge_c(a,b) \ 
({ \
	uint64_t *__a, *__b; \
	__a = (uint64_t *) a; \
	__b = (uint64_t *) b; \
	(*__a >= *__b); \
})

#define comp_ci_ge_asm(a, b) \
({ \
	uint64_t *__a, *__b; \
	__a = ((uint64_t *) a)+1; \
	__b = ((uint64_t *) b)+1; \
	uint8_t __res,__tmp1, __tmp2; \
	asm volatile("ldi %0, 0x08" "\n" \
		     ".L%_1:" "\n\t" \
		     "ld %1, X+" "\n\t" \
		     "ld %2, Z+" "\n\t" \
		     "cp %1, %2" "\n\t" \
		     "brcs .L%_2" "\n\t" \
		     "dec %1" "\n\t" \
		     "brne .L%_1" "\n\t" \
		     "ldi %0, 0x01" "\n\t" \
		     "rjmp .L%_3" "\n\t" \
		     ".L%_2:" "\n\t" \
		     "clr %0" "\n\t" \
		     : "=d" (__res) \
		     : "r" (__tmp1), "r" (__tmp2), "x" (__b), "z" (__a)); \
	__res; \
})

#define comp_ci_ge(a,b) comp_ci_ge_asm(a, b)

#define comp_ci_eq_c(a,b) \ 
({ \
	uint64_t *__a, *__b; \
	__a = (uint64_t *) a; \
	__b = (uint64_t *) b; \
	(*__a >= *__b); \
})

#define comp_ci_eq_asm(a, b) \
({ \
	uint64_t *__a, *__b; \
	__a = ((uint64_t *) a); \
	__b = ((uint64_t *) b); \
	uint8_t __res,__tmp1, __tmp2; \
	asm volatile("ldi %0, 0x08" "\n" \
		     ".L%_1:" "\n\t" \
		     "ld %1, X+" "\n\t" \
		     "ld %2, Z+" "\n\t" \
		     "cp %1, %2" "\n\t" \
		     "brne .L%_2" "\n\t" \
		     "dec %1" "\n\t" \
		     "brne .L%_1" "\n\t" \
		     "ldi %0, 0x01" "\n\t" \
		     "rjmp .L%_3" "\n\t" \
		     ".L%_2:" "\n\t" \
		     "clr %0" "\n\t" \
		     : "=d" (__res) \
		     : "r" (__tmp1), "r" (__tmp2), "x" (__b), "z" (__a)); \
	__res; \
})

#endif /* BAPTIZE_H */
