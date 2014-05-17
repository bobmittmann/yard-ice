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

#if defined(WIN32)
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #ifndef in_addr_t
    #define in_addr_t uint32_t
  #endif
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netinet/tcp.h>
  #include <netdb.h>
#endif

#include <pthread.h>

#include "arm_elf.h"

#define VERSION_MAJOR 2
#define VERSION_MINOR 0

#define LVL_PANIC   1
#define LVL_EXCEPT  2
#define LVL_ERROR   3
#define LVL_WARNING 4
#define LVL_TRACE   5
#define LVL_INFO    6
#define LVL_MSG     7

const char * level_tab[] = {
	"NONE",
	"PANIC",
	"EXCEPT",
	"ERROR",
	"WARNING",
	"TRACE",
	"INFO",
	"MSG"
};

struct dcclog_entry {
	uint32_t file;
	uint16_t line;
	uint8_t level;
	uint8_t opt;
	uint32_t function;
	uint32_t msg;
};

struct log_def {
	uint32_t addr;
	int level;
	char * file;
	unsigned int line;
	char * function;
	char * fmt;
};

struct sym_def {
	uint32_t addr;
	int      size;
	uint8_t  type;
	uint8_t  binding;
	uint8_t  options;
	uint8_t  flags;
	char name[128];
};

struct mem_def {
	uint32_t addr;
	uint32_t size;
	uint32_t options;
	uint8_t * image;
};

char * prog;
int verbose = 0;

/* log entries */ 
struct log_def dcc[16384];
int dcc_count = 0;

/* symbol table */ 
struct sym_def sym[16384];
int sym_count = 0;

/* memory block content */ 
struct mem_def mem[64];
int mem_count = 0;

struct sym_def * sym_lookup(uint32_t addr) 
{
	int i;

	for (i= 0; i < sym_count; i++) {
		if ((addr >= sym[i].addr) && (addr < sym[i].addr + sym[i].size)) {
			return &sym[i];
		}
	}

	return NULL;
}

struct sym_def * get_sym_by_name(const char * name) 
{
	int i;

	for (i= 0; i < sym_count; i++) {
		if (strcmp(sym[i].name, name) == 0) {
			return &sym[i];
		}
	}

	return NULL;
}

struct sym_def * get_sym(int i) 
{
	if (!sym_count)
		return NULL;

	return &sym[i];
}

int add_sym(char * section, char * name, uint32_t addr, uint32_t size)
{

	if (section == NULL)
		return -1;

	if (name == NULL)
		return -1;
	
	if ((strcmp(section, ".dcclog")  == 0) && 
		(size == sizeof(struct dcclog_entry))) {
		if (verbose) {
			if (verbose > 1) {
				printf("+log: ");
			}
			printf("%12s  %08x %6d %s\n", section, addr, size, name); 
			fflush(stdout);
		}
		dcc[dcc_count++].addr = addr;
		return 0;
	}

	if ((strcmp(section, ".text")  == 0) || 
		(strcmp(section, ".init")  == 0)) {
		if (verbose) {
			if (verbose > 1) {
				printf("+log: ");
			}
			printf("%12s  %08x %6d %s\n", section, addr, size, name); 
			fflush(stdout);
		}
		sym[sym_count].addr = addr;
		sym[sym_count].size = size;
		strcpy(sym[sym_count].name, name);
		sym_count++; 
		return 0;
	} 

	if (verbose) {
		if (verbose > 1) {
			printf("-   : ");
		}
		printf("%12s  %08x %6d %s\n", section, addr, size, name); 
		fflush(stdout);
	}
	return -1;
}

char * log_level(struct log_def * log)
{
	int level;

	if (log == NULL) {
		fprintf(stderr, "%s:%s log is NULL.\n", prog, __FUNCTION__);
		return NULL;
	}

	level = log->level;

	if ((level < LVL_PANIC) && (level > LVL_MSG)) {
		fprintf(stderr, "%s:%s invalid level: %d.\n", 
				__FILE__, __FUNCTION__, level);
		return NULL;
	}

	return (char *)level_tab[level];
}

int hexdump(FILE * f, unsigned int addr, void * ptr, unsigned int count);

