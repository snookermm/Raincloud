#ifndef _MUTEX_H_
#define _MUTEX_H_
#include <types.h>
#include <proc.h>
#include <list.h>
struct mutex
{
	/*-1:unlocked,0:locked,positive:locked,waiters*/
	atomic_t 		status;
	struct list_head	wait_list;
	struct thread		*owner;
};

struct wait_list
{
	struct list_head	list;
	struct thread		*thread;
};
/**
 * Init mutex
 */
static inline void mutex_init(struct mutex *lock)
{
	atomic_set(&lock->status, -1);
	list_init(&lock->wait_list);
}
/**
 * Sleep until the mutex is available.
 */
static inline void wait_for_lock(struct mutex *lock)
{
	if (atomic_read(&lock->status) != -1)
	{
		add_current_wait_list();
	}
	while(lock->wait_list.thread != current_thread)
	{
		sched();
	}
	delete_to_wait_list();
}
/**
 * Set current thread to be the owner.
 */
static inline void mutex_set_owner(struct mutex *lock)
{
	lock->owner = current_thread();
}
/**
 * Clear current owner.
 */
static inline void mutex_clear_owner(struct mutex *lock)
{
	lock->owner = NULL;
}
/**
 * Lock mutex.If it's locked already,then blocks
 * until it's unlocked and lock.
 */
static inline void mutex_lock(struct mutex *lock)
{
	wait_for_lock(lock);
	__mutex_lock(lock);
	mutex_set_owner(lock);
}
/**
 * Test whether mutex is locked.
 */
static inline int mutex_trylock(struct mutex *lock)
{
	return atomic_read(&lock->status) == 0;
}
/**
 * Unlock mutex.If it's not locked,do nothing.
 */
static inline void mutex_unlock(struct mutex *lock)
{
	__mutex_unlock(lock);
	mutex_clear_owner(lock);
}
#endif
