/**
  * Dump register when called by a near call
  */
.globl	dump_reg, __dump_reg
dump_reg:
	push	%r15
	push	%r14
	push	%r13
	push	%r12
	push	%r11
	push	%r10
	push	%r9
	push	%r8
	push	%rbp
	push	%rsi
	push	%rdi
	push	%rdx
	push	%rcx
	push	%rbx
	push	%rax
	
	mov	%rsp, %rdi
	call	__dump_reg
	ret
	
