#include <klibc.h>
#include <mm.h>
#include <fat.h>
#include <sched.h>
#include <syscall.h>
#include <int_ctl.h>
#include <queue.h>
#include <pci.h>
#include <fs.h>
#include <fs_generic.h>
#include <GlyphBDF.h>
#include <PotableGrayMap.h>
#include <pci.h>
#include <paging.h>
#include <id.h>
#include <sched.h>
#include <syscall.h>
#include <int_ctl.h>

/**
 * 
 * This is the routine of the kernel
 * called by bootloader.
 * 
 */
void main(uint64_t *param_list)
{
	printf("\nStarting kernel...\n");
	for (int i = 0; i < 10; i ++)
		printf("%lx\n", *(param_list + i));
	
	printf("text_start\t=\t%lx\n", __text_start);
	printf("text_end\t=\t%lx\n", __text_end);
	printf("data_start\t=\t%lx\n", __data_start);
	printf("data_end\t=\t%lx\n", __data_end);
	printf("bss_start\t=\t%lx\n", __bss_start);
	printf("bss_end\t\t=\t%lx\n", __bss_end);
	
	mm_init(param_list[2], param_list[3], param_list[4], param_list[5] + param_list[6] - PHY_MAP_BASE);
	vm_init();
	pci_init();
	DisplayInit(&param_list[9]);
	id_init();
	sched_init();
	syscall_init();
	int_init();
	fs_init();

	exec("/test", NULL);
	printf("Dead loop.\n");
	//Unmask irq0 timer interrupt 
	pic_8259_unmask_int(0);
	while(1)
	{
// 		printk("Hello,wolrd : %d\n", i++);
	}
}
extern uint64_t val;
void thread1(void)
{

	while(1)
	{
// 		asm(
// 			"mov $0, %rax\r\n"
// 			"int $0x80"
// 		);
		printf("This is thread1.\n");
	}

} 
void thread2(void)
{
	while(1)
	{
		printf("This is thread2.\n");
	}
}
