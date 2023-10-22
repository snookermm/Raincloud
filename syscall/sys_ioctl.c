#include <syscall.h>

int sys_ioctl(int fd, int request)
{
	fs_context_t *fs_context = get_fd_ptr(fd);
	if (fs_context == NULL)
		return -1;
	return fs_ioctl(fs_context, request);
}
