#ifdef	__ASSERT_H__

#undef	__ASSERT_H__
#undef	assert

#endif /* __ASSERT_H__ */

#define	__ASSERT_H__

#ifdef __cplusplus 
#define __ASSERT_VOID_CAST static_cast<void>
#else
#define __ASSERT_VOID_CAST (void)
#endif

#define __CONCAT(x,y) x ## y
#define __STRING(x)	#x

#ifdef	NDEBUG

#define assert(EXPR)		(void)0

#include <sys/dcclog.h>

#else /* Not NDEBUG.  */

/* This prints an "Assertion failed" message and aborts.  */
extern void __assert_fail(const char *__assertion, const char *__file, 
						  unsigned int __line, const char *__function) 
			__attribute__ ((__noreturn__));

#ifndef	assert
#define assert(EXPR) ((EXPR) ? (void)0 : \
					  __assert_fail(__STRING(EXPR), \
									__FILE__, __LINE__, __func__))
#endif

#endif /* NDEBUG.  */

