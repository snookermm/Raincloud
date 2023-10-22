#include <klibc.h>
#include <serial_port/serial_port.h>

void __serial_port_print(char *str)
{
	serial_port_write(str, strlen(str));
}
void early_print_init(void)
{
	serial_port_init();
	__stdout = &__serial_port_print;
}
