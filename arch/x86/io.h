#ifndef _IO_H_
#define _IO_H_

#include <types.h>

static inline void __io_write_8(uint16_t addr, uint8_t data)
{
	asm (
		"mov %0, %%al\n\t"
		"mov %1, %%dx\n\t"
		"out %%al, %%dx\n\t"
		:
		:"m"(data), "m"(addr)
	);
}
static inline void __io_write_16(uint16_t addr, uint16_t data)
{
	asm (
		"mov %0, %%ax\n\t"
		"mov %1, %%dx\n\t"
		"out %%ax, %%dx\n\t"
		:
		:"m"(data), "m"(addr)
	);
}
static inline void __io_write_32(uint16_t addr, uint32_t data)
{
	asm (
		"mov %0, %%eax\n\t"
		"mov %1, %%dx\n\t"
		"out %%eax, %%dx\n\t"
		:
		:"m"(data), "m"(addr)
	);
}
static inline uint8_t __io_read_8(uint16_t addr)
{
	uint8_t data;
	asm (
		"mov %1, %%dx\n\t"
		"in %%dx, %%al\n\t"
		"mov %%al, %0\n\t"
		:
		:"m"(data), "m"(addr)
	);
	return data;
}
static inline uint16_t __io_read_16(uint16_t addr)
{
	uint16_t data;
	asm (
		"mov %1, %%dx\n\t"
		"in %%dx, %%ax\n\t"
		"mov %%ax, %0\n\t"
		:
		:"m"(data), "m"(addr)
	);
	return data;
}
static inline uint32_t __io_read_32(uint16_t addr)
{
	uint32_t data;
	asm (
		"mov %1, %%dx\n\t"
		"in %%dx, %%eax\n\t"
		"mov %%eax, %0\n\t"
		:
		:"m"(data), "m"(addr)
	);
	return data;
}

#endif
