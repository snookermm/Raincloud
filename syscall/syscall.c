#include <syscall.h>
#include <klibc.h>
#include <mm.h>
#include <paging.h>
uint64_t *syscall_table = (uint64_t *)SYSCALL_BASE;

void syscall_init(void)
{
	printf("Syscall initializing...\n");
	uint64_t temp = page_alloc(SYS_CALL_TABLE_SIZE);
	assert(temp != INVALID_ADDR);
	modify_chunk_mapping(get_current_plm4e(), SYSCALL_BASE, (uint64_t)temp, SYS_CALL_TABLE_SIZE, DEFAULT_PTE_FLAG);
	memset(syscall_table, 0, SYS_CALL_TABLE_SIZE << 12);
	
	*(syscall_table + SYS_syscall) = (uint64_t)&sys_syscall;
	*(syscall_table + SYS_exit) = (uint64_t)&sys_exit;
	*(syscall_table + SYS_fork) = (uint64_t)&sys_fork;
	*(syscall_table + SYS_read) = (uint64_t)&sys_read;
	*(syscall_table + SYS_write) = (uint64_t)&sys_write;
	*(syscall_table + SYS_open) = (uint64_t)&sys_open;
	*(syscall_table + SYS_close) = (uint64_t)&sys_close;
	*(syscall_table + SYS_exec) = (uint64_t)&sys_exec;
	*(syscall_table + SYS_lseek) = (uint64_t)&sys_lseek;
	*(syscall_table + SYS_fstat) = (uint64_t)&sys_fstat;
	*(syscall_table + SYS_ioctl) = (uint64_t)&sys_ioctl;
	printf("Syscall initializing complete.\n");
}
