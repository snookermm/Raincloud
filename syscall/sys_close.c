#include <syscall.h>

int sys_close(int fd)
{
	fs_context_t *fs_context = get_fd_ptr(fd);
	if (fs_context == NULL)
		return -1;
	return fs_close(fs_context);
}
