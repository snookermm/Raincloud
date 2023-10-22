#include <queue.h>
int queue_init(queue_t *queue)
{
	list_init(&queue->head);
	queue->length = 0;
	return 0;
}
void queue_destroy(queue_t *queue)
{
	if (queue_isempty(queue))
		return;
	list_head_t *it;
	list_for_each(it, &queue->head)
	{
		queue_node_t *entry = list_entry(it, queue_node_t, list);
		list_del(&entry->list);
		free(entry);
		queue->length --;
	}
}

void *queue_dequeue(queue_t *queue)
{
	if (queue_isempty(queue))
		return NULL;
	
	queue_node_t *extract = list_entry(queue->head.next, queue_node_t, list);
	list_del(&extract->list);
	void *temp = extract->content;
	free(extract);
	queue->length --;
	return temp;
	
}
void queue_enqueue(queue_t *queue, void *content)
{
	queue_node_t *new_node = malloc(sizeof(queue_node_t));
	list_init(&new_node->list);
	new_node->content = content;
	
	list_add_before(&queue->head, &new_node->list);
	queue->length ++;
}
int queue_isempty(queue_t *queue)
{
	return queue->length == 0;
}
