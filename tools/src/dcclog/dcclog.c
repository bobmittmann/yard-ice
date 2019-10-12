/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	dcclog.c
 * Module:
 * Project:	ARM-DCC logger expander
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(C) 2005 Robinson Mittmann. All Rights Reserved.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#if defined(WIN32)
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <io.h>
  #include <fcntl.h>
  #ifndef in_addr_t
    #define in_addr_t uint32_t
  #endif
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netinet/tcp.h>
  #include <netdb.h>
  #include <pthread.h>
  #include <assert.h>
#endif

#include "arm_elf.h"
#include "dcc.h"

#include "debug.h"

#define VERSION_MAJOR 5
#define VERSION_MINOR 0

char * prog;
int verbose = 0;

int hexdump_g1(FILE * f, uint32_t  addr, const void * buf, int size);
int hexdump(FILE * f, unsigned int addr, void * ptr, unsigned int count);

void print_mem_blocks(int level)
{
	int i;

	if (level <= 0)
		return;

	printf(" - mem blocks: (%d)\n", mem_count);
	printf("       start:        end:     size:\n");
	for (i = 0; i < mem_count; i++) {
		printf("   0x%08x  0x%08x  %8d\n", mem[i].addr, 
			   mem[i].addr + mem[i].size - 1, mem[i].size);
		if (level > 2) {
			hexdump(stdout, mem[i].addr, mem[i].image, mem[i].size);
		}
	}
}

int hexdump_mem_block(FILE * f, unsigned int idx)
{

	if (idx >= mem_count)
		return -1;
	
	printf("       start:        end:     size:\n");
	printf("   0x%08x  0x%08x  %8d\n", mem[idx].addr, 
		   mem[idx].addr + mem[idx].size - 1, 
		   mem[idx].size);

	return hexdump(stdout, mem[idx].addr, mem[idx].image, mem[idx].size);
}

int dump_dcclog(int level)
{
	unsigned int i;

	if (level <= 0)
		return 0;

	printf(" - dcc entries: (%d)\n", dcc_count);
	printf("       addr:    file  line  lvl   opt    func     msg\n") ;
	for (i = 0; i < dcc_count; i++) {
		struct dcclog_entry * p;

		if ((p = image_ptr(dcc[i].addr)) == NULL) {
			fprintf(stderr, " * %s: dcc[%d].addr invalid!\n", __func__, i);
			return -1;
		}

		printf(" 0x%08x: 0x%08x  %6d %3d 0x%02x 0x%08x 0x%08x\n",
			   dcc[i].addr,
			   (uint32_t)p->file, p->line, p->level, p->opt, 
			   (uint32_t)p->function, (uint32_t)p->msg);
	}

	return 0;
}

void print_dcc_entries(int level)
{
	int file_width = 0;
	int function_width = 0;
	struct log_def * log;
	char * lvl;
	int w;
	int i;

	if (level <= 0)
		return;

	printf(" - dcc entries: (%d)\n", dcc_count);
	for (i = 0; i < dcc_count; i++) {
		log = &dcc[i];
		lvl = log_level(&dcc[i]);
		if ((lvl == NULL) || (log->fmt == NULL)) {
			/* ERROR: ...*/
			return;
		}
		w = strlen(log->file);
		file_width = (file_width >= w) ? file_width : w; 
		w = strlen(log->function);
		function_width = (function_width >= w) ? function_width : w; 
	}

	if (level > 1) {
		printf("%7s: %*s:%4s %*s: %s:\n", 
			   "level", file_width, "file", "line", 
			   function_width, "function", "message" ); 
		for (i = 0; i < dcc_count; i++) {
			log = &dcc[i];

			lvl = log_level(&dcc[i]);

			if ((lvl == NULL) || (log->fmt == NULL)) {
				return;
			}
			printf(" %7s %*s:%-4d  %*s '%s'\n", lvl, 
				   file_width, log->file, log->line, 
				   function_width, log->function, log->fmt);
		}
	} else {
		printf("  level: %*s: %s:\n", function_width, 
			   "function", "message");
		for (i = 0; i < dcc_count; i++) {
			log = &dcc[i];

			lvl = log_level(&dcc[i]);

			if ((lvl == NULL) || (log->fmt == NULL)) {
				return;
			}
			printf(" %7s  %*s '%s'\n", lvl, 
				   function_width, log->function, log->fmt);
		}
	}
}

