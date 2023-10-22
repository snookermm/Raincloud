#include <syscall.h>

int sys_fstat(int fildes, void *buf)
{
	fs_context_t *fs_context = get_fd_ptr(fildes);
	if (fs_context == NULL)
		return -1;
	return fs_fstat(fs_context, buf);
}
 
