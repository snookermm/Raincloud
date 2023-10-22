.globl page_fault_handler
divide_error_exception_handler:
	push $0
debug_exception_handler:
	push $1
nmi_handler:
	push $2
breakpoint_exception_handler:
	push $3
overflow_exception_handler:
	push $4
bound_range_exceeded_exception_handler:
	push $5
invalid_opcode_exception_handler:
	push $6
device_not_available_exception_handler:
	push $7
double_fault_handler:
	push $8
coprocessor_segment_overrun_handler:
	push $9
invalid_tss_exception_handler:
	push $10
segment_not_present_handler:
	push $11
stack_fault_exception_handler:
	push $12
general_protection_exception_handler:
	push $13
page_fault_exception_handler:
	push $14
x87_fpu_floating_point_error_handler:
	push $16
alignment_check_exception_handler:
	push $17
machine_check_exception_handler:
	push $18
simd_floating_point_exception_handler:
	push $19
virtualization_exception_handler:
	push $20
