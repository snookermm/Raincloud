#ifndef _FS_SPECIAL_H_
#define _FS_SPECIAL_H_
#include <klibc.h>
#include <fs.h>

void *stdio_open(file_node_t *node, const char *path, int oflag);
int stdio_close(file_node_t *node, void *context);
ssize_t stdio_read(file_node_t *node, void *context, void *buf, uint64_t size);
ssize_t stdio_write(file_node_t *node, void *context, const void *buf, uint64_t size);
int stdio_mknode(const char *path);


#define PIPE_BUFFER_SIZE	4096

typedef struct pipe_context
{
	void *buf;
	off_t write_offset;
	off_t read_offset;
	size_t length;
}pipe_context_t;

void *pipe_open(file_node_t *node, const char *path, int oflag);
int pipe_close(file_node_t *node, pipe_context_t *context);
ssize_t pipe_read(file_node_t *node, pipe_context_t *context, void *buf, uint64_t size);
ssize_t pipe_write(file_node_t *node, pipe_context_t *context, const void *buf, uint64_t size);
int pipe_mknode(const char *path);

#endif