void dump_syms(int level)
{
	int file_width = 0;
	int function_width = 0;
	struct log_def * log;
	char * lvl;
	int w;
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

	printf(" - symbols: (%d)\n", sym_count);
	printf("        addr:    size:  name:\n");
	for (i= 0; i < sym_count; i++) {
		printf("   0x%08x  %7d  %s\n", sym[i].addr, sym[i].size, sym[i].name);
	}
}

/*
 * Get an local pointer from target memory image
 */
void * image_ptr(uint32_t addr)
{
	int32_t offs;
	int i;


	for (i = 0; i < mem_count; i++) {
//		printf("%08x %8d <- %08x\n", mem[i].addr, mem[i].size, addr);
		offs = addr - mem[i].addr;
		if ((offs >= 0) && (offs < mem[i].size)) {
			return (void *) &mem[i].image[offs];
		}
	}

	fprintf(stderr, "ERROR: %s(): can't translate pointer %08x\n",
			__FUNCTION__, addr);
	return NULL;
}

/*
 * Adjust the address offset and possibly endianness of
 * log entries
 * TODO: endianness
 */
int fix_log(void) 
{
	struct log_def * log;
	struct dcclog_entry * entry;
	int i;

	for (i = 0; i < dcc_count; i++) {
		log = &dcc[i];

		if ((entry = image_ptr(log->addr)) == NULL)
			return -1;

		if ((log->file = image_ptr(entry->file)) == NULL)
			return -2;

		if ((log->function = image_ptr(entry->function)) == NULL)
			return -3;

		if ((log->fmt = image_ptr(entry->msg)) == NULL)
			return -4;

		log->line = entry->line;
		log->level = entry->level;
	}

	return 0;
}

int dcc_int(FILE *stream)
{
	int val;

	if (fread(&val, 4, 1, stream) != 1)
		return 0;

	return val;
}

unsigned int dcc_uint(FILE *stream)
{
	uint32_t val;

	if (fread(&val, 4, 1, stream) != 1)
		return 0;

	return val;
}

void * dcc_ptr(FILE *stream)
{
	uint32_t addr;

	if (fread(&addr, 4, 1, stream) != 1)
		return 0;

	return image_ptr(addr);
}

#define	GOT_PERCENT     0x80
#define	ZERO_FILL       0x40
#define	JUST_LEFT       0x20
#define	CONVERT_ALT     0x10

#define IP4_ADDR1(a)    (((in_addr_t) (a)) & 0xff)
#define IP4_ADDR2(a)    ((((in_addr_t) (a)) >> 8) & 0xff)
#define IP4_ADDR3(a)    ((((in_addr_t) (a)) >> 16) & 0xff)
#define IP4_ADDR4(a)    ((((in_addr_t) (a)) >> 24) & 0xff)

