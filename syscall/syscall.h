#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <fs.h>
#include <sched.h>
#include <proc.h>

#define	SYS_syscall	0
#define	SYS_exit	1
#define	SYS_fork	2
#define	SYS_read	3
#define	SYS_write	4
#define	SYS_open	5
#define	SYS_close	6
#define	SYS_wait4	7
#define SYS_exec	11
#define SYS_lseek	19
#define	SYS_pipe	42
#define SYS_ioctl	54
#define SYS_fstat	62


#define SYS_CALL_TABLE_SIZE 1


void sys_syscall(void);
void sys_exit(int status);
int sys_fork(void);
ssize_t sys_read(int fd, void *buf, uint64_t count);
ssize_t sys_write(int fd, const void *buf, size_t count);
int sys_open(const char *path, int oflag);
int sys_close(int fd);
ssize_t sys_write(int fd, const void *buf, size_t count);
void sys_exec(const char *path, char *const argv[]);
off_t sys_lseek(int fildes, off_t offset, int whence);
int sys_ioctl(int fd, int request);
int sys_fstat(int fildes, void *buf);


void syscall_init(void);

#endif
