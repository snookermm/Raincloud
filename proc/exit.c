#include <proc.h>
#include <sched.h>
void exit(int status)
{
	printf("EXIT\n");
	proc_t *current_proc = get_current_proc();
	current_proc->status = -1;
	current_proc->xstat = status;
	//Mark threads stopped
	list_head_t *it;
	list_for_each(it, &current_proc->threads)
	{
		thread_t *entry = list_entry(it, thread_t, list);
		entry->status = -1;
	}
	
	//Modify its children's parent pointer
	list_for_each(it, &current_proc->child)
	{
		proc_t *entry = list_entry(it, proc_t, list);
		entry->parent = current_proc->parent;
	}
	sched();
}
