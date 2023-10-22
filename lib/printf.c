#include <klibc.h>
#include <stdarg.h>


static const char hex_table[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

char *__print_hex(char *buf, uint64_t hex, int num)
{
	while(num --> 0)
	{
		*(buf ++) = hex_table[hex >> (num * 4) & 0x0f];
	}
	return buf;
}
char *__print_deci(char *buf, int64_t deci)
{
	char temp_buf[21];
	int i = 0;
	int sign = 0;
	if (deci < 0)
	{
		sign = 1;
		deci = -deci;
	}
	do {
		temp_buf[i ++] = deci % 10 + '0';
		deci /= 10;
	} while(deci);
	if (sign == 1)
		temp_buf[i ++] = '-';
	while(i > 0)
		*buf ++ = temp_buf[-- i];
	return buf;
}
char *__print_str(char *buf, char *str)
{
	strcpy(buf, str);
	return buf + strlen(str);
}
char *__print_ch(char *buf, char ch)
{
	*buf = ch;
	return buf + 1;
}
int __print_format(char *buf, const char *fmt, va_list ap)
{
	char *ptr = buf;
//  	long long ll;
	unsigned long lx;
 	long ld;
	unsigned int x;
	int d;
	char c;
	char *s;
	while(*fmt)
	{
		if (*fmt == '%')
		{
			switch (*++fmt) {
				case 's':
				case 'S':
					s = va_arg(ap, char *);
					ptr = __print_str(ptr, s);
					break;
				case 'c':
				case 'C':
					c = (char)va_arg(ap, int);
					ptr = __print_ch(ptr, c);
					break;
				case 'd':
				case 'D':
					d = va_arg(ap, int);
					ptr = __print_deci(ptr, d);
					break;
				case 'x':
				case 'X':
					x = va_arg(ap, unsigned int);
					ptr = __print_hex(ptr, x, sizeof(unsigned int) * 2);
					break;
				case 'l':
				case 'L':
					switch (*++fmt) {
						case 'd':
						case 'D':
							ld = va_arg(ap, long);
							ptr = __print_deci(ptr, ld);
							break;
						case 'x':
						case 'X':
							lx = va_arg(ap, unsigned long);
							ptr = __print_hex(ptr, lx, sizeof(unsigned long) * 2);
							break;
	// 					case 'l':
	// 						break;
						default:
							break;
					}
					break;
				default:
					break;
			};
		}
		else
		{
			*ptr++ = *fmt;
		}
		fmt++;
	}
 	*ptr++ = '\0';
 	
	return ptr - buf;
}
int sprintf(char *buf, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int ret = __print_format(buf, fmt, ap);
	va_end(ap);
	return ret;
}
int printf(const char *fmt, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, fmt);
	int ret = __print_format(buf, fmt, ap);
	va_end(ap);
	(*__stdout)(buf);
	return ret;
}

