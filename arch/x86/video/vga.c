#include "vga.h"
#include <io.h>

static const uint16_t addr_reg[3] = {CRTC_CON_ADDR_REG, GRAPH_CON_ADDR_REG, SEQU_ADDR_REG};
static const uint16_t data_reg[3] = {CRTC_CON_DATA_REG, GRAPH_CON_DATA_REG, SEQU_DATA_REG};

static void __graph_reg_write(uint8_t reg_type, uint8_t addr, uint8_t data)
{
	uint8_t previous_addr = __io_read_8(addr_reg[reg_type]);
	__io_write_8(addr_reg[reg_type], addr);
	__io_write_8(data_reg[reg_type], data);
	__io_write_8(addr_reg[reg_type], previous_addr);
}
static uint8_t __graph_reg_read(uint8_t reg_type, uint8_t addr)
{
	uint8_t previous_addr = __io_read_8(addr_reg[reg_type]);
	__io_write_8(addr_reg[reg_type], addr);
	uint8_t read_data = __io_read_8(data_reg[reg_type]);
	__io_write_8(addr_reg[reg_type], previous_addr);
	return read_data;
}
int vga_init(void)
{
	__io_write_8(MISC_OUTPUT_REG_W, 0x02);
	__graph_reg_write(GPAPHICS_REG, GRAPHICS_MISC_GRAPHICS_REG, 0x00);
	__graph_reg_write(GPAPHICS_REG, GRAPHICS_GRAPHICS_MODE_REG, 0x00);
	__graph_reg_write(SEQUENCER_REG, SEQU_MAP_MASK_REG, 0x01);
	
// 	__io_write_16(0x3C4, 0x0100); /* synchronous reset */ 
// 	__io_write_8(0x3D4, 0x11); /* enable crtc regs 0-7 */ 
// 	__io_write_8(0x3D5, __io_read_8(0x3D5) & 0x7F); 
// 	__io_write_16(0x3C4, 0x0604); /* disable chain-4 */
}
