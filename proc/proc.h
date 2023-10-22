#ifndef _PROC_H_
#define _PROC_H_

#include <ktypes.h>
// #include <mutex.h>
#include <fs.h>
#include <vm.h>
#include <context.h>

#define KERNEL_STACK_SIZE	0x4000
#define FD_TABLE_SIZE		0x0100
typedef struct pargs
{
	int argc;
	char **argv;
}pargs_t;
typedef struct penvs
{
	char *pwd;
}penvs_t;

/**
 * Thread control blocks definitions,
 * including registers and 
 */
typedef struct tcb
{
	context_t 		context;//Registers content
	size_t 			stack_size;//Size of the user stack
	uint64_t		kernel_stack;//Kernel stack top, the size if fixed
	uint64_t		entry_point;//Entry point of the thread
	
}tcb_t;
/*
 * Kernel runnable context (thread).
 * This is what is put to sleep and reactivated.
 * Thread context.  Processes may have multiple threads.
 */
typedef struct thread
{
	id_t			tid;	//Thread ID
	long			status;	//Thread status:-1 stopped 0 pending 1 runnable
	struct list_head	list;	//List of all threads
	struct proc		*proc;	//Process it belongs to
// 	struct mutex		mutex;	//Thread struct lock

	tcb_t			tcb;	//Thread control blocks(machine-dependent)
	
}thread_t;

/**
 * Process structure.
 */

typedef struct proc
{
	id_t			pid;	//Process ID
	struct list_head	list;	//List of all processes
	struct list_head	threads;//Thread list entry
// 	struct mutex		mutex;	//Process struct lock

	struct proc		*parent;//Parent Process
	struct list_head	child;	//Child Process list entry

	int64_t			status;	//Process status: -1 stopped 0 peding 1 runnable
	nice_t	 		nice;	//Nice of the process
	pargs_t			*args;	//Arguments from command line
	penvs_t			*envs;	//Current working directory

	vm_map_t		*vm_map;//Virtual memory map
	
	void*			fd_table[FD_TABLE_SIZE];//File descriptor table
	uint8_t 		xstat;	//Exit status
}proc_t;
// /*
//  * Process groups may have a or more processes.
//  * Each process associates with a process groups.
//  */
// struct pgrp
// {
// 	id_t			id;	//Group ID
// 	struct list_head	list;	//List of all process groups
// 	struct list_head	procs;	//Process struct entry
// 
// };
// /*
//  * Session consists of a set of process groups,
//  * associating with a user and maybe a terminal device.
//  */
// struct session
// {
// 	id_t			id;	//Session ID
// 	struct list_head	list;	//List of all sessions
// 	struct list_head	grps;	//Group struct entry
// 
// };
// 
// struct zone
// {
// 	id_t			id;	//Zone
// 	struct list_head	list;	//List of all namespaces
// 
// };

int fd_alloc(void *ptr);
void fd_free(int fd);
void *get_fd_ptr(int fd);

int thread_init(thread_t *thread, proc_t *proc, uint64_t entrypoint, uint64_t stack_top);
void thread_dup(thread_t *new,  thread_t *old);
void thread_attach(thread_t *thread, proc_t *proc);

int proc_init(proc_t *proc, proc_t *parent, pargs_t *args, penvs_t *envs, vm_map_t *vm_map);
void proc_dup(proc_t *new, proc_t *old);

#endif
