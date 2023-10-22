/**
  * Parameter passing by stack:
  * [         Image loading address          ]
  * [             Image size                 ]
  * [       UEFI memory map desc address     ]
  * [       UEFI memory map desc size        ]
  * [     UEFI memory map desc total size    ]
  *
  * 
  * After this,memory dialog looks like :
  * [Kernel Image] [UEFI Memory Descriptor Array] [Page tables]
  * All region boundary is aligned to 4KB boundary.
  * Kernel image consist of two parts:
  * Init: 
  * 	Initializing page tables and switch into virtual memory space,and execute kernel in the end.
  * Kernel:
  * 	The main body of kernel.
 **/
 KERNEL_ADDR_BASE = 0xffffffff80000000
 KERNEL_STACK_TOP = 0xfffff10000000000
 PHY_MAP_BASE     = 0xffff800000000000
 
.globl _init,__INIT_END, __stdout
_init:
	call	early_print_init
	
	mov	(%rsp), %rbp
	popq	image_base(%rbp)
	popq	image_size(%rbp)
	popq	mm_desc_addr(%rbp)
	popq	mm_desc_size(%rbp)
	popq	mm_desc_total(%rbp)
	push	%rbp
	
	call	init_mm_map
	call	load_mm_map
	call	init_dsc_tables
	call	load_dsc_tables_and_seg_sels
	
#	Adjust parameter list
#	Set up base address register %rbp first
	popq	%rbp
	mov	$PHY_MAP_BASE, %rbx
	
	mov	image_base(%rbp), %rax
	add	%rbx, %rax
	mov	%rax, image_base(%rbp)
	
	mov	mm_desc_addr(%rbp), %rax
	add	%rbx, %rax
	mov	%rax, mm_desc_addr(%rbp)
	
	mov	page_table_base(%rbp), %rax
	add	%rbx, %rax
	mov	%rax, page_table_base(%rbp)
	
	mov	stack_page_base(%rbp), %rax
	addq	%rbx, %rax
	mov	%rax, stack_page_base(%rbp)
	
	mov	$KERNEL_ADDR_BASE, %rbx
	mov	__stdout(%rbp), %rax
	add	%rbx, %rax
	sub	%rbp, %rax
	mov	%rax, __stdout(%rbp)
	
	lea	param_list, %rdi
	add	%rbx, %rdi
	
	mov	$KERNEL_STACK_TOP, %rsp
	lea	__INIT_END, %rax
	add	%rbx, %rax
	jmp	*%rax
	
.globl	image_base, image_size, mm_desc_addr, mm_desc_size, mm_desc_total, page_table_base, page_table_size, stack_page_base, stack_pages, __stdout
param_list:
image_base:.quad 0
image_size:.quad 0
mm_desc_addr:.quad 0
mm_desc_size:.quad 0
mm_desc_total:.quad 0
page_table_base:.quad 0
page_table_size:.quad 0
stack_page_base:.quad 0
stack_pages:.quad 0x10
__stdout:.quad 0

.section .zero
