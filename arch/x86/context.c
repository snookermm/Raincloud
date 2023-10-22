#include <klibc.h>
#include <context.h>

context_t *save_context;
context_t *load_context;

void __context_save(uint64_t *stack_top)
{
	int regnum = sizeof(context_t) >> 3;
	uint64_t *save_location = (uint64_t *)save_context;
	for (int i = 0; i < regnum + 1; i ++)
	{
		if (i == 17)
			stack_top ++;
		else
			*save_location++ = *stack_top++;
	}
}
void __context_load(uint64_t *stack_top)
{
	int regnum = sizeof(context_t) >> 3;
	uint64_t *save_location = (uint64_t *)load_context;
	for (int i = 0; i < regnum + 1; i ++)
	{
		if (i == 17) 
			stack_top++;
		else
			*stack_top++ = *save_location++;
	}
}
