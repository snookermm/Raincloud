#include <klibc.h>
#include <stdarg.h>
int __print_format(char *buf, const char *fmt, va_list ap);
void __bug(const char *__file, unsigned int __line, const char *__function, const char *fmt, ...)
{
	char buf[1024];
	strcpy(buf, "BUG: ");
	strcat(buf, __file);
	strcat(buf, ": ");
	sprintf(buf + strlen(buf), "%d", __line);
	strcat(buf, ": ");
	strcat(buf, __function);
	strcat(buf, ": ");
	va_list ap;
	va_start(ap, fmt);
	__print_format(buf + strlen(buf), fmt, ap);
	va_end(ap);
	(*__stdout)(buf);
}
