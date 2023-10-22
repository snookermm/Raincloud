#ifndef _FS_GENERIC_H_
#define _FS_GENERIC_H_
#include <klibc.h>
#include <fs.h>
#include <fat.h>
typedef struct fat_context
{
	FATFS_Type *fs;
	FATFile_Type *file;
	off_t offset;
	void *buf;
	int oflag;
}fat_context_t;

void *fat_open(file_node_t *node, const char *path, int oflag);
int fat_close(file_node_t *node, fat_context_t *context);
ssize_t fat_read(file_node_t *node, fat_context_t *context, void *buf, uint64_t size);
off_t fat_lseek(file_node_t *node, fat_context_t *context, off_t offset, int whence);
int fat_mknode(const char *path);

#endif
