#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <list.h>
#include <klibc.h>
typedef struct queue_node
{
	list_head_t list;
	void *content;
}queue_node_t;

typedef struct queue
{
	list_head_t head;
	uint64_t length;
}queue_t;

int queue_init(queue_t *queue);
void queue_destroy(queue_t *queue);

void *queue_dequeue(queue_t *queue);
void queue_enqueue(queue_t *queue, void *content);
int queue_isempty(queue_t *queue);
#endif
