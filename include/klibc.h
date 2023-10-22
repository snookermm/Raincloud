#ifndef _KLIBC_H_
#define _KLIBC_H_

#include<types.h>
/**
 * This is the routine of standard C library for use in the kernel.
 */

//String
/**
 * ASCII to integer convention
 * @str interger string
 * @retval integer the string represents
 * 
 */
int atoi(const char *str);
/**
 * Similar function as standard string functions.
 * @str the string to evaluate length of
 * @return the length of the string,
 */
int strlen(const char *str);
/**
 * String copy function,with a limit of the nubmer of characters.
 * @dst destination buffer
 * @src source string
 * @retval dst destination buffer
 * 
 */
char *strcat(char *dst, const char *src);
/**
 * String copy function,with a limit of the nubmer of characters.
 * @dst destination buffer
 * @src source string
 * @retval dst destination buffer
 * 
 */
char *strcpy(char *dst, const char *src);
/**
 * String copy function,with a limit of the nubmer of characters.
 * @dst destination buffer
 * @src source string
 * @n   maximum number of characters in total
 * @retval pointer to dst destination buffer
 * 
 */
char *strncpy(char *dst, const char *src, uint64_t n);
/**
 * Wide characters' string copy function,with a limit of the nubmer of characters.
 * @dst destination buffer
 * @src source string
 * @n   maximum number of characters in total
 * @retval pointer to dst destination buffer
 * 
 */
wchar *wstrncpy(wchar *dst, const wchar *src, uint64_t n);
/**
 * String compare function.
 * @s1 @s2 string to compare
 * @retval 1 if s1 is longer than s2 
 *         0 if s1 is as long as s2
 *         -1 if s1 is shorter than s2
 */
int strcmp(const char *s1, const char *s2);

/* Parse S into tokens separated by characters in DELIM. 
 * If S is NULL, the saved pointer in SAVE_PTR is used as 
 * the next starting point.  For example: 
 *      char s[] = "-abc-=-def"; 
 *      char *sp; 
 *      x = strtok_r(s, "-", &sp);      // x = "abc", sp = "=-def" 
 *      x = strtok_r(NULL, "-=", &sp);  // x = "def", sp = NULL 
 *      x = strtok_r(NULL, "=", &sp);   // x = NULL 
 *              // s = "abc\0-def\0" 
**/  
char *strtok_r(char *s, const char *delim, char **save_ptr);
/**
 * strchr, strrchr
 * locate the first (last) occurrence of character c in the string s.
 * @s string
 * @c character(one byte)
 * @retval pointer to the character in the string.
 */
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);

/**
 * strspn, strcspn
 * get length of a prefix substring
 * @s string
 * @accept char set to accept
 * @reject char set to reject
 */
size_t strspn(const char *s, const char *accept);
size_t strcspn(const char *s, const char *reject);

//Memory
/**
 * Use @ch to initialize @buf of size @count bytes.
 * @buf pointer to the buffer to be initialized
 * @ch content to be used to initialize
 * @count size of the buffer
 * @retval pointer to the buffer
 */
void *memset(void *dest, char ch, uint64_t n);
/**
 * Copy @count bytes data from @src to @dest.
 * @dest pointer to destination buffer
 * @src pointer to souce buffer
 * @n size of data to be copied
 * @retval pointer to dest
 */
void *memcpy(void *dest, const void *src, uint64_t n);
/**
 * strpbrk
 * search a string for any of a set of bytes
 * @s string
 * @accept set of bytes to locate
 * @retval pointer to the byte in @s that matches
 */
char *strpbrk(const char *s, const char *accept);
/**
 * Convert unicode string to utf-8 string
 * @unicode input
 * @utf8 output
 * @retval size of string to be converted(not including NULL terminator)
 */
int str_unicode_to_utf8(wchar *unicode, char *utf8);
/**
 * Allocate continuous @size bytes of a array.
 * @size number of bytes to be allocted
 * @retval pointer to the allocated memory,NULL on error
 */
extern void *malloc(uint64_t size);
/**
 * Free memory allocated by kmalloc.
 * @ptr pointer to allocated memory
 */
extern void free(void *ptr);

//Input/Output
extern void (*__stdout)(char *str);
/**
 * Formatted output fuction
 * @fmt format string
 * @buf buffer to hold string
 * @... (optional)multiple variable to output
 * @retval number of characters outputted
 * Note:
 * 	Current implementation of printf only support a subset
 * of format conversion specifiers including:%d, %s, %c, %x
 * TODO:
 * 	Add support for more functions.
 */
extern int printf(const char *fmt, ...);
extern int sprintf(char *buf, const char *fmt, ...);
//Debug
/**
 * To help programmers find bugs.
 * If the exp is false,the kernel will panic.
 * TODO:Add the support to print line number and expression.
 * @exp expression to check
 */
#ifdef NDEBUG
#define assert(expr) (void)(0)
#else
#define assert(expr)							\
	((expr)								\
	? (void)(0)							\
	: __assert_fail (#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__))
extern void __assert_fail(const char *__assertion, const char *__file, unsigned int __line, const char *__function);
#endif

#define bug(fmt,arg...)	__bug(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##arg)
extern void __bug(const char *__file, unsigned int __line, const char *__function,const char *fmt, ...);


#endif
