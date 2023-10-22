#ifndef _SCHED_H_
#define _SCHED_H_

#include <klibc.h>
#include <proc.h>

#include <queue.h>

proc_t *get_current_proc(void);
thread_t *get_current_thread(void);
queue_t *get_ready_queue(void);
void sched(void);
void sched_init(void);

int fork();
void exit(int status);
void exec(const char *path, char *const argv[]);


#endif

