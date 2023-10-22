#ifndef _LIST_H_
#define _LIST_H_

typedef struct list_head
{
	struct list_head *next;
	struct list_head *prev;
}list_head_t;
/**
 * Init a new entry.
 */
static inline void list_init(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}
/**
 * Internal use function to simplify the 
 * implementation of list_add_xx function.
 * DO NOT USE!!!
 */
static inline void __list_add(struct list_head *prev, struct list_head *next, struct list_head *new)
{
	new->prev=prev;
	prev->next=new;
	new->next=next;
	next->prev=new;
}
/**
 * Insert a new entry to the list.
 * @list the list head to add it after
 * @new  the new entry to be added
 * 
 */ 
static inline void list_add_after(struct list_head *list, struct list_head *new)
{
	__list_add(list, list->next, new);
}
/**
 * Insert a new entry to the list.
 * @list the list head to add it before
 * @new the new entry to be added
 */
static inline void list_add_before(struct list_head *list, struct list_head *new)
{
	__list_add(list->prev, list, new);
}
/**
 * Internal use function to simplify the 
 * implementation of list_del_xx function.
 * DO NOT USE!!!
 */
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	prev->next=next;
	next->prev=prev;
}
/**
 * Deletes an entry from the list.
 * @entry: the element to delete from the list
 */
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev,entry->next);
}
/**
 * Replace old entry by new one.
 * @old : the element to be replaced
 * @new : the new element to insert
 */
static inline void list_replace(struct list_head *old, struct list_head *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}
/**
 * Delete from one list and add as another's head.
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void list_move(struct list_head *head, struct list_head *list)
{
	list_del(list);
	list_add_after(head, list);
}
/**
 * Delete from one list and add as another's tail.
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void list_move_tail(struct list_head *head,
				  struct list_head *list)
{
	list_del(list);
	list_add_before(head, list);
}

/**
 * Tests whether @list is the last entry in list @head.
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int list_is_last(const struct list_head *head,
			       const struct list_head *list)
{
	return list->next == head;
}
/**
 * Tests whether a list is empty.
 * @head: the list to test
 */
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

#define list_entry(ptr, type, MEMBER) \
	((type *)((char *)ptr - ((size_t)&(((type *)0)->MEMBER))))

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#endif