int logprintf(FILE *stream, const char *fmt)
{
	char c;
	char flags;
	char sign = 0;
	int width = 0;
	char buf[32];
	union {
		char * cp;
		uint32_t n;
		uint8_t b[4];
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
			c = dcc_int(stream);
			m++;
		default:
			putchar(c);
			n++;
			flags = 0;
			width = 0;
			continue;

		case 's':
			m++;
			if ((val.cp = dcc_ptr(stream)) == NULL) {
				printf("[INVALID POINTER]");
				flags = 0;
				continue;
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
			if (width == 0)
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
			width = (width * 10) + (c - '0');
			continue;

		case '-':
			flags |= JUST_LEFT;
			continue;

		case '+':
			sign = '+';
			continue;

		case 'd':
		case 'i':
			val.n = dcc_int(stream);
			if (((int)val.n) < 0) {
				val.n = - val.n;
				sign = '-';
			}
			if (sign)
				width--;
			val.n = sprintf(buf, "%d", (int)val.n);
			m++;
			break;

		case 'u':
			val.n = dcc_uint(stream);
			val.n = sprintf(buf, "%u", val.n);
			m++;
			break;

		/* IPV4 address */			
		case 'I':
			val.n = dcc_uint(stream);
			val.n = sprintf(buf, "%d.%d.%d.%d", 
				IP4_ADDR1(val.n), IP4_ADDR2(val.n), 
				IP4_ADDR3(val.n), IP4_ADDR4(val.n));
			m++;
			break;

		/* Network byte order short */			
		case 'S':
			val.n = dcc_uint(stream);
			val.n = sprintf(buf, "%d", ntohs(val.n)); 
			m++;
			break;

		/* Network byte order long */			
		case 'L':
			val.n = dcc_uint(stream);
			val.n = sprintf(buf, "%lu", ntohl(val.n)); 
			m++;
			break;

		/* Memory dump */			
		case 'M':
			val.n = dcc_uint(stream);
			val.n = sprintf(buf, "%02x %02x %02x %02x", val.b[0],
							val.b[1], val.b[2], val.b[3]); 
			m++;
			break;

		case 'p':
			width = 8;
			flags |= ZERO_FILL;
			val.n = dcc_uint(stream);
			val.n = sprintf(buf, "%x", val.n);
			m++;
			break;

		case 'x':
		case 'X':
			val.n = dcc_uint(stream);
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

/*
 * This is the main function of the application...
 *
 */

int dcc_log_expand(FILE * f)
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
		if (fread(&addr, 4, 1, f) != 1)
			break;
	
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
		} else {
			printf("%7s: %s: ", lvl, log->function);
		}

//		fflush(stdout);
		logprintf(f, log->fmt);
		putchar('\n');
//		fflush(stdout);
		n++;
	}

	if (f != stdin)
		fclose(f);

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
	fprintf(stderr, "  -d[d]  \tDump symbols\n");
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

pthread_t * pipe_thread = NULL;

int pipe_start(FILE * stream)
{
	pipe_thread = (pthread_t *)malloc(sizeof(pthread_t));

	if (pthread_create(pipe_thread, NULL, 
				   (void * (*)(void *))stdin_pipe_proc, 
				   (void *)stream) != 0) {
		perror("pipe_open");
		return -1;
	}

	return 0;
}

void cleanup(void)
{
//	printf("cleanup...\n");
//	fflush(stdout);
	if (pipe_thread) {
		pthread_cancel(*pipe_thread);
		free(pipe_thread);
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

int net_connect(char * host, int port)
{
	int fd;
	struct hostent * hp;
	struct sockaddr_in sa;
	in_addr_t addr;
	int opt;

#if defined(WIN32)
	WSADATA wsaData;

	/* initialize windows sockets */
	if (WSAStartup(MAKEWORD(2,1), &wsaData) != 0) {
		MessageBox(NULL, 
			"Fatal ERROR!\nUnable to initialize windows socket!",
			"DCC Client", MB_ICONERROR);
		return -1;
	}
#endif

	/* Configuracao do Socket como TCP */
	fd = socket(AF_INET, SOCK_STREAM, 0); 
	if (fd < 0) {
		fprintf(stderr, "%s: socket(): %s\n", prog, strerror(errno));
		return -1;
	}

	memset(&sa, 0, sizeof(struct sockaddr_in));

	if ((hp = gethostbyname(host)) != NULL) {
		memcpy(&(sa.sin_addr), hp->h_addr, hp->h_length);
		sa.sin_family = hp->h_addrtype;
	} else {
		if ((addr = inet_addr(host)) == INADDR_NONE) {
			fprintf(stderr, "%s: Unknown host %s.\n", prog, host);
			return -1;
		}
		sa.sin_addr.s_addr = addr;
		sa.sin_family = AF_INET;
	}

	sa.sin_port = htons(port);

	if (connect(fd, (struct sockaddr *)&sa, sizeof(sa))) { 
		fprintf(stderr, "%s: connect(): %s\n", prog, strerror(errno));
		return -1;
	}

	opt = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(int));
#if 0
	setsockopt(fd, IPPROTO_TCP, TCP_QUICKACK, (void *)&opt, sizeof(int));
	send(fd, "\r", 1, 0);
#endif
	return fd;
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
	int fd;
	int i;
	int n;

	ehdr = malloc(sizeof(Elf32_Ehdr));

	/* load binary data and symbols from elf file */
	if ((fd = arm_elf_open(pathname, ehdr)) < 0) {
		return fd;
	}

	if ((phdr = arm_elf_read_program_headers(fd, ehdr)) == NULL) {
		printf(" #error: %s: arm_elf_read_program_headers()\n", __FUNCTION__);
		return -6;
	}

	if (verbose) {
		arm_elf_show_program_headers(ehdr, phdr);
		fflush(stdout);
	}

	for (i = 0; i < ehdr->e_phnum; i++) {
		if (phdr[i].p_offset > 0) {
			mem[mem_count].image = arm_elf_load_program(fd, ehdr, &phdr[i]);
			mem[mem_count].size = phdr[i].p_memsz;
			mem[mem_count].addr = phdr[i].p_vaddr;
			mem_count++;
		}
	}

	if ((shdr = arm_elf_read_section_headers(fd, ehdr))  == NULL) {
		printf(" #error: %s: arm_elf_read_section_headers()\n", __FUNCTION__);
		return -7;
	}

//	printf(" 2. %s: image_size=%ld\n", __FUNCTION__, image_size);
//	fflush(stdout);

	if ((ehdr->e_shstrndx > 0) && (ehdr->e_shstrndx < ehdr->e_shnum)) {
		/* load section headers string table */
		shstrtab = arm_elf_load_strings(fd, &shdr[ehdr->e_shstrndx]);
	} else {
		printf(" #error: %s: no section headers string table\n", __FUNCTION__);
		return -1;
	};
	
	if (verbose) {
		arm_elf_show_section_headers(ehdr, shdr, shstrtab);
		fflush(stdout);
	}

	for (i = 0; i < ehdr->e_shnum; i++) {
		/* look for the symbol table section */
		if (shdr[i].sh_type == SHT_SYMTAB) {
			sym = arm_elf_load_symbols(fd, &shdr[i], &n);
			if ((shdr[i].sh_link > 0) && (shdr[i].sh_link < ehdr->e_shnum)) {
				/* load section headers string table */
				symstrtab = arm_elf_load_strings(fd, &shdr[shdr[i].sh_link]);
				break;
			}
			printf(" #error: %s: no symbol string table\n", __FUNCTION__);
			return -1;
		}
	}

	close(fd);

	if (sym == NULL) {
		printf(" #error: %s: no symbol table\n", __FUNCTION__);
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

int main(int argc, char *argv[])
{
	extern char *optarg;	/* getopt */
	extern int optind;	/* getopt */
	char appfname[1024];
	char host[128];
	int port = 1001;
	char * logfname = "-";
	int dump = 0;
	int host_set = 0;
	int log_set = 0;
	int c;
	int fd;
	FILE * f;

	/* the prog name start just after the last lash */
	if ((prog = (char *)strrchr(argv[0], '/')) == NULL)
		prog = argv[0];
	else
		prog++;

	/* parse the command line options */
	while ((c = getopt(argc, argv, "V?dvh:l:")) > 0) {
		switch (c) {
			case 'V':
				version(prog);
				break;

			case '?':
				usage(prog);
				break;

			case 'd':
				dump++;
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
	
	while (optind < argc) {
		strcpy(appfname, argv[optind]);
		printf(" - elf: %s\n", appfname);
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

	if (fix_log() < 0) {
		cleanup();
		return 3;
	}

	if (dump) {
		cleanup();
		dump_syms(dump);
		return 0;
	}

	printf("== ARM DCC log viewer ==\n\n");

	if (log_set) {
		if (strcmp(logfname, "-") == 0) {
			f = stdin;
		} else {
			if (!(f = fopen(logfname, "r"))) {
				fprintf(stderr, "%s: can't open file %s\n", prog, logfname);
				return 4;
			};
		}
	} else {
		if (host_set) {
			printf(" - host: %s:%d\n", host, port);
			if ((fd = net_connect(host, port)) < 0) {
				return 5;
			}
			f = fdopen(fd, "r");
//			pipe_start(fdopen(fd, "w"));
		} else {
			f = stdin;
		}
	}

	printf("\n");

	if (dcc_log_expand(f) < 0) {
		cleanup();
		return 3;
	}

	cleanup();

	return 0;
}

