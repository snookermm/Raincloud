#ifndef _PAGING_H_
#define _PAGING_H_
#include <types.h>
#include <uefi.h>
#include <mm.h>
#include <klibc.h>
struct cr3
{
	uint64_t  _ignored_1:3,
		 pwt:1,
		 pcd:1,
		 _ignored_2:7,
		 phy_addr:40,
		 _reserved_1:12;
// 	uint64_t PCID:12,
// 	        addr:40,
// 	        _reserved:12;
};


struct plm4e_entry
{
	uint64_t p:1,
		 r_w:1,
		 u_s:1,
		 pwt:1,
		 pcd:1,
		 a:1,
		 _ignored_1:1,
		 ps:1,
		 _ignored_2:4,
		 phy_addr:40,
		  _ignored_3:11,
		 xd:1;
};

struct pdpte_entry
{
	uint64_t p:1,
		 r_w:1,
		 u_s:1,
		 pwt:1,
		 pcd:1,
		 a:1,
		 _ignored_1:1,
		 ps:1,
		 _ignored_2:4,
		 phy_addr:40,
		 _ignored_3:11,
		 xd:1; 
};
struct pde_entry
{
	uint64_t p:1,
		 r_w:1,
		 u_s:1,
		 pwt:1,
		 pcd:1,
		 a:1,
		 _ignored_1:1,
		 ps:1,
		 _ignored_2:4,
		 phy_addr:40,
		 _ignored_3:11,
		 xd:1;
};
struct pte_entry
{
	uint64_t p:1,
		 r_w:1,
		 u_s:1,
		 pwt:1,
		 pcd:1,
		 a:1,
		 d:1,
		 pat:1,
		 g:1,
		 _ignored_1:3,
		  phy_addr:40,
		 _ignored_2:7,
		 pk:4,
		 xd:1;
};
// struct plm4e_table
// {
// 	struct plm4e_entry entry[512];
// };
// struct pdpte_table
// {
// 	struct pdpte_entry entry[512];
// };
// struct pde_table
// {
// 	struct pde_entry entry[512];
// };
// struct pte_table
// {
// 	struct pte_entry entry[512];
// };
struct plm4e_table
{
	uint64_t entry[512];
};
struct pdpte_table
{
	uint64_t entry[512];
};
struct pde_table
{
	uint64_t entry[512];
};
struct pte_table
{
	uint64_t entry[512];
};
#define PAGING_MASK_P		0x0000000000000001
#define PAGING_MASK_R_W		0x0000000000000002
#define PAGING_MASK_U_S		0x0000000000000004
#define PAGING_MASK_PWT		0x0000000000000008
#define PAGING_MASK_PCD		0x0000000000000010
#define PAGING_MASK_A		0x0000000000000020
#define PAGING_MASK_D		0x0000000000000040
#define PAGING_MASK_PAT		0x0000000000000080
#define PAGING_MASK_G		0x0000000000000100
#define PAGING_MASK_ADDR	0x0000fffffffff000
#define PAGING_MASK_XD		0x8000000000000000

#define PAGING_MASK_ENTRY_OFFSET	     0x1ff


#define DEFAULT_PTE_FLAG (PAGING_MASK_P | PAGING_MASK_R_W | PAGING_MASK_PWT | PAGING_MASK_PCD)
#define DEFAULT_EMPTY_FLAG (PAGING_MASK_PWT | PAGING_MASK_PCD | PAGING_MASK_U_S)
#define DEFAULT_PT_FLAG (PAGING_MASK_P| PAGING_MASK_R_W | PAGING_MASK_PWT | PAGING_MASK_PCD | PAGING_MASK_U_S)

#define DEFAULT_USER_TEXT_PTE_FLAGS (PAGING_MASK_P | PAGING_MASK_R_W | PAGING_MASK_PWT | PAGING_MASK_PCD | PAGING_MASK_U_S)
/**
 * Size in pages.
 */
uint64_t page_alloc(uint64_t size);
void page_free(uint64_t addr, uint64_t size);

static inline uint64_t convert_virt_to_phy(uint64_t addr)
{
	assert(addr > PHY_MAP_BASE);
	return addr - PHY_MAP_BASE;
}
static inline uint64_t convert_phy_to_virt(uint64_t addr)
{
	assert(addr < PHY_MAP_BASE);
	return addr + PHY_MAP_BASE;
}

uint64_t *get_current_plm4e(void);
int modify_chunk_mapping(uint64_t *plm4e, uint64_t virt_start_addr, uint64_t phy_start_addr, uint64_t number_of_pages, uint64_t flag);
int query_mapping(uint64_t *plm4e, uint64_t virt_addr, uint64_t *phy_addr, uint64_t *flag);

#endif

