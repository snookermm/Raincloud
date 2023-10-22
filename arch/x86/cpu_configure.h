
/**
 * Enable ACPI timer.
 * @timer_mode one of the macros below
 * @vector     routine in the interrupt table 16-255
 */
#define acpi_timer_mode_one_shot      0x00
#define acpi_timer_mode_periodic      0x01
#define acpi_timer_mode_tsc_deadline  0x02

static inline void __enable_cpu_acpi_timer(uint8_t timer_mode, uint8_t vector)
{
	uint32_t *reg = (uint32_t *)0xffe00320;
	*reg = (timer_mode & 0x03) << 17 | vector;
	printf("%x\n", *reg);
}
/**
 * Configure ACPI timer.
 * @initial_count Auto reload value,32-bit unsigned integer is okay.
 * @divide        Frequency divider,0-7,indicating 2,4,8,16,32,64,128,1.
 */
static inline void __set_cpu_acpi_timer(uint32_t initial_count, uint8_t divide)
{
	uint32_t *reg = (uint32_t *)0xfee00380;
	*reg = initial_count;
	printf("%x\n", *reg);
	reg = (uint32_t *)0xfee003E0;
	*reg = (divide & 0x04) << 1 | (divide & 0x03);
	printf("%x\n", *reg);
}

