#include <sched.h>
#include <proc.h>
#include <list.h>
#include <queue.h>
#include <mm.h>
#include <paging.h>
#include <id.h>
list_head_t thread_list;
list_head_t proc_list;

thread_t *current_thread;
proc_t *current_proc;

queue_t *ready_queue;

proc_t *get_current_proc(void)
{
	return current_proc;
}
thread_t *get_current_thread(void)
{
	return current_thread;
}
queue_t *get_ready_queue(void)
{
	return ready_queue;
}
void sched_init(void)
{
	//Init global variable
	current_proc = NULL;
	
	list_init(&thread_list);
	list_init(&proc_list);
	//Init ready_queue
	ready_queue = malloc(sizeof(queue_t));
	queue_init(ready_queue);
	
	//Create default thread,at least we have one thread running
	thread_t *initial_kernel_thread = malloc(sizeof(thread_t));
	current_thread = initial_kernel_thread;
	initial_kernel_thread->tid = tid_alloc();
	list_add_before(&thread_list, &initial_kernel_thread->list);
	initial_kernel_thread->proc = NULL;
	initial_kernel_thread->status = 0;
	memset(&initial_kernel_thread->tcb, 0, sizeof(tcb_t));
	
	//Insert default thread into ready_queue
	queue_enqueue(ready_queue, initial_kernel_thread);
	//Init context loading and saveing pointer
	save_context = &initial_kernel_thread->tcb.context;
	load_context = &initial_kernel_thread->tcb.context;
	
	printf("Scheduler initializing complete.\n");
	
// 	void thread1(void);
// 	void thread2(void);
// 	initial_kernel_thread = malloc(sizeof(thread_t));
// 	uint64_t stack = (uint64_t)malloc(4096);
// 	thread_init(initial_kernel_thread, NULL, &thread1, stack + 4096);
// 	queue_enqueue(ready_queue, initial_kernel_thread);
// 	
// 	initial_kernel_thread = malloc(sizeof(thread_t));
// 	stack = (uint64_t)malloc(4096);
// 	thread_init(initial_kernel_thread, NULL, &thread2, stack + 4096);
// 	queue_enqueue(ready_queue, initial_kernel_thread);
}
// uint64_t val;
// void __sched(void)
// {
// 	static uint8_t count = 0;
// 	val = 0;
// // 	if (count ++ % 10 != 0)
// // 		return;
// // 	printk("%d\n", val);
// 	
// 	sched();
// }
/**
 * This function is called after context is saved,
 * then we can schedule.
 */
void sched(void)
{
// 	printf("Sched\n");
	
	thread_t *load_thread;
	do
	{
		load_thread = queue_dequeue(ready_queue);
		assert(load_thread != NULL);
		if (load_thread->status == -1)
			printf("Deleted in ready queue Thread : %ld\n", load_thread->tid);
		
	}while(load_thread->status == -1);
	
	if (load_thread->tid != 0)
		printf("Schedule to Thread : %ld\n", load_thread->tid);
	
	load_context = &load_thread->tcb.context;
	save_context = &load_thread->tcb.context;
	
	queue_enqueue(ready_queue, load_thread);
	current_thread = load_thread;
	current_proc = load_thread->proc;
	if (current_proc != NULL)
	{
		printf("Changed mapping\n");
		vm_map_do_mapping(current_proc->vm_map);
	}
}
