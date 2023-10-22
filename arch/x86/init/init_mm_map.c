 
#include <paging.h>
#include <klibc.h>
#include <types.h>
#include <uefi.h>
#include <mm.h>

/**
 * Global variable defined in link script
 */
extern char __INIT_START[];
extern char __INIT_END[];

/**
 * Global variable defined in init.S
 */
extern uint64_t image_base;
extern uint64_t image_size;
extern uint64_t mm_desc_addr;
extern uint64_t mm_desc_size;
extern uint64_t mm_desc_total;
extern uint64_t page_table_base;
extern uint64_t page_table_size;
extern uint64_t stack_page_base;
extern uint64_t stack_pages;

// #define DEFAULT_PTE_FLAG (PAGING_MASK_P | PAGING_MASK_R_W | PAGING_MASK_PWT | PAGING_MASK_PCD)
// #define DEFAULT_EMPTY_FLAG (PAGING_MASK_PWT | PAGING_MASK_PCD | PAGING_MASK_U_S)
// #define DEFAULT_PT_FLAG (PAGING_MASK_P | PAGING_MASK_PWT | PAGING_MASK_PCD | PAGING_MASK_U_S)

static inline uint64_t __get_addr(uint64_t *entry)
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
static inline void *__continous_alloc(void)
{
	page_table_size += 0x1000;
	return (void *)(page_table_base + page_table_size - 0x1000);
}
static int __do_mapping(uint64_t *plm4e, uint64_t virt_addr, uint64_t phy_addr, uint64_t flag)
{
	uint64_t *pdpte;
	uint64_t *pde;
	uint64_t *pte;
	uint64_t *plm4e_entry = plm4e + (virt_addr >> 39 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*plm4e_entry & PAGING_MASK_P))
	{
		pdpte = __continous_alloc();
		__init_table(pdpte, DEFAULT_EMPTY_FLAG);
		*plm4e_entry = __mk_entry((uint64_t )pdpte, DEFAULT_PT_FLAG);

	}
	else
		pdpte = (uint64_t *)__get_addr(plm4e_entry);
	
	uint64_t *pdpte_entry = pdpte + (virt_addr >> 30 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*pdpte_entry & PAGING_MASK_P))
	{
		pde = __continous_alloc();
		__init_table(pde, DEFAULT_EMPTY_FLAG);
		*pdpte_entry = __mk_entry((uint64_t )pde, DEFAULT_PT_FLAG);
	}
	else
		pde = (uint64_t *)__get_addr(pdpte_entry);
	
	uint64_t *pde_entry = pde + (virt_addr >> 21 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*pde_entry & PAGING_MASK_P))
	{
		pte = __continous_alloc();
		__init_table(pte, DEFAULT_EMPTY_FLAG);
		*pde_entry = __mk_entry((uint64_t )pte, DEFAULT_PT_FLAG);
	}
	else
		pte = (uint64_t *)__get_addr(pde_entry);
	
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
		pdpte = (uint64_t *)__get_addr(plm4e_entry);
	
	uint64_t *pdpte_entry = pdpte + (virt_addr >> 30 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*pdpte_entry & PAGING_MASK_P))
	{
		return -1;
	}
	else
		pde = (uint64_t *)__get_addr(pdpte_entry);
	
	uint64_t *pde_entry = pde + (virt_addr >> 21 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*pde_entry & PAGING_MASK_P))
	{
		return -1;
	}
	else
		pte = (uint64_t *)__get_addr(pde_entry);
	
	uint64_t *pte_entry = pte + (virt_addr >> 12 & PAGING_MASK_ENTRY_OFFSET);
	if (!(*pte_entry & PAGING_MASK_P))
	{
		return -1;
	}
	else
	{
		*phy_addr = __get_addr(pte_entry);
		*flag = __get_flag(pte_entry);
		return 0;
	}
}
static void __validate_mapping(uint64_t *plm4e, uint64_t virt_start_addr, uint64_t phy_start_addr, uint64_t number_of_pages, uint64_t flag)
{
	uint64_t temp_phy_addr, temp_flag;
	for (; number_of_pages -- > 0; phy_start_addr += 0x1000, virt_start_addr += 0x1000)
	{
		if (__query_mapping(plm4e, virt_start_addr, &temp_phy_addr, &temp_flag) != 0 || temp_phy_addr != phy_start_addr || temp_flag != flag)
		{
			printf("Mapping consistency checking failed.\n");
			printf("System halted.\n");
			asm("hlt");
		}
	}
}
void init_mm_map(void)
{
	printf("\n");
	printf("Init code start:%lx\n", __INIT_START);
	printf("Kernel code start:%lx\n", __INIT_END);
	printf("Memory Map:\n");
	printf("Type\t\tPhysicalStart\t\tVirtualStart\t\tNumberOfPages\t\tAttribute\n");
	efi_memory_descriptor_t *it;
	for (it = (void *)mm_desc_addr; (uint64_t)it < mm_desc_addr + mm_desc_total; it = (void *)it + mm_desc_size)
	{
		printf("%x\t%lx\t%lx\t%lx\t%lx\n", it->type, it->physical_start, it->virtual_start, it->number_of_pages, it->attribute);
	}
	//Calculate physical pages' number
	it = (void *)mm_desc_addr + mm_desc_total - mm_desc_size;
 	uint64_t phy_pages =  (it->physical_start >> 12) + it->number_of_pages;
	//Calculate pages to hold kernel image
	uint64_t kernel_image_pages = (image_size >> 12) + 1;
	//Calculate pages to hold UEFI memory descriptors and relocate.
	it = (void *)mm_desc_addr;
	mm_desc_addr = image_base + kernel_image_pages * 0x1000;
	uint64_t mm_desc_pages = (mm_desc_total >> 12) + 1;
	memset((void *)mm_desc_addr, 0, mm_desc_pages << 12);
	memcpy((void *)mm_desc_addr, it, mm_desc_total);
	
	//Calculate stack page base
	stack_page_base = mm_desc_addr + mm_desc_pages * 0x1000;
	//Produce page tables
	page_table_base = stack_page_base + stack_pages * 0x1000;
	
	
	printf("Image Base Address = %lx\n", image_base);
	printf("Image Size = %lx\n", image_size);
	printf("Memory Descriptor Location = %lx\n", mm_desc_addr);
	printf("Memory Descriptor Size = %lx\n", mm_desc_size);
	printf("Stack Page Base Address = %lx\n", stack_page_base);
	printf("Stack Pages = %lx\n", stack_pages);
	printf("Memory Descriptor Total = %lx\n", mm_desc_total);
	printf("Page Table Base Address = %lx\n", page_table_base);
	printf("Physical Page Number = %lx\n", phy_pages);
	
	uint64_t *plm4e;
	plm4e = __continous_alloc();
	__init_table(plm4e, PAGING_MASK_PWT | PAGING_MASK_PCD);
	
	//Map kernel image
	__map_by_pages(plm4e, KERNEL_IMAGE_BASE, image_base, kernel_image_pages, DEFAULT_PTE_FLAG);
	
	__validate_mapping(plm4e, KERNEL_IMAGE_BASE, image_base, kernel_image_pages, DEFAULT_PTE_FLAG);
	//Map physical address
	__map_by_pages(plm4e, PHY_MAP_BASE, 0, phy_pages, DEFAULT_PTE_FLAG);
	
	__validate_mapping(plm4e, PHY_MAP_BASE, 0, phy_pages, DEFAULT_PTE_FLAG);
	//Create a flat linear address space
	__map_by_pages(plm4e, 0, 0, phy_pages, DEFAULT_PTE_FLAG);
	__validate_mapping(plm4e, 0, 0, phy_pages, DEFAULT_PTE_FLAG);
	
	//Map stack pages
	__map_by_pages(plm4e, KERNEL_STACK_TOP - stack_pages * 0x1000, stack_page_base, stack_pages, DEFAULT_PTE_FLAG);
	__validate_mapping(plm4e, KERNEL_STACK_TOP - stack_pages * 0x1000, stack_page_base, stack_pages, DEFAULT_PTE_FLAG);

	printf("Page Table Size = %lx\n", page_table_size);
}
void load_mm_map(void)
{
	uint64_t cr3 = page_table_base | 0x18;
	asm (
		"mov	%0, %%rax\r\n"
		"mov	%%rax, %%cr3\r\n"
		:
		: "m" (cr3)
	);
	printf("Paging table has changed.\n");
}
