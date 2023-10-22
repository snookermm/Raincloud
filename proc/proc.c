#include <proc.h>
#include <list.h>
#include <sched.h>
#include <id.h>

void pargs_dup(pargs_t *new, pargs_t *old)
{
	new->argv = malloc(old->argc * sizeof(char *));
	for (int i = 0; i < old->argc; i ++)
	{
		new->argv[i] = malloc(strlen(old->argv[i]) + 1);
		strcpy(new->argv[i], old->argv[i]);
	}
}
void penvs_dup(penvs_t *new, penvs_t *old)
{
	new->pwd = malloc(strlen(old->pwd) + 1);
	strcpy(new->pwd, old->pwd);
}
void fd_table_dup(void *new, void *old)
{
	memcpy(new, old, sizeof(void*) * FD_TABLE_SIZE);
}
int proc_init(proc_t *proc, proc_t *parent, pargs_t *args, penvs_t *envs, vm_map_t *vm_map)
{
	proc->pid = pid_alloc();
	list_init(&proc->list);
	list_init(&proc->threads);
	list_init(&proc->child);
	proc->parent = parent;
	proc->nice = 0;
	proc->status = 0;
	proc->args = args;
	proc->envs = envs;
	proc->vm_map = vm_map;
	if (parent != NULL)
	{
		memcpy(proc->fd_table, parent->fd_table, FD_TABLE_SIZE * sizeof(void *));
		list_add_before(&parent->child, &proc->list);
	}
	else
	{
		memset(proc->fd_table, 0, FD_TABLE_SIZE * sizeof(void *));
	}
}
void proc_destroy(proc_t *proc)
{
	assert(proc->parent != NULL);
	pid_free(proc->pid);
	
	list_head_t *it;
	if (!list_empty(&proc->child))
	{
		list_for_each(it, &proc->child)
		{
			proc_t *child_proc = list_entry(it, proc_t, list);
			child_proc->parent = proc->parent;
			list_del(&child_proc->list);
			list_add_before(&proc->parent->child, &child_proc->list);
			
		}
	}
}
/**
 * duplicate proc structure except its id, threads and virtual memory map,
 * making it a child of original process
 * 
 */
void proc_dup(proc_t *new, proc_t *old)
{
	pargs_t *args = malloc(sizeof(pargs_t));
	pargs_dup(args, old->args);
	penvs_t *envs = malloc(sizeof(penvs_t));
	penvs_dup(envs, old->envs);
	vm_map_t *vm_map = malloc(sizeof(vm_map_t));
	vm_map_dup(vm_map, old->vm_map);
	proc_init(new, old, args, envs, vm_map);
}
int fd_alloc(void *ptr)
{
	assert(ptr != NULL);
	proc_t *proc = get_current_proc();
	int i;
	for (i = 0; i < FD_TABLE_SIZE; i ++)
	{
		if (proc->fd_table[i] == NULL)
		{
			proc->fd_table[i] = ptr;
			return i;
		}
	}
	return -1;
}
void fd_free(int fd)
{
	assert(fd >= 0 && fd < FD_TABLE_SIZE);
	proc_t *proc = get_current_proc();
	proc->fd_table[fd] = NULL;
}
void *get_fd_ptr(int fd)
{
	assert(fd >= 0 && fd < FD_TABLE_SIZE);
	proc_t *proc = get_current_proc();
	return proc->fd_table[fd];
}
