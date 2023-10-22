#ifndef _FS_H_
#define _FS_H_

#include <list.h>
#include <klibc.h>

#define HOOK_MAX			7

#define FILE_TYPE_ROOT			0x00
#define FILE_TYPE_DIRCTORY		0x01
#define FILE_TYPE_BLOCK_DEVICE		0x02
#define FILE_TYPE_CHARACTER_DEVICE	0x04
#define FILE_TYPE_SOCKET		0x08
#define FILE_TYPE_MOUNT_POINT		0x10
#define FILE_TYPE_SPECIAL		0x20

#define FILE_HOOK_INDEX_OPEN	0
#define FILE_HOOK_INDEX_CLOSE	1
#define FILE_HOOK_INDEX_READ	2
#define FILE_HOOK_INDEX_WRITE	3
#define FILE_HOOK_INDEX_LSEEK	4
#define FILE_HOOK_INDEX_IOCTL	5
#define FILE_HOOK_INDEX_FSTAT	6

#define FILE_LSEEK_WHENCE_SEEK_SET	0
#define FILE_LSEEK_WHENCE_SEEK_CUR	1
#define FILE_LSEEK_WHENCE_SEEK_END	2
typedef struct file_node
{
	char *name;
	uint64_t type;
	
	struct list_head list;
	struct list_head children;
	struct file_node *parent;	
	struct file_hook *hook;
}file_node_t;

typedef struct file_hook
{
	void *(*open)(file_node_t *node, const char *path, int oflag);
	int (*close)(file_node_t *node, void *context);
	ssize_t (*read)(file_node_t *node, void *context, void *buf, uint64_t count);
	ssize_t (*write)(file_node_t *node, void *context, const void *buf, uint64_t count);
	off_t (*lseek)(file_node_t *node, void *context, off_t offset, int whence);
	int (*ioctl)(file_node_t *node, void *context, int request);
	int (*fstat)(file_node_t *node, void *context, void *buf);
}file_hook_t;

typedef struct fs_context
{
	file_node_t *node;
	void *context;
}fs_context_t;


void fs_init(void);
int fs_mknode(const char *path, uint64_t type);
int fs_rmnode(const char *path);
file_node_t *fs_getnode(const char *path);

int fs_reghook(const char *path, int hook_index, void *hook);


fs_context_t *fs_open(const char *path, int oflag);
int fs_close(fs_context_t *context);
ssize_t fs_read(fs_context_t *context, void *buf, uint64_t count);
ssize_t fs_write(fs_context_t *context, const void *buf, uint64_t count);
off_t fs_lseek(fs_context_t *context, off_t offset, int whence);
int fs_ioctl(fs_context_t *context, int request);
int fs_fstat(fs_context_t *context, void *buf);


#endif
