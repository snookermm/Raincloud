#include <klibc.h>
#include <cpu_intrins.h>
extern void (*__stdout)(char *str);

void __assert_fail(const char *__assertion, const char *__file, unsigned int __line, const char *__function)
{
	char buf[1024];
	strcpy(buf, __file);
	strcat(buf, ": ");
// 	strcat(buf, __line);
	sprintf(buf + strlen(buf), "%d", __line);
	strcat(buf, ": ");
	strcat(buf, __function);
	strcat(buf, ": ");
	strcat(buf, "Assertion \'");
	strcat(buf, __assertion);
	strcat(buf, "\' failed.\n");
	(*__stdout)(buf);
// 	traceback();
	__halt__();
}
