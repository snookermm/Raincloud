#include <mbr.h>
#include <isa.h>
void MBR_Init(MBR_Record *record)
{
	pio_read_sector(record, 0);
	
}
int Get_Partition(MBR_Record *record, uint8_t index, uint32_t *LBA, uint32_t *size)
{
	if (record == NULL)
		return -1;
	*LBA = (uint32_t)record->Part_Entry[index].LBA_First[0] | (uint32_t)record->Part_Entry[index].LBA_First[1] << 8 | (uint32_t)record->Part_Entry[index].LBA_First[2] << 16 | (uint32_t)record->Part_Entry[index].LBA_First[3] << 24;
	*size = (uint32_t)record->Part_Entry[index].Num_Of_Sec[0] | (uint32_t)record->Part_Entry[index].Num_Of_Sec[1] << 8 | (uint32_t)record->Part_Entry[index].Num_Of_Sec[2] << 16 | (uint32_t)record->Part_Entry[index].Num_Of_Sec[3] << 24;
	return 0;
}
