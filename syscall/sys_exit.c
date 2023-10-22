#include <syscall.h>

void sys_exit(int status)
{
	exit(status);
}