void print_symbols(int level)
{
	int i;

	if (level <= 0)
		return;

	printf(" - symbols: (%d)\n", sym_count);
	printf("        addr:    size:  name:\n");
	for (i = 0; i < sym_count; ++i) {
		printf("   0x%08x  %7d  %s\n", sym[i].addr, sym[i].size, sym[i].name);
	}
}

/*
 * Adjust the address offset and possibly endianness of
 * log entries
 * TODO: endianness
 */
int fix_dcclog(void) 
{
	struct log_def * log;
	struct dcclog_entry * entry;
	int i;

	for (i = 0; i < dcc_count; i++) {
		log = &dcc[i];

		if ((entry = image_ptr(log->addr)) == NULL) {
			fprintf(stderr, " * %s: log->addr invalid!\n", __func__);
			return -1;
		}

		if ((log->fmt = image_ptr(entry->msg)) == NULL) {
			fprintf(stderr, " *%s: (0x%08x) entry->msg invalid! \n", 
					__func__, log->addr);
			return -2;
		}

		if ((log->function = image_ptr(entry->function)) == NULL) {
			fprintf(stderr, " * %s: (0x%08x) entry->function invalid!\n", 
					__func__, log->addr);
			return -3;
		}

		if ((log->file = image_ptr(entry->file)) == NULL) {
			fprintf(stderr, " * %s: (0x%08x) entry->file invalid!\n", 
					__func__, log->addr);
			return -4;
		}

		log->line = entry->line;
		log->level = entry->level;
		log->opt = entry->opt;
	}

	return 0;
}

int dcccompar(const void * p1, const void * p2)
{
	uint32_t addr1 = ((struct log_def *)p1)->addr;
	uint32_t addr2 = ((struct log_def *)p2)->addr;

	return (int32_t)(addr2 - addr1);
}

int sort_dcclog(void) 
{
	qsort(dcc, dcc_count, sizeof(struct dcclog_entry ), dcccompar);

	return 0;
}

uint32_t ulrevbits(uint32_t v)
{
	// swap odd and even bits
	v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
	// swap consecutive pairs
	v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
	// swap nibbles ... 
	v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
	// swap bytes
	v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
	// swap 2-byte long pairs
	v = ( v >> 16             ) | ( v               << 16);

	return v;
}

int ultobin_msb_first(char * s, uint32_t val)
{
	int i;
	int n = 0;

	for (i = 31; i >=0; --i) {
		if (val & (1 << 31))
			break;
		val <<= 1;
	}

	for (n = 0; i >= 0; --i, ++n) {
		s[n] = (val & (1 << 31)) ? '1' : '0';
		val <<= 1;
	}

	s[n] = '\0';

	return n;
}

int ultobin_lsb_first(char * s, uint32_t val, int width)
{
	int i;

	for (i = 0; (i < 32) && val; ++i) {
		s[i] = (val & 1) ? '1' : '0';
		val >>= 1;
	}

	for (; (i < 32) && (i < width); ++i) {
		s[i] = '0';
	}

	s[i] = '\0';

	return i;
}


#define	GOT_PERCENT     0x80
#define	ZERO_FILL       0x40
#define	JUST_LEFT       0x20
#define	CONVERT_ALT     0x10
#define	GOT_DOT         0x08

#define IP4_ADDR1(a)    (((in_addr_t) (a)) & 0xff)
#define IP4_ADDR2(a)    ((((in_addr_t) (a)) >> 8) & 0xff)
#define IP4_ADDR3(a)    ((((in_addr_t) (a)) >> 16) & 0xff)
#define IP4_ADDR4(a)    ((((in_addr_t) (a)) >> 24) & 0xff)

