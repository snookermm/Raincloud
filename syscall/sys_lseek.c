#include <syscall.h>

off_t sys_lseek(int fildes, off_t offset, int whence)
{
	fs_context_t *fs_context = get_fd_ptr(fildes);
	if (fs_context == NULL)
		return -1;
	return fs_lseek(fs_context, offset, whence);
}
