#ifndef _TYPES_H_
#define _TYPES_H_

#define NULL 0

/* Exact integral types.  */

/* Signed.  */

/* There is some amount of overlap with <sys/types.h> as known by inet code */
#ifndef __int8_t_defined
# define __int8_t_defined
typedef signed char		int8_t;
typedef short int		int16_t;
typedef int			int32_t;
# if __WORDSIZE == 64
typedef long int		int64_t;
# else
__extension__
typedef long long int		int64_t;
# endif
#endif

/* Unsigned.  */
typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
#ifndef __uint32_t_defined
typedef unsigned int		uint32_t;
# define __uint32_t_defined
#endif
#if __WORDSIZE == 64
typedef unsigned long int	uint64_t;
#else
__extension__
typedef unsigned long long int	uint64_t;
#endif

/* Signed.  */
typedef signed char		INT8;
typedef short int		INT16;
typedef int			INT32;
#if __WORDSIZE == 64
typedef long int		INT64;
#else
__extension__
typedef long long int		INT64;
#endif


/* Unsigned.  */
typedef unsigned char		UINT8;
typedef unsigned short int	UINT16;
typedef unsigned int		UINT32;
#if __WORDSIZE == 64
typedef unsigned long int	UINT64;
#else
__extension__
typedef unsigned long long int	UINT64;
#endif

// typedef void			VOID;
// 
// #define IN
// #define OUT

typedef uint64_t 		size_t;
typedef int64_t			ssize_t;
typedef int32_t			id_t;
typedef uint8_t			atomic_t;
typedef int32_t			nice_t;
typedef uint16_t		wchar;
typedef int64_t			off_t;

#endif