int dcc_logprintf(struct dcc_lnk * lnk, unsigned int opt, const char *fmt)
{
	char c;
	char flags;
	char sign = 0;
	int width = 0;
	int precision = 0;
	char buf[80];
	union {
		char * cp;
		uint32_t n;
		uint8_t b[4];
		float f;
	} val;
	int n = 0;
	int m = 0;

	flags = 0;
	for (;(c = *fmt++);) {

		if (!flags) {
			if (c == '%') {
				flags = GOT_PERCENT;
				width = 0;
				sign = 0;
				fflush(stdout);
			} else {
				putchar(c);
				n++;
			}
			continue;
		}
		switch (c) {

		case 'c':
			c = dcc_read_u32(lnk);
			m++;
		default:
			putchar(c);
			n++;
			flags = 0;
			width = 0;
			precision = 6;
			continue;

		case 's':
			m++;
			if (opt == LOG_OPT_STR) {
				if ((val.cp = dcc_read_str(lnk)) == NULL) {
					printf("[INVALID STRING]");
					flags = 0;
					continue;
				}
			} else {
				if ((val.cp = dcc_read_ptr(lnk)) == NULL) {
					printf("[INVALID POINTER]");
					flags = 0;
					continue;
				}
			}

			width -= (int)strlen(val.cp);
			if (!(flags & JUST_LEFT)) {
				for(; width > 0; width--) {
					putchar(' ');
					n++;
				}
			}
			while ((c = *(val.cp)++)) {
				putchar(c);
				n++;
			}
			if ((flags & JUST_LEFT)) {
				for(; width > 0; width--) {
					putchar(' ');
					n++;
				}
			}
			flags = 0;
			continue;

		case 'l':
			continue;

		case '0':
			if (!(flags & GOT_DOT) && (width == 0))
				flags |= ZERO_FILL;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (flags & GOT_DOT)
				precision = (precision * 10) + (c - '0');
			else
				width = (width * 10) + (c - '0');
			continue;

		case '-':
			flags |= JUST_LEFT;
			continue;

		case '.':
			flags |= GOT_DOT;
			precision = 0 ;
			continue;

		case '+':
			sign = '+';
			continue;

		case 'd':
		case 'i':
			val.n = dcc_read_u32(lnk);
			if (((int32_t)val.n) < 0) {
				val.n = - val.n;
				sign = '-';
			}
			if (sign)
				width--;
			val.n = sprintf(buf, "%d", (int)val.n);
			m++;
			break;

		case 'u':
			val.n = dcc_read_u32(lnk);
			val.n = sprintf(buf, "%u", val.n);
			m++;
			break;

		case 'b':
			val.n = dcc_read_u32(lnk);
			val.n = ultobin_msb_first(buf, val.n);
			m++;
			break;

		case 'B':
			val.n = dcc_read_u32(lnk);
			val.n = ultobin_lsb_first(buf, val.n, width);
			m++;
			break;

		/* IPV4 address */			
		case 'I':
			val.n = dcc_read_u32(lnk);
			val.n = sprintf(buf, "%d.%d.%d.%d", 
				IP4_ADDR1(val.n), IP4_ADDR2(val.n), 
				IP4_ADDR3(val.n), IP4_ADDR4(val.n));
			m++;
			break;

		/* Network byte order short */			
		case 'S':
			val.n = dcc_read_u32(lnk);
			val.n = sprintf(buf, "%d", ntohs(val.n)); 
			m++;
			break;

		/* Network byte order long */			
		case 'L':
			val.n = dcc_read_u32(lnk);
			val.n = sprintf(buf, "%lu", (long int)ntohl(val.n)); 
			m++;
			break;

		/* Memory dump */			
		case 'M':
			val.n = dcc_read_u32(lnk);
			val.n = sprintf(buf, "%02x %02x %02x %02x", val.b[0],
							val.b[1], val.b[2], val.b[3]); 
			m++;
			break;

		case 'f':
			val.f = dcc_read_float(lnk);
			val.n = sprintf(buf, "%.*f", precision, val.f); 
			m++;
			break;

		case 'p':
			width = 8;
			flags |= ZERO_FILL;
			val.n = dcc_read_u32(lnk);
			val.n = sprintf(buf, "%x", val.n);
			m++;
			break;

		case 'x':
		case 'X':
			val.n = dcc_read_u32(lnk);
			val.n = sprintf(buf, "%x", val.n);
			m++;
			break;
		}

		if (flags & ZERO_FILL) {
			c = '0';
			if (sign) {
				putchar(sign);
				n++;
			}
			sign = 0; 
			flags &= ~JUST_LEFT;
		} else
			c = ' ';

		width -= (int)val.n;
		if (!(flags & JUST_LEFT)) {
			for(; width > 0; width--) {
				putchar(c);
				n++;
			}
		}

		if (sign) {
			putchar(sign);
			n++;
		}

		val.cp = buf;
		while ((c = *(val.cp)++)) {
			putchar(c);
			n++;
		}

		if (flags & JUST_LEFT) {
			for(; width > 0; width--) {
				putchar(' ');
				n++;				
			}
		}

		flags = 0;

		fflush(stdout);
	}

	return m;
}

