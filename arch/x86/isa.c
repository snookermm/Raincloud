#include <io.h>
#include <klibc.h>
/**
 * The Intel 82C54 PIT used a 1.193182 MHz clock signal(one third of the color
 * burst frequency used by NTSC, one twelfth of the system clock crystal
 * oscillator and contains three timers.
 * 
 * div = input_frequency / output_frequency
 * 
 * @ch channel 0-2
 * @mode mode 0-5
 * @div reload counter value 0x0000-0xffff
 */

void timer_82c54_init(uint8_t ch, uint8_t mode,  uint16_t counter)
{
	__io_write_8(0x43, ch << 6 | 3 << 4 | mode << 1 | 0);
	__io_write_8(0x40, counter);
	__io_write_8(0x40, counter >> 8);
}

/**
 * 
 * @vec_base must be a multiple of 8
 * 
 */
void pic_8259_init(uint8_t vec_base)
{
	vec_base &= 0xf8;
	__io_write_8(0x20, 1 << 4 | 1);
	__io_write_8(0x21, vec_base);
	__io_write_8(0x21, 1 << 2);
	__io_write_8(0x21,  1 << 1 | 1);
	//Mask all interrupt except IRQ2
	__io_write_8(0x21, 0xfb);
	
	__io_write_8(0xa0, 1 << 4 | 1);
	__io_write_8(0xa1, vec_base + 8);
	__io_write_8(0xa1, 1 << 1);
	__io_write_8(0xa1,  1 << 1 | 1);
	//Mask all interrupt
	__io_write_8(0xa1, 0xff);
	
}
void pic_8259_mask_int(uint8_t irq)
{
	assert(irq >= 0 && irq <= 15);
	uint8_t reg = irq < 8 ? 0x21 : 0xa1;
	uint8_t bit = irq < 8 ? irq : irq - 8;
	__io_write_8(reg, __io_read_8(reg) | 1 << bit);
}
void pic_8259_unmask_int(uint8_t irq)
{
	assert(irq >= 0 && irq <= 15);
	uint8_t reg = irq < 8 ? 0x21 : 0xa1;
	uint8_t bit = irq < 8 ? irq : irq - 8;
	__io_write_8(reg, __io_read_8(reg) & ~(1 << bit));
}


#define IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET				0x1F0
#define IDE_ATA_PRIMARY_CONTROL_BLOCK_OFFSET				0x3F4

#define IDE_ATA_COMMAND_BLOCK_REG_OFFSET_DATA				0x00
#define IDE_ATA_COMMAND_BLOCK_REG_OFFSET_ERROR_FEATURES			0x01
#define IDE_ATA_COMMAND_BLOCK_REG_OFFSET_SECTOR_COUNT			0x02
#define IDE_ATA_COMMAND_BLOCK_REG_OFFSET_SECTOR_NUMBER			0x03
#define IDE_ATA_COMMAND_BLOCK_REG_OFFSET_CYLINDER_LOW			0x04
#define IDE_ATA_COMMAND_BLOCK_REG_OFFSET_CYLINDER_HIGH			0x05
#define IDE_ATA_COMMAND_BLOCK_REG_OFFSET_DRIVE_HEAD			0x06
#define IDE_ATA_COMMAND_BLOCK_REG_OFFSET_STATUS_COMMAND			0x07

#define IDE_ATA_CONTROL_BLOCK_REG_OFFSET_RESERVED_1			0x00
#define IDE_ATA_CONTROL_BLOCK_REG_OFFSET_RESERVED_2			0x01
#define IDE_ATA_CONTROL_BLOCK_REG_OFFSET_ALT_STATUS_DEVICE_CONTROL	0X02
#define IDE_ATA_CONTROL_BLOCK_REG_OFFSET_FORWARD_TO_ISA			0X03

#define IDE_ATA_PIO_COMMAND_READ_SECTOR					0x20
#define IDE_ATA_PIO_COMMAND_WRITE_SECTOR				0x30

#define IDE_ATA_PIO_STATUS_BSY						0x80
#define IDE_ATA_PIO_STATUS_DRDY						0X40
#define IDE_ATA_PIO_STATUS_DWF						0X20
#define IDE_ATA_PIO_STATUS_DSC						0X10
#define IDE_ATA_PIO_STATUS_DRQ						0X08
#define IDE_ATA_PIO_STATUS_CORR						0X04
#define IDE_ATA_PIO_STATUS_IDX						0X02
#define IDE_ATA_PIO_STATUS_ERR						0X01

#define PIO_STATUS(flag) (__io_read_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_STATUS_COMMAND) & flag)

/**
 * 
 * BUG:
 * It takes a lot of time until it sets DRQ.
 * Found in practice:
 * If read one sector, only checking BSY flag can make data valid.
 * If read more than one sectors, checking DRQ is neccessary.
 * 
 * WORKAROUND:
 * Only provide functionality to read one sector at one time.
 * 
 */
int pio_read_sector(void *buf, uint32_t lba/*, uint8_t sec_count*/)
{

	assert((lba & 0xf0000000) == 0);
	while(PIO_STATUS(IDE_ATA_PIO_STATUS_BSY));
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_ERROR_FEATURES, 0);
// 	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_SECTOR_COUNT, sec_count);
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_SECTOR_COUNT, 1);
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_SECTOR_NUMBER, lba);
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_CYLINDER_LOW, lba >> 8);
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_CYLINDER_HIGH, lba >> 16);
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_DRIVE_HEAD, lba >> 24 | 1 << 6);
	
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_STATUS_COMMAND,  IDE_ATA_PIO_COMMAND_READ_SECTOR);	

	if(PIO_STATUS(IDE_ATA_PIO_STATUS_ERR))
		return __io_read_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_ERROR_FEATURES);
// 	while(sec_count -- > 0)
// 	{
		while(PIO_STATUS(IDE_ATA_PIO_STATUS_BSY));
// 		while(!PIO_STATUS(IDE_ATA_PIO_STATUS_DRQ));
		for (int i = 0; i < 256; i ++, buf += 2)
		{
			*(uint16_t *)buf = __io_read_16(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_DATA);
		}
// 	}
	return 0;
}

int pio_write_sector(void *buf, uint32_t lba)
{
	assert((lba & 0xf0000000) == 0);
	while(PIO_STATUS(IDE_ATA_PIO_STATUS_BSY));
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_ERROR_FEATURES, 0);
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_SECTOR_COUNT, 1);
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_SECTOR_NUMBER, lba);
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_CYLINDER_LOW, lba >> 8);
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_CYLINDER_HIGH, lba >> 16);
	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_DRIVE_HEAD, lba >> 24 | 1 << 6);
	

	__io_write_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_STATUS_COMMAND,  IDE_ATA_PIO_COMMAND_WRITE_SECTOR);	
	
	if(PIO_STATUS(IDE_ATA_PIO_STATUS_ERR))
		return __io_read_8(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_ERROR_FEATURES);
	
	while(PIO_STATUS(IDE_ATA_PIO_STATUS_BSY));
// 	while(!PIO_STATUS(IDE_ATA_PIO_STATUS_DRQ));
	for (int i = 0; i < 256; i ++, buf += 2)
	{
		__io_write_16(IDE_ATA_PRIMARY_COMMAND_BLOCK_OFFSET + IDE_ATA_COMMAND_BLOCK_REG_OFFSET_DATA, *(uint16_t *)buf);
	}
	return 0;
}
