#include<klibc.h>


/**
 * Convert a 32-bit unicode charactor to a utf-8 coded charactor
 */
int unicode_into_utf8(uint32_t unicode, char *utf8)
{
	uint64_t size;
	if(unicode <= 0x7f)
	{
		*utf8 = (unicode & 0x7f);
		size=1;
	}
	if(0x80 <= unicode && unicode <= 0x7ff)
	{
		*(utf8+1) = (unicode & 0x3f) | 0x80;
		*utf8 =((unicode>>6) & 0x1f) | 0xc0;
		size=2;
	}
	if(0x800 <= unicode && unicode <= 0xffff)
	{
		*(utf8+2) = (unicode & 0x3f) | 0x80;
		*(utf8+1) =((unicode>>6) & 0x3f) | 0x80;
		*utf8 =((unicode>>12) & 0x0f) | 0xe0;
		size=3;
	}
	if(0x10000 <= unicode && unicode <= 0x10ffff)
	{
		*(utf8+3) = (unicode & 0x3f) | 0x80;
		*(utf8+2) =((unicode>>6) & 0x3f) | 0x80;
		*(utf8+1) =((unicode>>12) & 0x3f) | 0x80;
		*utf8 =((unicode>>18) & 0x07) | 0xf0;
		size=4;
	}
	if(0x200000 <= unicode && unicode <= 0x3ffffff)
	{
		*(utf8+4) = (unicode & 0x3f) | 0x80;
		*(utf8+3) =((unicode>>6) & 0x3f) | 0x80;
		*(utf8+2) =((unicode>>12) & 0x3f) | 0x80;
		*(utf8+1) =((unicode>>18) & 0x3f) | 0x80;
		*utf8 =((unicode>>24) & 0x03) | 0xf8;
		size=5;
	}
	if(0x4000000 <= unicode && unicode<= 0x7fffffff)
	{
		*(utf8+5) = (unicode & 0x3f) | 0x80;
		*(utf8+4) =((unicode>>6) & 0x3f) | 0x80;
		*(utf8+3) =((unicode>>12) & 0x3f) | 0x80;
		*(utf8+2) =((unicode>>18) & 0x3f) | 0x80;
		*(utf8+1) =((unicode>>24) & 0x3f) | 0x80;
		*utf8 =((unicode>>30) & 0x01) | 0xfc;
		size=6;
	}

	return size;
	
}
/**
 * Convert a 16-bit unicode string to a utf-8 coded string
 */
int str_unicode_to_utf8(uint16_t *unicode, char *utf8)
{
	uint64_t sum_size=0;
	for(; *unicode!= 0x0000; unicode++)
	{
		sum_size += unicode_into_utf8(*unicode,utf8 + sum_size);
	}
	*(utf8 + sum_size) = '\0';
	return sum_size;
}


