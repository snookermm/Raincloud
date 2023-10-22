/**
  * Stack:
  * [  EFLAGS  ]
  * [    CS    ]
  * [    EIP   ]
  * [Error Code]
  */

.globl exception_handler
exception_handler:
	pushq	$0
	jmp	dump
	pushq	$1
	jmp	dump
	pushq	$2
	jmp	dump
	pushq	$3
	jmp	dump
	pushq	$4
	jmp	dump
	pushq	$5
	jmp	dump
	pushq	$6
	jmp	dump
	pushq	$7
	jmp	dump
	pushq	$8
	jmp	dump
	pushq	$9
	jmp	dump
	pushq	$10
	jmp	dump
	pushq	$11
	jmp	dump
	pushq	$12
	jmp	dump
	pushq	$13
	jmp	dump
	pushq	$14
	jmp	dump
	pushq	$15
	jmp	dump
	pushq	$16
	jmp	dump
	pushq	$17
	jmp	dump
	pushq	$18
	jmp	dump
	pushq	$19
	jmp	dump
	pushq	$20
	jmp	dump
	pushq	$21
	jmp	dump
	pushq	$22
	jmp	dump
	pushq	$23
	jmp	dump
	pushq	$24
	jmp	dump
	pushq	$25
	jmp	dump
	pushq	$26
	jmp	dump
	pushq	$27
	jmp	dump
	pushq	$28
	jmp	dump
	pushq	$29
	jmp	dump
	pushq	$30
	jmp	dump
	pushq	$31
	
/*	push	%rax
	push	%rbx
	push	%rcx
	push	%rdx
	push	%rdi
	push	%rsi
	push	%rbp
	push	%r8
	push	%r9
	push	%r10
	push	%r11
	push	%r12
	push	%r13
	push	%r14
	push	%r15
	mov	%rsp, %rdi*/
dump:	call	dump_reg
/* 	call	print_traceback */
	hlt
