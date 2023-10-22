#include <syscall.h>
#include <klibc.h>
#include <mm.h>
#include <paging.h>

uint64_t *syscall_table = (uint64_t *)SYSCALL_BASE;

void set_syscall(uint64_t syscall_number, void *function_ptr) {
    if (syscall_number >= SYS_CALL_TABLE_SIZE) {
        return;
    }
    syscall_table[syscall_number] = (uint64_t)function_ptr;
}

void syscall_init(void) {
    printf("Syscall initializing...\n");

    uint64_t *syscall_table_memory = (uint64_t *)page_alloc(SYS_CALL_TABLE_SIZE);
    assert(syscall_table_memory != INVALID_ADDR);

    modify_chunk_mapping(get_current_plm4e(), SYSCALL_BASE, (uint64_t)syscall_table_memory, SYS_CALL_TABLE_SIZE, DEFAULT_PTE_FLAG);

    memset(syscall_table_memory, 0, SYS_CALL_TABLE_SIZE * PAGE_SIZE);

    set_syscall(SYS_syscall, &sys_syscall);
    set_syscall(SYS_exit, &sys_exit);
    set_syscall(SYS_fork, &sys_fork);
    set_syscall(SYS_read, &sys_read);
    set_syscall(SYS_write, &sys_write);
    set_syscall(SYS_open, &sys_open);
    set_syscall(SYS_close, &sys_close);
    set_syscall(SYS_exec, &sys_exec);
    set_syscall(SYS_lseek, &sys_lseek);
    set_syscall(SYS_fstat, &sys_fstat);
    set_syscall(SYS_ioctl, &sys_ioctl);

    printf("Syscall initializing complete.\n");
}
