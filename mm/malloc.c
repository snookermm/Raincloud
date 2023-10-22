#include <mm.h>
#include <list.h>
#include <paging.h>

struct mm_block
{
	struct list_head mm_block_list;
	uint64_t length;//Length in bytes
	uint64_t used;
};
#define BLOCK_STRUCT_SIZE sizeof(struct mm_block)
static struct
{
	struct list_head mm_list;
	uint64_t available;
	uint64_t total;
	uint64_t block_count;
}vm_heap;


/**
 * Round size to a multiple of 16.
 * 
 */
static inline uint64_t roundsize(uint64_t size)
{
	return size & 0xf ? (size & 0xfffffffffffffff0) + 0x10 : size;
}
/**
 * Allocate a buffer of size bytes.
 */
void *__malloc(size_t size)
{
	if (list_empty(&vm_heap.mm_list))
	{
		printf("Allocating heap memory failed due to out of memory:allocate size %ld with %ld available.\n", size, vm_heap.available);
		return NULL;
	}
	
	struct list_head *it;
	list_for_each(it, &vm_heap.mm_list)
	{
		struct mm_block *entry = list_entry(it, struct mm_block, mm_block_list);
		if (entry->used || entry->length < size)
			continue;
		if (entry->length < size + BLOCK_STRUCT_SIZE + 16)
		{
			entry->used = 1;
			vm_heap.available -= entry->length;
			return entry + 1;
		}
		else 
		{
			
			entry->length -= size + BLOCK_STRUCT_SIZE;
			struct mm_block *new_entry = (void *)(entry + 1) + entry->length;
			list_init(&new_entry->mm_block_list);
			new_entry->length = size;
			new_entry->used = 1;
			list_add_after(it, &new_entry->mm_block_list);
			
			vm_heap.available -= new_entry->length + BLOCK_STRUCT_SIZE;
			vm_heap.block_count ++;
			return new_entry + 1;
		}
	}
	return NULL;
}

/**
 * Deallocate a buffer.
 */
void __free(void *ptr)
{
	struct mm_block *entry = ptr - BLOCK_STRUCT_SIZE;
	if (entry->used == 1)
	{
		entry->used = 0;
		vm_heap.available += entry->length;
	}
	else
	{
		bug("Kernel heap free block has beed freed %ld\n",ptr);
	}
}
/**
 * Join small close block into continuous larger blocks.
 */
static void __merge_block(void)
{
	struct list_head *it;
	list_for_each(it, &vm_heap.mm_list)
	{
		if (it->next != &vm_heap.mm_list)
		{
			struct mm_block *entry = list_entry(it, struct mm_block, mm_block_list);
			struct mm_block *next_entry = list_entry(it->next, struct mm_block, mm_block_list);
			while (!entry->used && !next_entry->used)
			{
				entry->length += next_entry->length + BLOCK_STRUCT_SIZE;
				list_del(&next_entry->mm_block_list);
				memset(next_entry, 0, BLOCK_STRUCT_SIZE);
				vm_heap.block_count --;
				vm_heap.available += BLOCK_STRUCT_SIZE;
				if (it->next != &vm_heap.mm_list)
					next_entry = list_entry(it->next, struct mm_block, mm_block_list);
				else
					break;
			}
		}
	}
}
static void __mm_list_consistency_check(void)
{
	uint64_t sum = 0;
	struct list_head *it;
	list_for_each(it, &vm_heap.mm_list)
	{
		struct mm_block *entry = list_entry(it, struct mm_block, mm_block_list);
		sum += entry->length;
		if (it->next != &vm_heap.mm_list)
		{
			struct mm_block *next_entry = list_entry(it->next, struct mm_block, mm_block_list);
			if ((uint64_t)entry + entry->length + BLOCK_STRUCT_SIZE != (uint64_t)next_entry)
			{
				bug("Kernel heap free block linking list corrupted.\n");
			}
		}
	}
	if (sum + vm_heap.block_count * BLOCK_STRUCT_SIZE != vm_heap.total)
	{
		bug("Memory leakage detected, total %d, found %d.\n", vm_heap.total, sum);
	}
}
void __dump_mm_list(void)
{
	printf("Total %ld Available %ld Block Count %ld\n", vm_heap.total, vm_heap.available, vm_heap.block_count);
	struct list_head *it;
	list_for_each(it, &vm_heap.mm_list)
	{
		struct mm_block *entry = list_entry(it, struct mm_block, mm_block_list);
		printf("%lx %lx\n", entry, entry->length);
	}
	
}
void *malloc(size_t size)
{
	uint64_t *ret;
	if (size == 0)
		return NULL;
	
	size = roundsize(size);
	ret = __malloc(size);
	if (ret == NULL)
	{
		__merge_block();
		ret = __malloc(size);
	}
	memset(ret, 0, size);
	__mm_list_consistency_check();
// 	printf("Alloc %d\n", size);
// 	__dump_mm_list();
	return ret;
}
void free(void *ptr)
{
	__free(ptr);
	__mm_list_consistency_check();
}
//TODO:Implement this function
void malloc_expand_heap(uint64_t size)
{
	
}
#define INITIAL_HEAP_SIZE 0x10000
void malloc_init(void)
{
	list_init(&vm_heap.mm_list);
	vm_heap.total = INITIAL_HEAP_SIZE << 12;
	vm_heap.block_count = 1;
	vm_heap.available = vm_heap.total - vm_heap.block_count * BLOCK_STRUCT_SIZE;
	uint64_t page_addr = page_alloc(INITIAL_HEAP_SIZE);
	printf("%lx\n", get_current_plm4e());
	modify_chunk_mapping(get_current_plm4e(), KERNEL_HEAP_BASE, page_addr, INITIAL_HEAP_SIZE, DEFAULT_PTE_FLAG);

	struct mm_block *new_block = (struct mm_block *)KERNEL_HEAP_BASE;
	list_init(&new_block->mm_block_list);
	list_add_after(&vm_heap.mm_list, &new_block->mm_block_list);
	new_block->length = vm_heap.available;
	new_block->used = 0;
	
	

//Test
	__dump_mm_list();
	uint64_t s[10] = {2,4,8,16,32,64,128,256,512,1024};
	uint64_t *a[10] = {0};
	for (int i = 0; i < 10; i ++)
		a[i] = malloc(s[i]);
// 	__dump_mm_list();
	__merge_block();
	for (int i = 0; i < 10; i ++)
		free(a[i]);
// 	__dump_mm_list();
	__merge_block();
	__dump_mm_list();
}