int dcc_log_expand(struct dcc_lnk * lnk)
{
	struct log_def * log;
	unsigned int n;
	unsigned int m = 0;
	uint32_t addr;
	char * lvl;
	char * fmt;
	int sync;
	int i;

	n = 0;
	sync = 0;
	for (;;) {

		if (dcc_read_addr(lnk, &addr) < 0) {
			fprintf(stderr, "Can't read DCC entry: 0x%08x\n", addr);
			fflush(stderr);
			break;
		}
	
		fmt = NULL;
		lvl = NULL;
		for (i = 0; i < dcc_count; i++) {
			log = &dcc[i];

			if (log->addr == addr) {
				lvl = log_level(log);
				fmt = log->fmt;
				break;
			}
		}

		if (fmt == NULL) {
			if (sync) {
				printf("%d\n", m);
				m = 0;
				sync = 0;
			}
			printf(".");
			continue;
		}

		if (!sync) {
			sync = 1;
			printf("\n");
		}

		m++;

		if (verbose > 0) {
			printf("%6d %7s: ", n, lvl);
			if (verbose > 1) {
				printf("%s:%4d ", log->file, log->line);
			}
			printf("%s: ", log->function);
			fflush(stdout);
		} else {
			printf("%7s: %s: ", lvl, log->function);
			fflush(stdout);
		}

		if (log->opt == LOG_OPT_XXD) {
			uint32_t len;
			uint8_t * buf;
			printf("%s:\n", log->fmt);
			fflush(stdout);

			DBG(DBG_TRACE, "XXD");
			len = dcc_read_u32(lnk);
			if ((buf = dcc_read_bin(lnk, len)) != NULL) {
				hexdump_g1(stdout, 0, buf, len);
			}
		} else if (log->opt <= LOG_OPT_STR) {
			dcc_logprintf(lnk, log->opt, log->fmt);
			putchar('\n');
			fflush(stdout);
		}
		n++;
	}

	return 0;
}

void usage(char * prog)
{
	fprintf(stderr, "Usage: %s [OPTION...] [ELF APPS]\n", prog);
	fprintf(stderr, "Expand (filter out) a DCC LOG....\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  -?     \tShow this help message\n");
	fprintf(stderr, "  -l FILE\tLog file\n");
	fprintf(stderr, "  -h HOST\thost address (TCP connection)\n");
	fprintf(stderr, "  -p PORT\thost port (TCP connection)\n");
	fprintf(stderr, "  -d[d]  \tDump DCC entries\n");
	fprintf(stderr, "  -m[m]  \tDump memory blocks\n");
	fprintf(stderr, "  -s[s]  \tDump symbols\n");
	fprintf(stderr, "  -v[v]  \tVerbosity level\n");
	fprintf(stderr, "  -V     \tPrint version\n");
	fprintf(stderr, "\n");
	exit(0);
}

