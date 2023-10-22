#include <sched.h>
#include <proc.h>

int fork(void)
{
	printf("Fork\n");
	thread_t *current_thread = get_current_thread();
	proc_t *current_proc = get_current_proc();
	queue_t *ready_queue = get_ready_queue();
	
	thread_t *new_thread = malloc(sizeof(thread_t));
	if (new_thread == NULL)
		return -1;
	thread_dup(new_thread, current_thread);
	printf("TID:%d\n",new_thread->tid);
	printf("Thread dup complete\n");
	proc_t *new_proc = malloc(sizeof(proc_t));
	if (new_proc == NULL)
		return -1;
	proc_dup(new_proc, current_proc);
	printf("Proc dup complete\n");
	thread_attach(new_thread, new_proc);
	queue_enqueue(ready_queue, new_thread);
	
	new_thread->tcb.context.RAX = new_proc->pid;
	
	return 0;
}
