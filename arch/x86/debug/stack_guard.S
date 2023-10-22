.globl __stack_chk_fail
__stack_chk_fail:
	cli
	lea	str, %rdi
	call	printf
	hlt

str:
.asciz "Stack overflow or underflow!\n"