void version(char * prog)
{
	fprintf(stderr, "dcclog %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
	fprintf(stderr, "(C)Copyright BORESTE (www.boreste.com)\n");
	exit(1);
}

void parse_err(char * prog, char * opt)
{
	fprintf(stderr, "%s: invalid option %s\n", prog, opt);
	exit(1);
}

static int stdin_pipe_proc(FILE * stream)
{
	char buf[130];

	while (fgets(buf, 128, stdin) != NULL) {
		if (fputs(buf, stream) == EOF) {
			perror("pipe");
			break;
		}
		fflush(stream);
	};

	return 0;
}

#ifdef _WIN32
typedef HANDLE __thread_t;
#else
typedef pthread_t * __thread_t;
#endif

int __thread_create(__thread_t * pthread, void *(* task)(void*), void * arg)
{
	__thread_t thread;
	int ret;

#ifdef _WIN32
	unsigned threadId;
	unsigned ( __stdcall *func)(void *);
	
	func = (unsigned (__stdcall *)(void *))task;

	thread = (HANDLE)_beginthreadex(NULL, 0, func, arg, 0, &threadId);
	ret = (thread == (HANDLE)-1L) ? -1 : 0;
#else
	assert(pthread != NULL);	
	thread = malloc(sizeof(pthread_t));

	if ((ret = pthread_create(thread, NULL,
							  (void * (*)(void *))task,
							  (void *)arg)) < 0) {
		fprintf(stderr, "%s: pthread_create() failed: %s.\n",
				__func__, strerror(ret));
		fflush(stderr);
	}
#endif

	*pthread = thread;

	return ret;
}

int __thread_cancel(__thread_t thread)
{
#ifdef _WIN32
	TerminateThread(thread, 0);
	return 0;
#else
	return pthread_cancel(*thread);
#endif
}

int __thread_join(__thread_t thread, void ** value_ptr)
{
#ifdef _WIN32
	WaitForSingleObject(thread, INFINITE);
	return 0;
#else
	return pthread_join(*thread, value_ptr);
#endif
}

int __thread_destroy(__thread_t thread)
{
#ifdef _WIN32
	return 0;
#else
	free(thread);
	return 0;
#endif
}

__thread_t pipe_thread;

int pipe_start(FILE * stream)
{
	int ret;

	if ((ret = __thread_create(&pipe_thread,
							   (void * (*)(void *))stdin_pipe_proc,
							   (void *)stream)) < 0) {
		perror("__thread_create() failed.");
	}

	return ret;
}

void cleanup(void)
{
//	printf("cleanup...\n");
//	fflush(stdout);
	if (pipe_thread) {
		__thread_cancel(pipe_thread);
		__thread_destroy(pipe_thread);
		pipe_thread = NULL;
	}
}

void sig_quit(int signo)
{
	printf("\n");
	fflush(stdout);
	cleanup();
	exit(3);
}

int load_elf(char * pathname)
{
	char * shstrtab;
	char * symstrtab = NULL;
	Elf32_Ehdr * ehdr;
	Elf32_Shdr * shdr;
	Elf32_Phdr * phdr;
	Elf32_Sym * sym = NULL;
	char * section;
	char * name;
	FILE * f;
	int i;
	int n;

	ehdr = malloc(sizeof(Elf32_Ehdr));

	/* load binary data and symbols from elf file */
	if ((f = arm_elf_open(pathname, ehdr)) == NULL) {
		return -1;
	}

	if ((phdr = arm_elf_read_program_headers(f, ehdr)) == NULL) {
		printf(" #error: %s: arm_elf_read_program_headers()\n", __func__);
		return -6;
	}

	if (verbose) {
		arm_elf_show_program_headers(ehdr, phdr);
		fflush(stdout);
	}

	for (i = 0; i < ehdr->e_phnum; i++) {
		if (phdr[i].p_offset >= 0) {
			mem[mem_count].image = arm_elf_load_program(f, ehdr, &phdr[i]);
			if (mem[mem_count].image == NULL) {
				printf(" #error: %s: arm_elf_load_program() failed!\n", 
					   __func__);
				printf("         program header(%d): "
					   "offset=%d memsz=%d vaddr=%08x\n",  
					   i, phdr[i].p_offset, phdr[i].p_memsz, phdr[i].p_vaddr);
				return -5;
			}
			mem[mem_count].size = phdr[i].p_memsz;
			mem[mem_count].addr = phdr[i].p_vaddr;
			mem_count++;
		}
	}

	if ((shdr = arm_elf_read_section_headers(f, ehdr))  == NULL) {
		printf(" #error: %s: arm_elf_read_section_headers()\n", __func__);
		return -7;
	}

//	printf(" 2. %s: image_size=%ld\n", __func__, image_size);
//	fflush(stdout);

	if ((ehdr->e_shstrndx > 0) && (ehdr->e_shstrndx < ehdr->e_shnum)) {
		/* load section headers string table */
		shstrtab = arm_elf_load_strings(f, &shdr[ehdr->e_shstrndx]);
	} else {
		printf(" #error: %s: no section headers string table\n", __func__);
		return -1;
	};
	
	if (verbose) {
		arm_elf_show_section_headers(ehdr, shdr, shstrtab);
		fflush(stdout);
	}

	for (i = 0; i < ehdr->e_shnum; i++) {
		/* look for the symbol table section */
		if (shdr[i].sh_type == SHT_SYMTAB) {
			sym = arm_elf_load_symbols(f, &shdr[i], &n);
			if ((shdr[i].sh_link > 0) && (shdr[i].sh_link < ehdr->e_shnum)) {
				/* load section headers string table */
				symstrtab = arm_elf_load_strings(f, &shdr[shdr[i].sh_link]);
				break;
			}
			printf(" #error: %s: no symbol string table\n", __func__);
			return -1;
		}
	}

	fclose(f);

	if (sym == NULL) {
		printf(" #error: %s: no symbol table\n", __func__);
		return -1;
	}

	for (i = 0; i < n; i++) {
		if ((sym[i].st_shndx == SHN_UNDEF) || 
			(sym[i].st_shndx > SHN_LORESERVE)) {
			section = "*ABS*";
		} else {
			section = &shstrtab[shdr[sym[i].st_shndx].sh_name];
		}

		if (sym[i].st_name == 0)
			continue;

		name = &symstrtab[sym[i].st_name];

		if (name[0] == '$')
			continue;

		add_sym(section, name, sym[i].st_value, sym[i].st_size); 
	}

	free(ehdr);
	free(shdr);
	free(phdr);
	free(sym);

	return n;
}

/* Should be called from the main(). This will
 initialize signals, and signal handlers. 

 Alert: The application cannot use SIGALRM as the interval timer 
 depends on it to work.
 
 */
/* global cleanup callback */
static void (* __term_handler)(void) = NULL;

#ifdef _WIN32

BOOL CtrlHandler(DWORD fdwCtrlType) 
{ 
	switch (fdwCtrlType) { 
	case CTRL_C_EVENT: // Handle the CTRL-C signal. 
	case CTRL_BREAK_EVENT: 
	case CTRL_CLOSE_EVENT: 
		if (__term_handler != NULL) {
			__term_handler();
			return FALSE; 
		} else {
			return FALSE; 
		}

	default: 
		return FALSE; 
	} 
} 

static void __termination_handler(int signum)
{
	printf("That was all, folks\n");
	fflush(stdout);
}

#else

static void __abort_handler(int signum)
{
	const char msg[] = "\n!!! ABORTED !!!\n";
	int ret = write(STDERR_FILENO, msg, strlen(msg));
	(void)ret;
	_exit(4);
}

static void __termination_handler(int signum)
{
	struct sigaction new_action;

	/* Redirect the signal handlers to the abort handler */
	new_action.sa_handler = __abort_handler;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;

	sigaction(SIGINT, &new_action, NULL);
	sigaction(SIGTERM, &new_action, NULL);
	sigaction(SIGQUIT, &new_action, NULL);

	if (__term_handler != NULL) {
		__term_handler();
	} 

	exit(3);
}

#endif

void __term_sig_handler(void (* handler)(void))
{       
#ifdef _WIN32
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE)) { 
		/* Register a cleanup callback routine */
		__term_handler = handler;
	} else {
#ifdef __MINGW32__
		__term_handler = handler;
		signal(SIGINT, __termination_handler);
		signal(SIGTERM, __termination_handler);
		signal(SIGBREAK, __termination_handler);
#endif
	}
