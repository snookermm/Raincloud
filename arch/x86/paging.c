#include <paging.h>
#include <klibc.h>
#include <types.h>
#include <uefi.h>
#include <list.h>
#include <mm.h>
#include <cpu_intrins.h>

void __page_consistency_check(void);
void __page_merge(void);


static inline uint64_t __get_virt_addr(uint64_t *entry)
{
	return convert_phy_to_virt((*entry & PAGING_MASK_ADDR));
}
static inline uint64_t __get_phy_addr(uint64_t *entry)
{
	return *entry & PAGING_MASK_ADDR;
}
static inline uint64_t __get_flag(uint64_t *entry)
{
	return *entry & (PAGING_MASK_P | PAGING_MASK_R_W | PAGING_MASK_U_S | PAGING_MASK_PWT | PAGING_MASK_PCD | PAGING_MASK_A | PAGING_MASK_D | PAGING_MASK_PAT | PAGING_MASK_G | PAGING_MASK_XD);
}

static inline uint64_t __mk_entry(uint64_t addr, uint64_t flag)
{
	return addr & PAGING_MASK_ADDR | flag ;
}
static inline void __init_table(void *table, uint64_t init_entry)
{
	for (int i = 0; i < 512; i ++)
	{
		*((uint64_t *)table + i) = init_entry;
	}
}
static inline void __set_entry(void *table, uint16_t n, uint64_t addr, uint64_t flag)
{
	assert(n < 512 && n >= 0);
	*((uint64_t *)table + n) = addr & PAGING_MASK_ADDR | flag;
}
static inline void *__alloc_new_page_table(void)
{
	return (void *)convert_phy_to_virt(page_alloc(1));
}
static int __do_mapping(uint64_t *plm4e, uint64_t virt_addr, uint64_t phy_addr, uint64_t flag)
{
	uint64_t *pdpte;
	uint64_t *pde;
	uint64_t *pte;
	uint64_t *plm4e_entry = plm4e + (virt_addr >> 39 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*plm4e_entry & PAGING_MASK_P))
	{
		pdpte = __alloc_new_page_table();
		__init_table(pdpte, DEFAULT_EMPTY_FLAG);
		*plm4e_entry = __mk_entry(convert_virt_to_phy((uint64_t)pdpte), DEFAULT_PT_FLAG);

	}
	else
		pdpte = (uint64_t *)__get_virt_addr(plm4e_entry);
	
	uint64_t *pdpte_entry = pdpte + (virt_addr >> 30 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*pdpte_entry & PAGING_MASK_P))
	{
		pde = __alloc_new_page_table();
		__init_table(pde, DEFAULT_EMPTY_FLAG);
		*pdpte_entry = __mk_entry(convert_virt_to_phy((uint64_t)pde), DEFAULT_PT_FLAG);
	}
	else
		pde = (uint64_t *)__get_virt_addr(pdpte_entry);
	
	uint64_t *pde_entry = pde + (virt_addr >> 21 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*pde_entry & PAGING_MASK_P))
	{
		pte = __alloc_new_page_table();
		__init_table(pte, DEFAULT_EMPTY_FLAG);
		*pde_entry = __mk_entry(convert_virt_to_phy((uint64_t )pte), DEFAULT_PT_FLAG);
	}
	else
		pte = (uint64_t *)__get_virt_addr(pde_entry);
	
	uint64_t *pte_entry = pte + (virt_addr >> 12 & PAGING_MASK_ENTRY_OFFSET);
	*pte_entry = __mk_entry(phy_addr, flag);
}
static int __map_by_pages(uint64_t *plm4e, uint64_t virt_start_addr, uint64_t phy_start_addr, uint64_t number_of_pages, uint64_t flag)
{
	for (; number_of_pages -- > 0; phy_start_addr += 0x1000, virt_start_addr += 0x1000)
	{
		__do_mapping(plm4e, virt_start_addr, phy_start_addr, flag);
	}
}
static int __query_mapping(uint64_t *plm4e, uint64_t virt_addr, uint64_t *phy_addr, uint64_t *flag)
{
	uint64_t *pdpte;
	uint64_t *pde;
	uint64_t *pte;
	uint64_t *plm4e_entry = plm4e + (virt_addr >> 39 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*plm4e_entry & PAGING_MASK_P))
	{
		return -1;
	}
	else
		pdpte = (uint64_t *)__get_virt_addr(plm4e_entry);
	
	uint64_t *pdpte_entry = pdpte + (virt_addr >> 30 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*pdpte_entry & PAGING_MASK_P))
	{
		return -1;
	}
	else
		pde = (uint64_t *)__get_virt_addr(pdpte_entry);
	
	uint64_t *pde_entry = pde + (virt_addr >> 21 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*pde_entry & PAGING_MASK_P))
	{
		return -1;
	}
	else
		pte = (uint64_t *)__get_virt_addr(pde_entry);
	
	uint64_t *pte_entry = pte + (virt_addr >> 12 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*pte_entry & PAGING_MASK_P))
	{
		return -1;
	}
	else
	{
		*phy_addr = __get_phy_addr(pte_entry);
		*flag = __get_flag(pte_entry);
		return 0;
	}
}
static void __validate_mapping(uint64_t *plm4e, uint64_t virt_start_addr, uint64_t phy_start_addr, uint64_t number_of_pages, uint64_t flag)
{
	uint64_t temp_phy_addr, temp_flag;
	for (; number_of_pages -- > 0; phy_start_addr += 0x1000, virt_start_addr += 0x1000)
	{
		if (__query_mapping(plm4e, virt_start_addr, &temp_phy_addr, &temp_flag) != 0 || 
			temp_phy_addr != phy_start_addr || 
			temp_flag != flag)
		{
			printf("Mapping consistency checking failed.\n");
			printf("System halted.\n");
			__halt__();
		}
	}
}
uint64_t *get_current_plm4e(void)
{
	uint64_t cr3 = __read_cr3__();
	return (uint64_t *)((cr3 & 0xfffffffffffff000) + PHY_MAP_BASE);
}
int modify_chunk_mapping(uint64_t *plm4e, uint64_t virt_start_addr, uint64_t phy_start_addr, uint64_t number_of_pages, uint64_t flag)
{
	__map_by_pages(plm4e, virt_start_addr, phy_start_addr, number_of_pages, flag);
	__validate_mapping(plm4e, virt_start_addr, phy_start_addr, number_of_pages, flag);
	return 0;
}
int query_mapping(uint64_t *plm4e, uint64_t virt_addr, uint64_t *phy_addr, uint64_t *flag)
{
	return __query_mapping(plm4e, virt_addr, phy_addr, flag);
}

