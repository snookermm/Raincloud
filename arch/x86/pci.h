#ifndef _PCI_H_
#define _PCI_H_

#include <io.h>
#include <klibc.h>
#include <list.h>

#define PCI_CONF_ADDR_REG_ADDR	0x0CF8
#define PCI_CONF_DATA_REG_ADDR	0x0CFC

static inline void __pci_conf_addr_reg_write(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg)
{
	assert(dev >> 5 == 0);
	assert(func >> 3 == 0);
	assert(reg >> 6 == 0);
	__io_write_32(PCI_CONF_ADDR_REG_ADDR, 1 << 31 | bus << 16 | dev << 11 | func << 8 | reg << 2);
}
static inline void __pci_conf_data_reg_write(uint32_t data)
{
	__io_write_32(PCI_CONF_DATA_REG_ADDR, data);
}
static inline uint32_t __pci_conf_addr_reg_read(void)
{
	return __io_read_32(PCI_CONF_ADDR_REG_ADDR);
}
static inline uint32_t __pci_conf_data_reg_read(void)
{
	return __io_read_32(PCI_CONF_DATA_REG_ADDR);
}

static inline void __pci_write_reg(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg, uint32_t data)
{
	__pci_conf_addr_reg_write(bus, dev, func, reg);
	__pci_conf_data_reg_write(data);
}
static inline uint32_t __pci_read_reg(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg)
{
	__pci_conf_addr_reg_write(bus, dev, func, reg);
	return __pci_conf_data_reg_read();
}

#define CLASS_CODE_BASE_CLASS_DISPLAY_CONTROLLER	0x03
#define CLASS_CODE_BASE_CLASS_DISPLAY_CONTROLLER_VGA	0x00


typedef struct Command
{
	uint16_t IO_Space:1,
		 Memory_Space:1,
		 Bus_Master:1,
		 Special_Cycles:1,
		 Memory_Write_and_Invalidate_Enable:1,
		 VGA_Palette_Snoop:1,
		 Parity_Error_Response:1,
		 Reserved_1:1,
		 SERR_Enable:1,
		 Fast_Back_to_Back_Enable:1,
		 Interrupt_Disable:1,
		 Reserved_2:5;
}Command_t;
typedef struct Status
{
	uint16_t Reserved_1:3,
		 Interrupt_Status:1,
		 Capabilities_List:1,
		 _66MHz_Capable:1,
		 Reserved_2:1,
		 Fast_Back_to_Back_Capable:1,
		 Master_Data_Parity_Error:1,
		 DEVSEL_Timing:2,
		 Signaled_Target_Abort:1,
		 Received_Target_Abort:1,
		 Received_Master_Abort:1,
		 Signaled_System_Error:1,
		 Detected_Parity_Error:1;
}Status_t;
typedef struct BIST
{
	uint8_t Completion_Code:4,
		Reserved:2,
		Start_BIST,
		BIST_Capable;
}BIST_t;
typedef struct Class_Code
{
	uint8_t Interface;
	uint8_t Sub_Class;
	uint8_t Base_Class;
}Class_Code_t;

typedef struct __attribute__((packed)) PCI_Conf_Space_Header
{
	uint16_t Vendor_ID;
	uint16_t Device_ID;
	Command_t Command;
	Status_t Status;
	uint8_t Revision_ID;
	Class_Code_t Class_Code;
	uint8_t Cacheline_Size;
	uint8_t Latency_Timer;
	uint8_t Header_Type;
	uint8_t BIST;
	uint32_t Base_Address_Register[6];
	uint32_t Cardbus_CIS_Pointer;
	uint16_t Subsystem_Vendor_ID;
	uint16_t Subsystem_ID;
	uint32_t Expansion_ROM_Base_Address;
	uint8_t Capabilities_Pointer;
	uint8_t Reserved[7];
	uint8_t Interrupt_Line;
	uint8_t Interrupt_Pin;
	uint8_t Min_Gnt;
	uint8_t Max_Lat;
}PCI_Conf_Space_Header_t;


typedef struct PCI_Device
{
	list_head_t list;
	
	uint8_t Bus;
	uint8_t Device;
	uint8_t Function;
	PCI_Conf_Space_Header_t Conf_Header;
}PCI_Device_t;

uint64_t get_vga_frame_buffer_addr(void);
void pci_init(void);

#endif