#else
	sigset_t set;
	struct sigaction new_action;

	/* Register a cleanup callback routine */
	__term_handler = handler;

	/* Disable SIGALRM signal wich is used by the interval timer. 
	   Only one thread can have this signal enabled. */
	sigemptyset(&set);
//	sigaddset(&set, SIGALRM);
	pthread_sigmask(SIG_BLOCK, &set, NULL);

	/* Configure the common termination handlers to call
	   the cleanup routine.  */
	new_action.sa_flags = SA_NODEFER;
	new_action.sa_handler = __termination_handler;
	sigaction(SIGINT, &new_action, NULL);
	sigaction(SIGTERM, &new_action, NULL);
	sigaction(SIGQUIT, &new_action, NULL);
#endif
}

int main(int argc, char *argv[])
{
	char appfname[1024];
	char host[128];
	int port = 1001;
	char * logfname = "-";
	int showsym = 0;
	int showdcc = 0;
	int showmem = 0;
	int host_set = 0;
	int log_set = 0;
	struct dcc_lnk lnk;
	int c;
	time_t t;
	FILE * f;

	/* the prog name start just after the last lash */
	if ((prog = (char *)strrchr(argv[0], '/')) == NULL)
		prog = argv[0];
	else
		prog++;

	/* parse the command line options */
	while ((c = getopt(argc, argv, "V?dmsvh:l:")) > 0) {
		switch (c) {
			case 'V':
				version(prog);
				break;

			case '?':
				usage(prog);
				break;

			case 'd':
				showdcc++;
				break;

			case 's':
				showsym++;
				break;

			case 'm':
				showmem++;
				break;

			case 'v':
				verbose++;
				break;

			case 'h':
				strcpy(host, optarg);
				host_set = 1;
				break;

			case 'p':
				port = strtoul(optarg, NULL, 0);
				break;
				
			case 'l':
				logfname = optarg;
				log_set = 1;
				break;

			default:
				parse_err(prog, optarg);
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "%s: missing APP name.\n\n", prog);
		usage(prog);
	}
	
	printf("\n== ARM DCC log viewer %d.%d ==\n", VERSION_MAJOR, VERSION_MINOR);
	fflush(stdout);
	 
	while (optind < argc) {
		strcpy(appfname, argv[optind]);
		printf(" - elf: %s\n", appfname);
		fflush(stdout);
		/* if elf_fd is set then force loading the elf file */
		if (load_elf(appfname) < 0) {
			return 1;
		} 
		optind++;
	}

#if defined(WIN32)
#else
	signal(SIGINT, sig_quit);
	signal(SIGTERM, sig_quit);
	signal(SIGQUIT, sig_quit);
	signal(SIGABRT, sig_quit);
#endif

	print_mem_blocks(showmem);
	print_symbols(showsym);

	dump_dcclog(showdcc * verbose);

	if (fix_dcclog() < 0) {
		return 3;
	}

	print_dcc_entries(showdcc);

	if (showsym || showmem || showdcc) {
		return 0;
	}

	__term_sig_handler(cleanup);

	if (log_set) {
		if (strcmp(logfname, "-") == 0) {
			f = stdin;
		} else {
			if (!(f = fopen(logfname, "r"))) {
				fprintf(stderr, "%s: can't open file %s\n", prog, logfname);
				return 4;
			};
		}

		dcc_file_link(&lnk, f);
	} else {
		if (host_set) {
			if ((net_connect(&lnk, host, port)) < 0) {
				fprintf(stderr, "%s: can't connect to '%s:%d'\n", prog, 
						host, port);
				return 5;
			}
			printf(" - host: %s:%d (connected).\n", host, port);
			fflush(stdout);
		} else {
			f = stdin;
			dcc_file_link(&lnk, f);
		}
	}

	t = time(NULL);
	printf(" - log date: %s\n", asctime(localtime(&t)));
	printf("\n");
	fflush(stdout);

	if (dcc_log_expand(&lnk) < 0) {
		cleanup();
		return 3;
	}

	cleanup();

	return 0;
}

