#ifndef _UEFI_H_
#define _UEFI_H_
#include <types.h>

//*******************************************************
//EFI_MEMORY_TYPE
//*******************************************************
typedef enum {
	EfiReservedMemoryType,
	EfiLoaderCode,
	EfiLoaderData,
	EfiBootServicesCode,
	EfiBootServicesData,
	EfiRuntimeServicesCode,
	EfiRuntimeServicesData,
	EfiConventionalMemory,
	EfiUnusableMemory,
	EfiACPIReclaimMemory,
	EfiACPIMemoryNVS,
	EfiMemoryMappedIO,
	EfiMemoryMappedIOPortSpace,
	EfiPalCode,
	EfiPersistentMemory,
	EfiMaxMemoryType
}efi_momery_type;

//*******************************************************
// Memory Attribute Definitions
//*******************************************************
// These types can be “ORed” together as needed.
#define EFI_MEMORY_UC			0x0000000000000001
#define EFI_MEMORY_WC			0x0000000000000002
#define EFI_MEMORY_WT			0x0000000000000004
#define EFI_MEMORY_WB			0x0000000000000008
#define EFI_MEMORY_UCE			0x0000000000000010
#define EFI_MEMORY_WP			0x0000000000001000
#define EFI_MEMORY_RP			0x0000000000002000
#define EFI_MEMORY_XP			0x0000000000004000
#define EFI_MEMORY_NV			0x0000000000008000
#define EFI_MEMORY_MORE_RELIABLE	0x0000000000010000
#define EFI_MEMORY_RO			0x0000000000020000
#define EFI_MEMORY_RUNTIME		0x8000000000000000
//*******************************************************
//EFI_MEMORY_DESCRIPTOR
//*******************************************************
typedef struct {
	uint32_t type;
	uint64_t physical_start;
	uint64_t virtual_start;
	uint64_t number_of_pages;
	uint64_t attribute;
}efi_memory_descriptor_t;

#endif
