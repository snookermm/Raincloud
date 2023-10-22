#include <mm.h>
#include <paging.h>
#include <vm.h>

int vm_map_init(vm_map_t *vm_map)
{
	list_init(&vm_map->vm_map_list);
	vm_map->map_entry = NULL;
}
int vm_map_add(vm_map_t *vm_map, vm_map_entry_t *entry)
{
	vm_map_t *new_map;
	if ((new_map = malloc(sizeof(vm_map_t))) == NULL)
		return -1;
	list_init(&new_map->vm_map_list);
	list_add_before(&vm_map->vm_map_list, &new_map->vm_map_list);
	new_map->map_entry = entry;
	return 0;
}
void vm_map_clear(vm_map_t *vm_map)
{

	while(!list_empty(&vm_map->vm_map_list))
	{
		list_head_t *it = vm_map->vm_map_list.next;
		vm_map_t *vm_map = list_entry(it, vm_map_t, vm_map_list);
		vm_map_entry_t *entry = vm_map->map_entry;
		page_free(entry->phy_addr, entry->page_nums);
		list_del(it);
		free(vm_map);
	}
}
void vm_map_do_mapping(vm_map_t *vm_map)
{
	list_head_t *it;
// 	printf("%lx\n", vm_map);
	list_for_each(it, &vm_map->vm_map_list)
	{
		vm_map_t *vm_map = list_entry(it, vm_map_t, vm_map_list);
// 		printf("%lx\n", vm_map);
		vm_map_entry_t *entry = vm_map->map_entry;
// 		printf("%lx\n", entry);
		assert(vm_map != NULL);
// 		printf("%lx\n", entry->flags);
		assert((entry->flags & PAGING_MASK_ADDR) == 0);
		assert(entry->virt_addr + (entry->page_nums << 12) <= 0x0000800000000000);
// 		printf("%lx %lx %lx %lx\n",entry->virt_addr, entry->phy_addr, entry->page_nums, entry->flags);
		modify_chunk_mapping(get_current_plm4e(), entry->virt_addr, entry->phy_addr, entry->page_nums, entry->flags);
	}
}
void vm_map_undo_mapping(vm_map_t *vm_map)
{
	
	
}
void vm_map_entry_dup(vm_map_entry_t *new_entry, vm_map_entry_t *old_entry)
{
	new_entry->virt_addr = old_entry->virt_addr;
	new_entry->phy_addr = page_alloc(old_entry->page_nums);
	printf("%lx %lx\n", new_entry->phy_addr, old_entry->phy_addr);
	memcpy((void *)convert_phy_to_virt(new_entry->phy_addr), (void *)convert_phy_to_virt(old_entry->phy_addr), old_entry->page_nums << 12);
	new_entry->page_nums = old_entry->page_nums;
	new_entry->flags = old_entry->flags;
	new_entry->refs = 1;
}
/**
 * dupliate memory map structure and its associate phyisical memory region
 */
void vm_map_dup(vm_map_t *new_vm_map, vm_map_t *old_vm_map)
{
	vm_map_init(new_vm_map);
	list_head_t *it;
	list_for_each(it, &old_vm_map->vm_map_list)
	{
		vm_map_t *old_vm_map = list_entry(it, vm_map_t, vm_map_list);
		vm_map_entry_t *old_entry = old_vm_map->map_entry;
		
		assert(old_entry != NULL);
		
		vm_map_entry_t *new_entry = malloc(sizeof(vm_map_entry_t));
		vm_map_entry_dup(new_entry, old_entry);
		
		vm_map_add(new_vm_map, new_entry);
	}
}
