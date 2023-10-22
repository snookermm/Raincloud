#include <klibc.h>

void sys_exec(const char *path, char *const argv[])
{
	exec(path, argv);
}
