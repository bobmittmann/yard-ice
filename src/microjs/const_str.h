#ifndef __CONST_STR_H__
#define __CONST_STR_H__

#include <stdint.h>

#ifdef __DEF_CONST_STRBUF__

const struct {
	uint16_t cnt;
	uint16_t pos;
	uint16_t offs[14];
	char buf[];
} const_strbuf = {
	.cnt = 14, /* number of strings */
	.pos = 32, /* initial offset */
	.offs = { /* string offsets */
		32,
		33,
		38,
		44,
		51,
		56,
		62,
		67,
		73,
		78,
		84,
		92,
		94,
		96,
	},
	.buf = {
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
	}
};

#endif

#define NM_LOG2             1
#define NM_PRINT            2
#define NM_PRINTF           3
#define NM_RAND             4
#define NM_SLEEP            5
#define NM_SQRT             6
#define NM_SRAND            7
#define NM_TIME             8
#define NM_WRITE            9
#define NM_WRITELN          10
#define NM_X                11
#define NM_Y                12
#define NM_Z                13

#define CONST_STRINGS_MAX 14

#endif /* __CONST_STR_H__ */

