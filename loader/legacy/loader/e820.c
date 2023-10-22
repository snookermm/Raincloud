#include <e820.h>
#include <BIOS_interrupt.h>

mm_desc_t mm_desc[100] = {0};

#define BIOS_SIGNATURE ('S' << 24 | 'M' << 16 | 'A' << 8 | 'P')

void e820_query_memory_map(void)
{
	reg32 regs;
	int i = 0;
	regs.ebx = 0;
	do {
		regs.eax = 0xE820;
		regs.es = 0x1000;
		regs.edi = &mm_desc[i];
		regs.ecx = sizeof(mm_desc_t);
		regs.edx = BIOS_SIGNATURE;
		bios_int16(&regs,15);
		
		if (regs.eflags & EFLAG_CARRY || regs.eax != BIOS_SIGNATURE)
		{
			break;
		}
		
		if (regs.ecx < 20 || regs.ecx > sizeof(mm_desc))
		{
			;
		}
		
	} while(regs.ebx != 0);
}
