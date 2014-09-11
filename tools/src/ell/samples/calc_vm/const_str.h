#ifndef __CONST_STR_H__
#define __CONST_STR_H__

#include <stdint.h>

#ifdef __DEF_CONST_STR__

const char const_str_buf[] = {
	"\0"
	"log2\0"
	"print\0"
	"rand\0"
	"sqrt\0"
	"write\0"
	"writeln\0"
	"x\0"
	"y\0"
	"z\0"
};

const struct str_pool const_str_pool = {
	.buf = (char *)const_str_buf,
	.cnt = 10,
	.offs = {
	0,
	1,
	6,
	12,
	17,
	22,
	28,
	36,
	38,
	40,
	}
};

#else 

extern const char const_str_buf[];

extern const struct str_pool const_str_pool;

#endif

#define NM_LOG2         1
#define NM_PRINT        2
#define NM_RAND         3
#define NM_SQRT         4
#define NM_WRITE        5
#define NM_WRITELN      6
#define NM_X            7
#define NM_Y            8
#define NM_Z            9

#endif /* __CONST_STR_H__ */

