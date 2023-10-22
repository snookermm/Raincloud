#ifndef _VM_H_
#define _VM_H_

#include <types.h>
#include <list.h>



typedef struct vm_map_entry
{
	//All vm_map_entry are linked in this list
	list_head_t entry_list;
	
	uint64_t virt_addr;
	uint64_t phy_addr;
	uint64_t page_nums;
	uint64_t flags;
	uint64_t refs;
}vm_map_entry_t;
typedef struct vm_map
{
	list_head_t vm_map_list;
	vm_map_entry_t *map_entry;
}vm_map_t;


void malloc_init(void);
void *malloc(size_t size);
void free(void *ptr);

int vm_map_init(vm_map_t *vm_map);
int vm_map_add(vm_map_t *vm_map, vm_map_entry_t *entry);
void vm_map_clear(vm_map_t *vm_map);
void vm_map_dup(vm_map_t *new_vm_map, vm_map_t *old_vm_map);
void vm_map_do_mapping(vm_map_t *vm_map);

void vm_init(void);

#endif
