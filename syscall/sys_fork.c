#include <proc.h>
#include <sched.h>
int sys_fork(void)
{
	return fork();
}