struct mm_block
{
	struct list_head mm_block_list;
	uint64_t length;//Length in pages
};

static struct 
{
	struct list_head mm_list;
	uint64_t available;
	uint64_t total;
} mm_pool;

struct mm_block *__init_block(uint64_t base, uint64_t number_of_pages)
{
// 	memset((void *)base, 0, number_of_pages << 12);
// 	for (uint64_t ptr = base; ptr < base + (number_of_pages << 12); ptr += 8)
// 		*(uint64_t *)ptr = 0;
	struct mm_block *block_ptr = (void *)base;
	list_init(&block_ptr->mm_block_list);
	block_ptr->length = number_of_pages;
	return block_ptr;
}

static void __dump_mm_map(efi_memory_descriptor_t *desc, uint64_t size, uint64_t total_length)
{
	printf("Memory Map:\n");
	printf("Type\t\tPhysicalStart\t\tVirtualStart\t\tNumberOfPages\t\tAttribute\n");
	efi_memory_descriptor_t *it;
	for (it = (void *)desc; (uint64_t)it < (uint64_t)desc + total_length; it = (void *)it + size)
	{
		printf("%x\t%lx\t%lx\t%lx\t%lx\n", it->type, it->physical_start, it->virtual_start, it->number_of_pages, it->attribute);
	}
	
}

/**
 *  Initializing physical memory according to UEFI memory descriptor array.
 */
