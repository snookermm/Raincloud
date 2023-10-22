#include <cpu_intrins.h>
#include <paging.h>
#include <segments.h>
#include <int_ctl.h>
#include <io.h>
#include <isa.h>

int int_reg(uint8_t vector, void *handle, enum INT_TYPE type);
void init_int_stack(void);
void systick_handler(void);
void syscall_handler(void);

int int_init(void)
{
	__disable_interrupt__();
	init_int_stack();
	
	int_reg(0x20, &systick_handler, INT);
	timer_82c54_init(0, 2, 0xffff);
	pic_8259_init(0x20);
// 	pic_8259_unmask_int(0);
	
	int_reg(0x80, &syscall_handler, TRAP);
	
	__enable_interrupt__();
}
void init_int_stack(void)
{
	uint64_t stack_phy = page_alloc(0x10);
	uint64_t stack_top_virt = KERNEL_STACK_TOP - 0x100000;
	uint64_t stack_base_virt = stack_top_virt - (0x10 << 12);
	modify_chunk_mapping(get_current_plm4e(), stack_base_virt, stack_phy, 0x10, DEFAULT_PTE_FLAG);
	((struct tss_seg *)(TSS_BASE + PHY_MAP_BASE))->RSP0_lower = stack_top_virt;
	((struct tss_seg *)(TSS_BASE + PHY_MAP_BASE))->RSP0_upper = stack_top_virt >> 32;
}
int int_reg(uint8_t vector, void *handle, enum INT_TYPE type)
{
	struct int_trap_seg_dsc *dsc = (struct int_trap_seg_dsc *)__get_idt_addr__() + vector;
	struct seg_sel seg_dsc = 
	{
		.rpl = 0,
		.ti = 0,
		.index = 5,
	};
	dsc->offset_0_15 = (uint64_t)(handle) & 0xffff;
	dsc->seg_sel = seg_dsc;
	dsc->ist = 0;
	dsc->zero_1 = 0;
	switch(type)
	{
		case 0:
			dsc->type = DSC_TYPE_INIT;
			break;
		case 1:
			dsc->type = DSC_TYPE_TRAP;
			break;
		default:
			bug("Wrong INT_TYPE specified %d\n", type);
			break;
	}
	dsc->zero_2 = 0;
	dsc->dpl = 3;
	dsc->p = 1;
	dsc->offset_16_31 = ((uint64_t)(handle) >> 16) & 0xffff;
	dsc->offset_32_63 = ((uint64_t)(handle) >> 32) & 0xffffffff;
}

int int_unreg(uint8_t vector)
{
	struct int_trap_seg_dsc *dsc = (struct int_trap_seg_dsc *)__get_idt_addr__() + vector;
	dsc->p = 0;
}
