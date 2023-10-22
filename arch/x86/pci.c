#include <klibc.h>
#include <pci.h>
#include <list.h>
#include <paging.h>
list_head_t *pci_dev_list;

void pci_init(void)
{
	
	pci_dev_list = malloc(sizeof(list_head_t));
	list_init(pci_dev_list);
	
	//Enumerate all pci devices and store infomation into pci_dev_list
	uint8_t dev, func, reg;
	for (dev = 0;; dev ++)
	{
		if (__pci_read_reg(0, dev, 0, 0) & 0xffff == 0xffff)
			break;
		
		for (func = 0;; func ++)
		{
			if (__pci_read_reg(0, dev, func, 0) & 0xffff == 0xffff)
				break;
			
			PCI_Device_t *pci_device = malloc(sizeof(PCI_Device_t));
			list_init(&pci_device->list);
			list_add_before(pci_dev_list, &pci_device->list);
			
			pci_device->Bus = 0;
			pci_device->Device = dev;
			pci_device->Function = func;
			uint32_t *header = (uint32_t *)&pci_device->Conf_Header;
			for (reg = 0; reg < 16; reg ++)
				*header++ = __pci_read_reg(0, dev, func, reg);
		}
	}
	printf("PCI Device Enumeration Complete.\n");
}

uint64_t get_vga_frame_buffer_addr(void)
{
	list_head_t *it;
	list_for_each(it, pci_dev_list)
	{
		PCI_Device_t *dev_entry = list_entry(it, PCI_Device_t, list);
		if (dev_entry->Conf_Header.Class_Code.Base_Class == 0x03 && dev_entry->Conf_Header.Class_Code.Sub_Class == 0x00 && dev_entry->Conf_Header.Class_Code.Interface == 0x00)
		{
			return ((uint64_t)dev_entry->Conf_Header.Base_Address_Register[0] & 0x00000000fffff000);
		}
	}
	return 0;
}
