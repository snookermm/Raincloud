#ifndef _VGA_H_
#define _VGA_H_

//Register definitions

#define CRTC_CON_ADDR_REG	0x03B4
#define CRTC_CON_DATA_REG	0x03B5
#define INPUT_STATUS_REG1_R	0x03BA
#define FEAT_CON_REG_W		0x03BA
#define ATTRI_ADDR_REG		0x03C0
#define ATTRI_DATA_REG_W	0x03C0
#define ATTRI_DATA_REG_R	0x03C1
#define INPUT_STATUS_REG0_R	0x03C2
#define MISC_OUTPUT_REG_W	0x03C2
#define SEQU_ADDR_REG		0x03C4
#define SEQU_DATA_REG		0x03C5
#define DAC_STATE_REG_R		0x03C7
#define DAC_ADDR_RM_REG_W	0x03C7
#define DAC_ADDR_WM_REG		0x03C8
#define DAC_DATA_REG		0x03C9
#define FEAT_CON_REG_R		0x03CA
#define MISC_OUTPUT_REG_R	0x03CC
#define GRAPH_CON_ADDR_REG	0x03CE
#define GRAPH_CON_DATA_REG	0x03CF
// #define CRTC_CON_ADDR_REG	0x03D4
// #define CRTC_CON_DATA_REG	0x03D5
// #define INPUT_STATUS_REG1_R	0x03DA
// #define FEAT_CON_REG_W	0x03DA

#define CRTC_REG		0x00
#define GPAPHICS_REG		0x01
#define SEQUENCER_REG		0x02


#define GRAPHICS_SET_RESET_REG		0x00
#define GRAPHICS_ENABLE_SET_RESERT_REG	0x01
#define GRAPHICS_COLOR_COMPARE_REG	0x02
#define GRAPHICS_DATA_ROTATE_REG	0x03
#define GRAPHICS_READ_MAP_SELECT_REG	0x04
#define GRAPHICS_GRAPHICS_MODE_REG	0x05
#define GRAPHICS_MISC_GRAPHICS_REG	0X06
#define GRAPHICS_COLOR_DONT_CARE_REG	0X07
#define GRAPHICS_BIT_MASK_REG		0X08

#define SEQU_RESERT_REG			0x00
#define SEQU_CLOCKING_MODE_REG		0x01
#define SEQU_MAP_MASK_REG		0x02
#define SEQU_CHAR_MAP_SELECT_REG	0x03
#define SEQU_MEM_MODE_REG		0x04


#endif

