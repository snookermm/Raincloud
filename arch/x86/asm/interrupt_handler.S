
.globl systick_handler
systick_handler:
	call	context_save
	call	sched			#Schedule routine
	call	context_load
	iretq				#Automatically load the registers
	
str:.asciz "Hello\n"
.globl syscall_handler, syscall_table, save_context, load_context
syscall_handler:
	cli
	
	call	context_save
	#Load params
	mov	save_context, %rbp
	mov	(%rbp), %rax
	mov	48(%rbp), %rdi
	mov	40(%rbp), %rsi
	mov	16(%rbp), %rdx
	mov	8(%rbp), %rcx
	mov	56(%rbp), %r8
	mov	64(%rbp), %r9
	
	mov	syscall_table, %rbp
	call	*(%rbp, %rax, 8)
	#Set return value
	mov	load_context, %rbp
	mov	%rax, (%rbp)
	
	call	context_load
	sti
	iretq
