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
	"sleep\0"
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
	.cnt = 14,
	.offs = {
	0,
	1,
	6,
	12,
	19,
	24,
	30,
	35,
	41,
	46,
	52,
	60,
	62,
	64,
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
#define NM_SLEEP        5
#define NM_SQRT         6
#define NM_SRAND        7
#define NM_TIME         8
#define NM_WRITE        9
#define NM_WRITELN      10
#define NM_X            11
#define NM_Y            12
#define NM_Z            13

#define CONST_STRINGS_MAX 14

#endif /* __CONST_STR_H__ */

