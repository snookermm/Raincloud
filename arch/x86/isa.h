#ifndef _ISA_H_
#define _ISA_H_

void timer_82c54_init(uint8_t ch, uint8_t mode,  uint16_t counter);
void pic_8259_init(uint8_t vec_base);
void pic_8259_mask_int(uint8_t vec);
void pic_8259_unmask_int(uint8_t vec);
void pio_read_sector(void *buf, uint32_t lba);
void pio_write_sector(void *buf, uint32_t lba);


#endif