void page_alloc_init(efi_memory_descriptor_t *desc, uint64_t size, uint64_t total_length, uint64_t lowerbound)
{
	printf("Page_alloc initializing:\n%lx %lx %lx %lx\n", desc, size, total_length, lowerbound);
// 	__dump_mm_map(desc, size, total_length);
	
	list_init(&mm_pool.mm_list);
	mm_pool.available = 0;
	
	efi_memory_descriptor_t *ptr = NULL;
	uint64_t count = total_length / size;
	assert(count * size == total_length);
	for (uint64_t i = 0; i < count; i ++)
	{
		ptr = ((void *)desc + i * size);
		if (ptr->type == EfiConventionalMemory ||
			 ptr->type == EfiBootServicesCode ||
			 ptr->type == EfiBootServicesData ||
			 ptr->type == EfiLoaderCode || 
			 ptr->type == EfiLoaderData)
		{ 
			uint64_t physical_start = ptr->physical_start > lowerbound ? ptr->physical_start : lowerbound;
			uint64_t physical_end = ptr->physical_start + ((ptr->number_of_pages) << 12);
			uint64_t number_of_pages = (physical_end - physical_start) >> 12;
			struct mm_block *block_ptr = __init_block(convert_phy_to_virt(physical_start), number_of_pages);
			list_add_before(&mm_pool.mm_list, &block_ptr->mm_block_list);
			mm_pool.available += number_of_pages;
		}
	}
	mm_pool.total = mm_pool.available;
	printf("Page_alloc initializing complete.\n");
	printf("Total %ld pages available.\n", mm_pool.available);
}
uint64_t __page_alloc(uint64_t size)
{
	if (list_empty(&mm_pool.mm_list))
	{
		printf("Allocating pages faild due to out of memory:allocate size %ld with %ld available\n", size, mm_pool.available);
		return INVALID_ADDR;
	}
	struct list_head *it;
	list_for_each(it, &mm_pool.mm_list)
	{
		struct mm_block *entry = list_entry(it, struct mm_block, mm_block_list);
		if (entry->length == size)
		{
			list_del(it);
			mm_pool.available -= size;
// 			printf("Allocate %d pages, %d pages left.\n", size, mm_pool.available);
			return convert_virt_to_phy((uint64_t)it);
		}
		else if (entry->length > size)
		{
			entry->length -= size;
			mm_pool.available -= size;
// 			printf("Allocate %d pages, %d pages left.\n", size, mm_pool.available);
			return convert_virt_to_phy((uint64_t)it + (entry->length << 12));
		}
	}
	return INVALID_ADDR;
}
//In pages.
uint64_t page_alloc(uint64_t size)
{
	if (size == 0)
		return INVALID_ADDR;
	
	uint64_t ret = __page_alloc(size);
//Lazily merge adjacent free blocks
	if (ret == INVALID_ADDR)
	{
		__page_merge();
		ret = __page_alloc(size);
	}
	__page_consistency_check();
	return ret;
}
void __page_free(uint64_t addr, uint64_t size)
{
	addr = convert_phy_to_virt(size);
	struct mm_block *block_ptr = __init_block(addr, size);
	struct list_head *it;
	list_for_each(it, &mm_pool.mm_list)
	{
		struct mm_block *entry = list_entry(it, struct mm_block, mm_block_list);
		if ((uint64_t)entry > addr)
			break;
	}
	list_add_before(it, &block_ptr->mm_block_list);
	mm_pool.available += size;
// 	printf("Free %d pages, %d pages left.\n", size, mm_pool.available);
}
void __page_merge(void)
{
	struct list_head *it;
	list_for_each(it, &mm_pool.mm_list)
	{
		if (it->next != &mm_pool.mm_list)
		{
			struct mm_block *entry = list_entry(it, struct mm_block, mm_block_list);
			struct mm_block *next_entry = list_entry(it->next, struct mm_block, mm_block_list);
			while ((uint64_t)entry + (entry->length << 12) == (uint64_t)next_entry)
			{
				entry->length += next_entry->length;
				list_del(&next_entry->mm_block_list);
				memset(next_entry, 0, sizeof(struct mm_block));
				if (it->next != &mm_pool.mm_list)
					next_entry = list_entry(it->next, struct mm_block, mm_block_list);
				else
					break;
			}
		}
	}

}
void page_free(uint64_t addr, uint64_t size)
{
	__page_free(addr, size);

	__page_consistency_check();
}
void __page_consistency_check(void)
{
	assert(mm_pool.available <= mm_pool.total);
	uint64_t pages_sum = 0;
	struct list_head *it;
	list_for_each(it, &mm_pool.mm_list)
	{
		struct mm_block *entry = list_entry(it, struct mm_block, mm_block_list);
		if (it->next != &mm_pool.mm_list)
		{
			struct mm_block *next_entry = list_entry(it->next, struct mm_block, mm_block_list);
			if((uint64_t)entry + (entry->length << 12) > (uint64_t)next_entry + (next_entry->length << 12))
				bug("Free page block linking list corrupted");
		}
		pages_sum += entry->length;
	}
	if (pages_sum != mm_pool.available)
	{
		bug("Memory leakage detected, available %d, found %d.\n", mm_pool.available, pages_sum);
	}
}
void __dump_page_list(void)
{
	struct list_head *it;
	list_for_each(it, &mm_pool.mm_list)
	{
		struct mm_block *entry = list_entry(it, struct mm_block, mm_block_list);
		printf("Start address:%lx\tPages:%lx\n", entry, entry->length);
	}
	
}
int mm_init(efi_memory_descriptor_t *desc, uint64_t size, uint64_t total_length, uint64_t lowerbound)
{
	page_alloc_init(desc, size, total_length, lowerbound);
	
// 	uint64_t s[10] = { 1,2,4,8,16.32,64,128,256,512,1024};
// 	uint64_t a[10] = {0};
// 	for (int i = 0; i < 10; i ++)
// 		a[i] = page_alloc(s[i]);
// 	for (int i = 0; i < 10; i ++)
// 		page_free(a[i], s[i]);
// 	__dump_page_list();
// 	printf("\n");
// 	__page_merge();
// 	__dump_page_list();
// 	uint64_t addr = page_alloc(10000);
// 	printf("Total %ld pages available.\n", mm_pool.available);
// 	page_free(addr, 10000);
// 	printf("Total %ld pages available.\n", mm_pool.available);
	return 0;
}



