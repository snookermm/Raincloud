
#include <cpu_intrins.h>
#include <cpu_configure.h>
#include <io.h>
void init_apic(void)
{
	__io_write_8(0x20, 1 << 4 | 1);
	__io_write_8(0x21, 0x20);
	__io_write_8(0x21, 0);
	__io_write_8(0x21,  1 << 1|1);
	
	
	__io_write_8(0x43, 0 << 6 | 3 << 4 | 2 << 1 | 0);
	__io_write_8(0x40, 0xff);
	__io_write_8(0x40, 0xff);
	
// 	cpu_id_info ret = __cpuid__(0);
// 	printf("%x %x %x %x\n", ret.eax, ret.ebx, ret.ecx, ret.edx);
// 	__write_msr__(0x1b, __read_msr__(0x1b) & ~(1 << 11));
// 	printf("%lx\n", __read_msr__(0x1b));
// 	__write_msr__(0x1b, __read_msr__(0x1b) | 1 << 11);
// 	printf("%lx\n", __read_msr__(0x1b));
// // 	
// 	uint32_t *reg;
// 	reg = (uint32_t *)0xfee000f0;
// 	printf("%x\n", *reg);
// 	*reg = *reg | 1 << 8;
// 	printf("%x\n", *reg);
// // 	
// 	reg = (uint32_t *)0xfee00380;
// 	*reg = 0xffff;
// 	__enable_cpu_acpi_timer(acpi_timer_mode_periodic, 0x30);
// 	__set_cpu_acpi_timer(0xfffffff, 6);
// 
// 	reg = (uint32_t *)0xfee00280;
// 	printf("%x\n", *reg);
// 	apic_timer_init(10000);
// 	printf("Initialzing complete.\n");
// 	reg = (uint32_t *)0xfee0390;
// 	while(1)
// 	{
// 		printf("%x\n", *reg);
// 	}
}
#define apic 0xFEE00000
#define APIC_APICID  0x20
#define APIC_APICVER     0x30
#define APIC_TASKPRIOR   0x80
#define APIC_EOI     0x0B0
#define APIC_LDR     0x0D0
#define APIC_DFR     0x0E0
#define APIC_SPURIOUS    0x0F0
#define APIC_ESR     0x280
#define APIC_ICRL    0x300
#define APIC_ICRH    0x310
#define APIC_LVT_TMR     0x320
#define APIC_LVT_PERF    0x340
#define APIC_LVT_LINT0   0x350
#define APIC_LVT_LINT1   0x360
#define APIC_LVT_ERR     0x370
#define APIC_TMRINITCNT  0x380
#define APIC_TMRCURRCNT  0x390
#define APIC_TMRDIV  0x3E0
#define APIC_LAST    0x38F
#define APIC_DISABLE     0x10000
#define APIC_SW_ENABLE   0x100
#define APIC_CPUFOCUS    0x200
#define APIC_NMI    (4<<8)
#define TMR_PERIODIC     0x20000
#define TMR_BASEDIV  (1<<20)
#define APIV_SW_ENABLE (1<<8)
void isr_dummytmr(void)
{
	*(uint64_t *)(apic + APIC_EOI) = 0;
	asm ("pop %rax\r\niretq\r\n");
}
void isr_spurious(void)
{
	asm("pop %rax\r\niretq\r\n");
}
void apic_timer_init(uint32_t quantum){
    uint32_t tmp, cpubusfreq;

//     //set up isrs
//     writegate(32,isr_dummytmr);
//     writegate(39,isr_spurious);
    int_reg(32, &isr_dummytmr, 0);
    int_reg(39, &isr_spurious, 0);

    //initialize LAPIC to a well known state
    *(uint32_t*)(apic+APIC_DFR)=0xFFFFFFFF;
    *(uint32_t*)(apic+APIC_LDR)=((*(uint32_t*)(apic+APIC_LDR)&0x00FFFFFF)|1);
    *(uint32_t*)(apic+APIC_LVT_TMR)=APIC_DISABLE;
    *(uint32_t*)(apic+APIC_LVT_PERF)=APIC_NMI;
    *(uint32_t*)(apic+APIC_LVT_LINT0)=APIC_DISABLE;
    *(uint32_t*)(apic+APIC_LVT_LINT1)=APIC_DISABLE;
    *(uint32_t*)(apic+APIC_TASKPRIOR)=0;

    //okay, now we can enable APIC
    //global enable
//     cpuSetAPICBase(cpuGetAPICBase());
    //software enable, map spurious interrupt to dummy isr
    *(uint32_t*)(apic+APIC_SPURIOUS)=39|APIV_SW_ENABLE;
    //map APIC timer to an interrupt, and by that enable it in one-shot mode
    *(uint32_t*)(apic+APIC_LVT_TMR)=32;
    //set up divide value to 16
    *(uint32_t*)(apic+APIC_TMRDIV)=0x03;

    //initialize PIT Ch 2 in one-shot mode
    //waiting 1 sec could slow down boot time considerably,
    //so we'll wait 1/100 sec, and multiply the counted ticks
    __io_write_8(0x61,(__io_read_8(0x61)&0xFD)|1);
    __io_write_8(0x43,0xB2);
    //1193180/100 Hz = 11931 = 2e9bh
    __io_write_8(0x42,0x9B);    //LSB
    __io_read_8(0x60);   //short delay
    __io_write_8(0x42,0x2E);    //MSB

    //reset PIT one-shot counter (start counting)
    tmp=__io_read_8(0x61)&0xFE;
    __io_write_8(0x61,(uint8_t)tmp);      //gate low
    __io_write_8(0x61,(uint8_t)tmp|1);        //gate high
    //reset APIC timer (set counter to -1)
    *(uint32_t*)(apic+APIC_TMRINITCNT)=0xFFFFFFFF;

    //now wait until PIT counter reaches zero
    while(!(__io_read_8(0x61)&0x20));

    //stop APIC timer
    *(uint32_t*)(apic+APIC_LVT_TMR)=APIC_DISABLE;

    //now do the math...
    cpubusfreq=((0xFFFFFFFF-*(uint32_t*)(apic+APIC_TMRINITCNT))+1)*16*100;
    tmp=cpubusfreq/quantum/16;

    //sanity check, now tmp holds appropriate number of ticks, use it as APIC timer counter initializer
    *(uint32_t*)(apic+APIC_TMRINITCNT)=(tmp<16?16:tmp);
    //finally re-enable timer in periodic mode
    *(uint32_t*)(apic+APIC_LVT_TMR)=32|TMR_PERIODIC;
    //setting divide value register again not needed by the manuals
    //although I have found buggy hardware that required it
    *(uint32_t*)(apic+APIC_TMRDIV)=0x03;
}
