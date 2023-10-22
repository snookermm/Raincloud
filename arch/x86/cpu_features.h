#ifndef _CPU_FEATURES_H_
#define _CPU_FEATURES_H_

static inline uint64_t get_cpu_physical_address_width(void)
{
	uint64_t result; 
	asm(	"mov $0x80000008, %%eax\n\t"
		"cpuid\n\t"
		"mov %%eax, %0\n\t"
		:"=m" (result)
		:
	);
	return result & 0xff;
}
static inline uint64_t get_cpu_linear_address_width(void)
{
	uint64_t result; 
	asm(	"mov $0x80000008, %%eax\n\t"
		"cpuid\n\t"
		"mov %%eax, %0\n\t"
		:"=m" (result)
		:
	);
	return result >> 8;
}

#endif
