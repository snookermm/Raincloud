/**
  * Entry to main.
  * We initialize __stdout here
  */
.globl _start
_start:
	mov	72(%rdi), %rax
	mov	%rax, __stdout
/*  	mov	kernel_image_base_addr, %rax */
/* 	mov	(%rax), %rax */
/* 	mov	kernel_image_base_addr, %rax */
/* 	pop	(mm_map) */
/* 	mov	$0xffffffff80000000, %rax */
/* 	add	$0x0400, %rax */
/* 	hlt */
/* 	jmp	*%rax */
/*	add	$main, %rax*/
/*  	jmp	*%rax  */
	jmp	main
/* .globl mm_map_size,mm_map_addr */
/* mm_map_size:.quad 0 */
/* mm_map_addr:.quad 0 */
.globl	__stdout
__stdout:.quad 0

.section .zero
