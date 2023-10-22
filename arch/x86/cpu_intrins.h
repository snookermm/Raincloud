#ifndef _CPU_H_
#define _CPU_H_

#include <klibc.h>

static inline void __halt__(void)
{
	asm ("hlt");
}
static inline void __enable_interrupt__(void)
{
	asm ("sti");
}
	
static inline void __disable_interrupt__(void)
{
	asm ("cli");
}
static inline uint64_t __get_idt_addr__(void)
{
	uint64_t result[2] = {0};
	
	asm (
		"lea %0, %%rax\r\n"
		"sidt 6(%%rax)"
		: "=m" (result)
		:
	);
	
	return result[1];
}
static inline uint64_t __read_msr__(uint32_t addr)
{
	uint64_t result;
	
	asm (
		"mov %0, %%ecx\r\n"
		"rdmsr\r\n"
		"lea %1, %%rdi\r\n"
		"mov %%eax, (%%rdi)\r\n"
		"mov %%edx, 4(%%rdi)\r\n"
		: "=m" (addr)
		: "m" (result)
	);
	
	return result;
}
static inline void __write_msr__(uint32_t addr, uint64_t data)
{
	asm (
		"mov %0, %%ecx\r\n"
		"lea %1, %%rdi\r\n"
		"mov (%%rdi), %%eax\r\n"
		"mov 4(%%rdi), %%edx\r\n"
		"wrmsr\r\n"
		: 
		: "m" (addr), "m" (data)
	);
}
/**
 * Execute CPUID instruction and return information,
 * with input value EAX
 */
typedef struct cpu_id_info
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
}cpu_id_info;
static inline cpu_id_info __cpuid__(uint32_t eax)
{
	cpu_id_info ret;
	asm (
		"mov %0, %%eax\r\n"
		"cpuid\r\n"
		"lea %1, %%rdi\r\n"
		"mov %%eax, (%%rdi)\r\n"
		"mov %%ebx, 4(%%rdi)\r\n"
		"mov %%ecx, 8(%%rdi)\r\n"
		"mov %%edx, 12(%%rdi)\r\n"
		:
		: "m" (eax), "m" (ret)
	);
	return ret;
}
static inline uint64_t __read_cr3__()
{
	uint64_t cr3;
	asm (
		"mov %%cr3, %%rax\r\n"
		"mov %%rax, %0\r\n"
		: "=m" (cr3)
		:
	);
	return cr3;
}
static inline uint64_t __read_rflags__(void)
{
	uint64_t rflags;
	asm (
		"pushfq\r\n"
		"pop %0\r\n"
		: "=m" (rflags)
		:
	);
	return rflags;	
}
#endif
