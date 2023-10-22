#include <klibc.h>

asm 
(
	".globl	dump_reg\r\n"
	"dump_reg:\r\n"
	"push	%r15\r\n"
	"push	%r14\r\n"
	"push	%r13\r\n"
	"push	%r12\r\n"
	"push	%r11\r\n"
	"push	%r10\r\n"
	"push	%r9\r\n"
	"push	%r8\r\n"
	"push	%rbp\r\n"
	"push	%rsi\r\n"
	"push	%rdi\r\n"
	"push	%rdx\r\n"
	"push	%rcx\r\n"
	"push	%rbx\r\n"
	"push	%rax\r\n"
	"mov	%rsp, %rdi\r\n"
	"call	__dump_reg\r\n"
	"hlt\r\n"
);
const char *exception_type[] = {
	"Divide Error Exception",
	"Debug Exception",
	"NMI Interrupt",
	"Breakpoint Exception",
	"Overflow Exception",
	"BOUND Range Exceeded Exception",
	"Invalid Opcode Exception",
	"Device Not Available Exception",
	"Double Fault Exception",
	"Coprocessor Segment Overrun",
	"Invalid TSS Exception",
	"Segment Not Present",
	"Stack Fault Exception",
	"General Protection Exception",
	"Page-Fault Exception",
	"",
	"x87 FPU Floating-Point Error",
	"Alignment Check Exception",
	"Machine-Check Exception",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
};

extern char __INIT_START[];
void __dump_reg(uint64_t *reg_stack)
{
	uint64_t type = *(reg_stack + 16);
	if (type <= 20)
		printf("%s\n", (char *)((uint64_t)exception_type[type] + (uint64_t)__INIT_START));
	
	printf("RAX\t\t=\t%lx\n", *reg_stack++);
	printf("RBX\t\t=\t%lx\n", *reg_stack++);
	printf("RCX\t\t=\t%lx\n", *reg_stack++);
	printf("RDX\t\t=\t%lx\n", *reg_stack++);
	printf("RDI\t\t=\t%lx\n", *reg_stack++);
	printf("RSI\t\t=\t%lx\n", *reg_stack++);
	printf("RBP\t\t=\t%lx\n", *reg_stack++);
	printf("R8\t\t=\t%lx\n", *reg_stack++);
	printf("R9\t\t=\t%lx\n", *reg_stack++);
	printf("R10\t\t=\t%lx\n", *reg_stack++);
	printf("R11\t\t=\t%lx\n", *reg_stack++);
	printf("R12\t\t=\t%lx\n", *reg_stack++);
	printf("R13\t\t=\t%lx\n", *reg_stack++);
	printf("R14\t\t=\t%lx\n", *reg_stack++);
	printf("R15\t\t=\t%lx\n", *reg_stack++);
	reg_stack++;
	printf("Exception Type\t=\t%lx\n",  *reg_stack++);
	printf("Error Code\t= \t%lx\n", *reg_stack++);
	printf("RIP\t\t=\t%lx\n", *reg_stack++);
	printf("CS\t\t=\t%lx\n", *reg_stack++);
	printf("RFLAGS\t\t=\t%lx\n", *reg_stack++);
	printf("RSP\t\t=\t%lx\n", *reg_stack++);
	printf("SS\t\t=\t%lx\n", *reg_stack++);
	uint64_t buffer[2];
	asm (
		"lea	%0, %%rax\r\n"
		"sgdt	6(%%rax)\r\n"
		:
		: "m" (buffer)
	);
	printf("GDTR\t\t=\t%lx %lx\n", buffer[1], buffer[0] >> 48);
	asm (
		"lea	%0, %%rax\r\n"
		"sldt	6(%%rax)\r\n"
		:
		: "m" (buffer)
	);
	printf("LDTR\t\t=\t%lx %lx\n", buffer[1], buffer[0] >> 48);
	asm (
		"lea	%0, %%rax\r\n"
		"sidt	6(%%rax)\r\n"
		:
		: "m" (buffer)
	);
	printf("IDTR\t\t=\t%lx %lx\n", buffer[1], buffer[0] >> 48);
}
