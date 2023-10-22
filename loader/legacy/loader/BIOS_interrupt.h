#ifndef _BIOS_INTERRUPT_
#define _BIOS_INTERRUPT_

#include <types.h>


typedef struct reg16
{
	uint16_t ax;
	uint16_t bx;
	uint16_t cx;
	uint16_t dx;
	uint16_t si;
	uint16_t di;
	uint16_t es;
	
	uint16_t bp;
	
	uint16_t flags;
}reg16;
typedef struct reg32
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;

	uint16_t es;
	uint32_t eflags;
}reg32;
#define FLAG_CARRY 0x01
#define EFLAG_CARRY 0x01

#define bios_int16(_reg16, _num) 		\
{						\
	asm 					\
	(					\
		"push	%%bp\n"			\
 		"lea	%0,%%bp\n"		\
 		"push	%%bp\n"			\
		"push	%%ds\n"			\
						\
		"mov	(%%bp), %%ax\n"		\
		"mov	2(%%bp), %%bx\n"	\
		"mov	4(%%bp), %%cx\n"	\
		"mov	6(%%bp), %%dx\n"	\
		"mov	8(%%bp), %%si\n"	\
		"mov	10(%%bp), %%di\n"	\
		"mov	12(%%bp), %%es\n"	\
		"mov	14(%%bp), %%bp\n"	\
						\
		"int	%1\n"			\
 		"pop	%%ds\n"			\
		"push	%%bp\n"			\
		"mov	%%sp, %%bp\n"		\
		"mov	2(%%bp), %%bp\n"	\
						\
		"mov	%%ax, (%%bp)\n"		\
		"mov	%%bx, 2(%%bp)\n"	\
		"mov	%%cx, 4(%%bp)\n"	\
		"mov	%%dx, 6(%%bp)\n"	\
		"mov	%%si, 8(%%bp)\n"	\
		"mov	%%di, 10(%%bp)\n"	\
		"mov	%%es, 12(%%bp)\n"	\
						\
		"pop	%%ax\n"			\
		"mov	%%ax, 14(%%bp)\n"	\
		"pop	%%bp\n"			\
		"pop	%%bp\n"			\
						\
		:				\
		: "m"(*_reg16), "i"(_num)	\
	);					\
}	

#define bios_int32(_reg32, _num) 		\
{						\
	asm 					\
	(					\
 		"lea	%0,%%ebp\n"		\
		"push	%%ds\n"			\
						\
		"mov	(%%ebp), %%eax\n"	\
		"mov	4(%%ebp), %%ebx\n"	\
		"mov	8(%%ebp), %%ecx\n"	\
		"mov	12(%%ebp), %%edx\n"	\
		"mov	16(%%ebp), %%esi\n"	\
		"mov	20(%%ebp), %%edi\n"	\
		"mov	24(%%ebp), %%es\n"	\
		"mov	26(%%ebp), %%ebp\n"	\
						\
		"int	%1\n"			\
 		"pop	%%ds\n"			\
						\
		"push	%%ebp\n"		\
		"lea	%0, %%ebp\n"		\
						\
		"mov	%%eax, (%%ebp)\n"	\
		"mov	%%ebx, 4(%%ebp)\n"	\
		"mov	%%ecx, 8(%%ebp)\n"	\
		"mov	%%edx, 12(%%ebp)\n"	\
		"mov	%%esi, 16(%%ebp)\n"	\
		"mov	%%edi, 20(%%ebp)\n"	\
		"mov	%%es, 24(%%ebp)\n"	\
						\
		"pop	%%eax\n"		\
		"mov	%%eax, 26(%%ebp)\n"	\
						\
		"pushfd\n"			\
		"pop	30(%%ebp)\n"		\
		:				\
		: "m"(*_reg32), "i"(_num)	\
	);					\
}
#endif
