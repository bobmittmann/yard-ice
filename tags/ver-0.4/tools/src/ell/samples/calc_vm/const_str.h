#ifndef __CONST_STR_H__
#define __CONST_STR_H__

#include <stdint.h>

#ifdef __DEF_CONST_STR__

const char const_str_buf[] = {
	"\0"
	"log2\0"
	"print\0"
	"printf\0"
	"rand\0"
	"sqrt\0"
	"srand\0"
	"time\0"
	"write\0"
	"writeln\0"
	"x\0"
	"y\0"
	"z\0"
};

const struct str_pool const_str_pool = {
	.buf = (char *)const_str_buf,
	.cnt = 13,
	.offs = {
	0,
	1,
	6,
	12,
	19,
	24,
	29,
	35,
	40,
	46,
	54,
	56,
	58,
	}
};

#else 

extern const char const_str_buf[];

extern const struct str_pool const_str_pool;

#endif

#define NM_LOG2         1
#define NM_PRINT        2
#define NM_PRINTF       3
#define NM_RAND         4
#define NM_SQRT         5
#define NM_SRAND        6
#define NM_TIME         7
#define NM_WRITE        8
#define NM_WRITELN      9
#define NM_X            10
#define NM_Y            11
#define NM_Z            12

#define CONST_STRINGS_MAX 13

#endif /* __CONST_STR_H__ */

