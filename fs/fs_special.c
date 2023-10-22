#include <fs_special.h>

void *stdio_open(file_node_t *node, const char *path, int oflag)
{
	return NULL;
}
int stdio_close(file_node_t *node, void *context)
{
	return 0;
}
ssize_t stdio_read(file_node_t *node, void *context, void *buf, uint64_t size)
{
	return 0;
}
ssize_t stdio_write(file_node_t *node, void *context, const void *buf, uint64_t size)
{
	char *temp = malloc(size + 1);
	memcpy(temp, buf, size);
	*(temp + size) = '\0';
	__stdout(temp);
	free(temp);
	return size;
}
int stdio_mknode(const char *path)
{
	int ret;
	ret = fs_mknode(path, FILE_TYPE_SPECIAL);
	if (ret != 0)
	{
		printf("Create special file failed:stdio\n");
		return ret;
	}
	ret = fs_reghook(path, FILE_HOOK_INDEX_OPEN, &stdio_open);
	if (ret != 0)
	{
		printf("Register hook function failed:open\n");
		return ret;
	}
	ret = fs_reghook(path, FILE_HOOK_INDEX_CLOSE, &stdio_close);
	if (ret != 0)
	{
		printf("Register hook function failed:close\n");
		return ret;
	}
	ret = fs_reghook(path, FILE_HOOK_INDEX_READ, &stdio_read);
	if (ret != 0)
	{
		printf("Register hook function failed:read\n");
		return ret;
	}
	ret = fs_reghook(path, FILE_HOOK_INDEX_WRITE, &stdio_write);
	if (ret != 0)
	{
		printf("Register hook function failed:write\n");
		return ret;
	}
	return 0;
}
void *pipe_open(file_node_t *node, const char *path, int oflag)
{
	pipe_context_t *context = malloc(sizeof(pipe_context_t));
	context->buf = malloc(PIPE_BUFFER_SIZE);
	context->write_offset = 0;
	context->read_offset = 0;
	context->length = 0;
	return context;
}
int pipe_close(file_node_t *node, pipe_context_t *context)
{
	free(context->buf);
	free(context);
}

ssize_t pipe_read(file_node_t *node, pipe_context_t *context, void *buf, uint64_t size)
{
	//Compute size to transfer
	size = context->length >= size ? size : context->length;
	context->length -= size;
	for (ssize_t i = 0; i < size; i ++)
	{
		*(char *)buf ++ =  *((char *)context->buf + context->read_offset ++);
		if (context->read_offset == PIPE_BUFFER_SIZE)
			context->read_offset = 0;
	}
	return size;
}
ssize_t pipe_write(file_node_t *node, pipe_context_t *context, const void *buf, uint64_t size)
{
	//Compute size to transfer
	size = PIPE_BUFFER_SIZE - context->length >= size ? size : PIPE_BUFFER_SIZE - context->length;
	context->length += size;
	for (ssize_t i = 0; i < size; i ++)
	{
		*((char *)context->buf + context->write_offset ++) = *(char *)buf ++;
		if (context->write_offset == PIPE_BUFFER_SIZE)
			context->write_offset = 0;
	}
	return size;
}
int pipe_mknode(const char *path)
{
	int ret;
	ret = fs_mknode(path, FILE_TYPE_SPECIAL);
	if (ret != 0)
	{
		printf("Create special file failed:stdio\n");
		return ret;
	}
	ret = fs_reghook(path, FILE_HOOK_INDEX_OPEN, &pipe_open);
	if (ret != 0)
	{
		printf("Register hook function failed:open\n");
		return ret;
	}

	ret = fs_reghook(path, FILE_HOOK_INDEX_CLOSE, &pipe_close);
	if (ret != 0)
	{
		printf("Register hook function failed:close\n");
		return ret;
	}
	ret = fs_reghook(path, FILE_HOOK_INDEX_READ, &pipe_read);
	if (ret != 0)
	{
		printf("Register hook function failed:read\n");
		return ret;
	}
	ret = fs_reghook(path, FILE_HOOK_INDEX_WRITE, &pipe_write);
	if (ret != 0)
	{
		printf("Register hook function failed:write\n");
		return ret;
	}
	return ret;
}

