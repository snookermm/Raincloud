#include <syscall.h>

int sys_open(const char *path, int oflag)
{
	fs_context_t *fs_context = fs_open(path, oflag);
	if (fs_context == NULL)
		return -1;
	int fd = fd_alloc(fs_context);
	return fd;
}
