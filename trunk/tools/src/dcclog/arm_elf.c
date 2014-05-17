/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	arm_elf.c
 * Module:
 * Project:	ARM-DCC logger expander
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2005-2009 Robinson Mittmann. All Rights Reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>

#include <errno.h>
#include <sys/types.h>

#include <elf.h>

#if defined(WIN32)
int ffs(int i);
#endif

static const char * p_type_tab[] = {
	/* Program header table entry unused */
	"NULL",
	/* Loadable program segment */
	"LOAD",
	/* Dynamic linking information */
	"DYNAMIC",
	/* Program interpreter */
	"INTERP",
	/* Auxiliary information */
	"NOTE",
	/* Reserved */
	"SHLIB",
	/* Entry for header table itself */
	"PHDR",
	/* Thread-local storage segment */
	"TLS",
	/* Number of defined types */
	"NUM"
};

static const char * p_flags_tab[] = {
	"---",
	"--x",
	"-w-",
	"-wx",
	"r--",
	"r-x",
	"rw-",
	"rwx"
};

static void show_prog_entry(Elf32_Phdr * p)
{
	char type[16];
	char flags[16];

	if (p->p_type > PT_NUM)
		sprintf(type, "0x%08x", p->p_type);
	else
		strcpy(type, p_type_tab[p->p_type]);

	if (p->p_flags > 0x07)
		sprintf(flags, "0x%08x", p->p_flags);
	else
		strcpy(flags, p_flags_tab[p->p_flags]);

	printf(" %8s off 0x%08x vaddr 0x%08x paddr 0x%08x align 2**%d\n"
		   "    filesz 0x%08x memsz 0x%08x flags %s\n",
		   type, p->p_offset, p->p_vaddr, p->p_paddr, ffs(p->p_align) - 1,
		   p->p_filesz, p->p_memsz, flags);
}

static const char * sht_type_tab[] = {
	"NULL", /* Section header table entry unused */
	"PROGBITS", /* Program data */
	"SYMTAB", /* Symbol table */
	"STRTAB", /* String table */
	"RELA", /* Relocation entries with addends */
	"HASH", /* Symbol hash table */
	"DYNAMIC", /* Dynamic linking information */
	"NOTE", /* Notes */
	"NOBITS", /* Program space with no data (bss) */
	"REL", /* Relocation entries, no addends */
	"SHLIB", /* Reserved */
	"DYNSYM", /* Dynamic linker symbol table */
	"INIT_ARRAY", /* Array of constructors */
	"FINI_ARRAY", /* Array of destructors */
	"PREINIT_ARRAY", /* Array of pre-constructors */
	"GROUP", /* Section group */
	"SYMTAB_SHNDX", /* Extended section indeces */
	"NUM", /* Number of defined types.  */
};

static const char * sht_flag_tab[] = {
	"WRITE", /* Writable */
	"ALLOC", /* Occupies memory during execution */
	"EXECINSTR", /* Executable */
	" 3",
	"MERGE", /* Might be merged */
	"STRINGS", /* Contains nul-terminated strings */
	"INFO_LINK", /* `sh_info' contains SHT index */
	"LINK_ORDER", /* Preserve order after combining */
	"OS_NONCONFORMING", /* Non-standard OS specific handling required */
	"GROUP", /* Section is member of a group.  */
	"TLS" /* Section hold thread-local data.  */
};

static void show_section_entry(Elf32_Shdr * p, char * strtab)
{
	char type[16];
	char flags[128] = "";
	char * s;
	char * name;
	int i;

	if (p->sh_type > SHT_NUM)
		sprintf(type, "0x%08x", p->sh_type);
	else
		strcpy(type, sht_type_tab[p->sh_type]);

	if (p->sh_flags > ((SHF_TLS << 1) - 1)) {
		sprintf(flags, "%0x08x", p->sh_flags);
	} else {
		s = flags;
		for (i = 0; i < 32; i++) {
			if (p->sh_flags & (1 << i)) {
				s += sprintf(s, " %s", sht_flag_tab[i]);
			}
		}
	}

	if (strtab != NULL) {
		name = &strtab[p->sh_name];
		if (*name == '\0')
			name = type;
	} else {
		name = type;
	}

	printf("%12s  %08x  %08x  %08x  %08x  2**%d\n"
		   "             %s  %0d\n",
		   name, p->sh_size, p->sh_addr, p->sh_info, p->sh_offset, 
		   ffs(p->sh_addralign) - 1, flags, p->sh_link);
}

void arm_elf_show_program_headers(Elf32_Ehdr * ehdr, Elf32_Phdr * phdr)
{
	int i;

	for (i = 0; i < ehdr->e_phnum; i++) {
		printf(" %d ", i);
		fflush(stdout);
		show_prog_entry(phdr++);
	}
}

void * arm_elf_load_program(FILE * f, Elf32_Ehdr * ehdr, Elf32_Phdr * phdr)
{
	uint8_t * ptr;
	ssize_t ret;

	//ffs(p->p_align) - 1,
	/* TODO: alignement */
	fseek(f, phdr->p_offset, SEEK_SET);

	if (ftell(f) != phdr->p_offset) {
		fprintf(stderr, "ERROR: %s: ftell()=%ld != %d\n",
				__func__, ftell(f), phdr->p_offset);
		return NULL;
	}

	ptr = malloc(phdr->p_memsz);

	if ((ret = fread(ptr, phdr->p_memsz, 1, f)) != 1) {
		fprintf(stderr, "ERROR: %s: fread(%d): %s.\n",
				__func__, phdr->p_memsz, strerror(errno));
		fprintf(stderr, "ERROR: %s: ftell()=%ld != %d\n",
				__func__, ftell(f), phdr->p_offset);
		free(ptr);
		return NULL;
	}

	return ptr;
}

char * arm_elf_load_strings(FILE * f, Elf32_Shdr * shdr)
{
	char * str;
	int ret;

	fseek(f, shdr->sh_offset, SEEK_SET);

	if (ftell(f) != shdr->sh_offset) {
		fprintf(stderr, "ERROR: %s: fseek()...\n", __func__);
		return NULL;
	}

	str = malloc(shdr->sh_size);

	if ((ret = fread(str, shdr->sh_size, 1, f)) != 1) {
		fprintf(stderr, "ERROR: %s: fread(): %s.\n",
				__func__, strerror(errno));
		free(str);
		return NULL;
	}

	return str;
}

void arm_elf_show_section_headers(Elf32_Ehdr * ehdr, Elf32_Shdr * shdr, 
								  char * strtab)
{
	int i;

	printf("Idx Name          Size       VMA       LMA       File off  Algn\n");

	for (i = 0; i < ehdr->e_shnum; i++) {
		if (shdr->sh_type != SHT_NULL) {
			printf(" %2d ", i);
			fflush(stdout);
			show_section_entry(shdr, strtab);
		}
		shdr++;
	}
}


const char * st_bind_tab[] = {
	"LOCAL", /* Local symbol */
	"GLOBAL", /* Global symbol */
	"WEAK", /* Weak symbol */
	" 4", /* Number of defined types.  */
	" 5",
	" 6",
	" 7",
	" 8",
	" 9",
	"10",
	"11",
	"12",
	"13",
	"14",
	"15"
};

/* Legal values for ST_TYPE subfield of st_info (symbol type).  */
const char * st_type_tab[] = {
	"NOTYPE", /* Symbol type is unspecified */
	"OBJECT", /* Symbol is a data object */
	"FUNC", /* Symbol is a code object */
	"SECTION", /* Symbol associated with a section */
	"FILE", /* Symbol's name is file name */
	"COMMON", /* Symbol is a common data object */
	"TLS", /* Symbol is thread-local data object*/
	" 7", /* Number of defined types.  */
	" 8",
	" 9",
	"10",
	"11",
	"THUMB",
	"13",
	"14",
	"15"
};


/* Symbol visibility specification encoded in the st_other field.  */
const char * st_visibility_tab[] = {
	"DEFAULT", /* Default symbol visibility rules */
	"INTERNAL", /* Processor specific hidden class */
	"HIDDEN", /* Sym unavailable in other modules */
	"PROTECTED" /* Not preemptible, not exported */
};

void arm_elf_show_symbols(Elf32_Shdr * shdr, char * shstr, 
						  Elf32_Sym * sym, char * symstr, int num)
{
	int i;
	char name[1014];
	char section[1024];
	char * flags;
	char type;
	char bind;
	int v;

//	printf("Idx Name          Size       VMA       LMA       File off  Algn\n");

	for (i = 0; i < num; i++) {
		if (symstr != NULL) {
			if (sym->st_name == 0) {
				if ((ELF32_ST_TYPE(sym->st_info) == STT_SECTION) &&
					(shstr != NULL)) {
					strcpy(name, &shstr[shdr[sym->st_shndx].sh_name]);
				} else {
					strcpy(name, "$");
				}
			} else {
				strcpy(name, &symstr[sym->st_name]);
			}
		} else {
			sprintf(name, "[%4d]", sym->st_name);
		}

		bind = "lgw3456789ABCDEF"[ELF32_ST_BIND(sym->st_info)];
		type = " OFSfc6789ABCDEF"[ELF32_ST_TYPE(sym->st_info)];
		(void)type;
		switch (ELF32_ST_TYPE(sym->st_info)) {
		case STT_NOTYPE:
			/* Symbol type is unspecified */
			flags = "  ";
			break;
		case STT_OBJECT:
			/* Symbol is a data object */
			flags = " O";
			break;
		case STT_FUNC:
			/* Symbol is a code object */
			flags = " F";
			break;
		case STT_SECTION:
			/* Symbol associated with a section */
			flags = "d ";
			break;
		case STT_FILE:
			/* Symbol's name is file name */
			flags = "df";
			break;
		case STT_COMMON:
			/* Symbol is a common data object */
			flags = "..";
			break;
		default:
			flags = "??";
		}

		if ((sym->st_shndx == SHN_UNDEF) || (sym->st_shndx > SHN_LORESERVE)) {
			strcpy(section, "*ABS*");
		} else {
			if (shstr != NULL) {
				strcpy(section, &shstr[shdr[sym->st_shndx].sh_name]);
			} else {
				sprintf(section, "[%2d]", sym->st_shndx);
			}
		}

		v = ELF32_ST_VISIBILITY(sym->st_other);
		(void)v;

		if (name[0] != '$') {
			printf("%08x %c    %s %-8s %08x %s\n", 
				   sym->st_value, bind, flags, section, sym->st_size, name);  
		}
		sym++;
	}
}

Elf32_Phdr * arm_elf_read_program_headers(FILE * f, Elf32_Ehdr * ehdr)
{
	int off = ehdr->e_phoff;
	Elf32_Phdr * phdr;
	int ret;
	int i;

	phdr = malloc(ehdr->e_phnum * sizeof(Elf32_Phdr));

	for (i = 0; i < ehdr->e_phnum; i++) {

		fseek(f, off, SEEK_SET);
		if ((ret = fread(&phdr[i], sizeof(Elf32_Phdr), 1, f)) != 1) {
			fprintf(stderr, "ERROR: %s: fread(): %s.\n",
					__func__, strerror(errno));
			free(phdr);
			return NULL;
		}

		off += ehdr->e_phentsize;
	}

	return phdr;
}

Elf32_Shdr * arm_elf_read_section_headers(FILE * f, Elf32_Ehdr * ehdr)
{
	int off = ehdr->e_shoff;
	Elf32_Shdr * shdr;
	int ret;
	int i;

	shdr = malloc(sizeof(Elf32_Shdr) * ehdr->e_shnum);

	for (i = 0; i < ehdr->e_shnum; i++) {

		fseek(f, off, SEEK_SET);
		if ((ret = fread(&shdr[i], sizeof(Elf32_Shdr), 1, f)) != 1) {
			fprintf(stderr, "ERROR: %s: fread(): %s.\n",
					__func__, strerror(errno));
			free(shdr);
			return NULL;
		}
		off += ehdr->e_shentsize;
	}

	return shdr;
}

Elf32_Sym * arm_elf_load_symbols(FILE * f, Elf32_Shdr * shdr, int * num)
{
	Elf32_Sym * sym;
	int ret;

	if (shdr->sh_type != SHT_SYMTAB) {
		fprintf(stderr, 
				"%s(): ASSERT: shdr->sh_type != SHT_SYMTAB\n",
				__func__);
		return NULL;
	}

	if (shdr->sh_entsize != sizeof(Elf32_Sym)) {
		fprintf(stderr, 
				"%s(): ASSERT: shdr->sh_entsize != sizeof(Elf32_Sym)\n",
				__func__);
		return NULL;
	}

	printf(" - symbols off: %08x  size: %d  entsize: %d link %08x\n", 
		   shdr->sh_offset, shdr->sh_size, shdr->sh_entsize,
		   shdr->sh_link);

	fseek(f, shdr->sh_offset, SEEK_SET);

	sym = malloc(shdr->sh_size);

	if ((ret = fread(sym, shdr->sh_size, 1, f)) != 1) {
		fprintf(stderr, "ERROR: %s: fread(): %s.\n",
				__func__, strerror(errno));
		free(sym);
		return NULL;
	}

	if (num != NULL)
		*num = shdr->sh_size / shdr->sh_entsize;

	return sym;
}

FILE * arm_elf_open(const char * pathname, Elf32_Ehdr * ehdr)
{
	FILE * f;
	int ret;

	if ((pathname== NULL) || (ehdr == NULL)) {
		fprintf(stderr, "ERROR: %s: invalid arguments.\n", __func__);
		return NULL;
	}

	if ((f = fopen(pathname, "rb")) == NULL) {
		fprintf(stderr, "ERROR: %s: open(): %s.\n",
				__func__, strerror(errno));
		return f;
	}

	if ((ret = fread(ehdr, sizeof(Elf32_Ehdr), 1, f)) != 1) {
		fprintf(stderr, "ERROR: %s: fread(): %s.\n",
				__func__, strerror(errno));
		fclose(f);
		return NULL;
	}

	if ((ehdr->e_ident[EI_MAG0] != ELFMAG0) ||
		(ehdr->e_ident[EI_MAG1] != ELFMAG1) ||
		(ehdr->e_ident[EI_MAG2] != ELFMAG2) ||
		(ehdr->e_ident[EI_MAG3] != ELFMAG3)) {
		fprintf(stderr, "ERROR: %s: not an ELF file.\n", __func__);
		/* Not ELF */
		fclose(f);
		return NULL;
	}

	if (ehdr->e_type != ET_EXEC) {
		fprintf(stderr, "ERROR: %s: not an executable file.\n", __func__);
		/* Not executable */
		fclose(f);
		return NULL;
	}

	if (ehdr->e_machine != EM_ARM) {
		fprintf(stderr, "ERROR: %s: not an ARM machine file.\n", __func__);
		/* Not executable */
		fclose(f);
		return NULL;
	}

	if ((ehdr->e_ident[EI_DATA] != ELFDATA2MSB) &&
		(ehdr->e_ident[EI_DATA] != ELFDATA2LSB)) {
		fprintf(stderr, "ERROR: %s: invalid data enconding.\n", 
				__func__);
		fclose(f);
		return NULL;
	}

	return f;
}

#if 0
int load_elf(char * pathname)
{
	FILE * f;
	int i;
	char * shstrtab;
	char * symstrtab = NULL;
	Elf32_Ehdr * ehdr;
	Elf32_Shdr * shdr;
	Elf32_Phdr * phdr;
	Elf32_Sym * sym;
	int n;

	ehdr = malloc(sizeof(Elf32_Ehdr));

	/* load binary data and symbols from elf file */
	if ((fd = arm_elf_open(pathname, ehdr)) < 0) {
		return fd;
	}

	printf(" - %s: %s endian.\n", __func__, 
		   (ehdr->e_ident[EI_DATA] == ELFDATA2MSB) ? "big" : "little");

	printf(" - %s: program header table: off=%08x entsize=%d num=%d\n", 
		   __func__, ehdr->e_phoff, ehdr->e_phentsize, ehdr->e_phnum);
	printf(" - %s: section header table: off=%08x entsize=%d num=%d\n", 
		   __func__, ehdr->e_shoff, ehdr->e_shentsize, ehdr->e_shnum);

	if ((phdr = arm_elf_read_program_headers(fd, ehdr)) == NULL) {
		printf(" #error: %s: arm_elf_read_program_headers()\n", __func__);
		return -6;
	}

	printf(" - %s 0.\n", __func__);
	fflush(stdout);

	if ((shdr = arm_elf_read_section_headers(fd, ehdr))  == NULL) {
		printf(" #error: %s: arm_elf_read_section_headers()\n", __func__);
		return -7;
	}

	printf(" - %s 1.\n", __func__);
	fflush(stdout);

	arm_elf_show_program_headers(ehdr, phdr);

	printf(" - %s 2.\n", __func__);
	fflush(stdout);

	if (ehdr->e_shstrndx > 0) {
		/* load section headers string table */
		shstrtab = arm_elf_load_strings(fd, &shdr[ehdr->e_shstrndx]);
	};

	arm_elf_show_section_headers(ehdr, shdr, shstrtab);

	printf(" - %s 3.\n", __func__);
	fflush(stdout);

	for (i = 0; i < ehdr->e_shnum; i++) {
		/* look for the symbol table section */
		if (shdr[i].sh_type == SHT_SYMTAB) {

			sym = arm_elf_load_symbols(fd, &shdr[i], &n);

			if (shdr[i].sh_link > 0) {
				/* load section headers string table */
				symstrtab = arm_elf_load_strings(fd, &shdr[shdr[i].sh_link]);
			};

			break;
		}
	}

	arm_elf_show_symbols(shdr, shstrtab, sym, symstrtab, n);

	close(fd);

	return 0;
}
#endif

/*
int arm_elf_read_sections(FILE * f, Elf32_Ehdr * ehdr, 
						  Elf32_Shdr * shdr)
{
	int off = ehdr->e_shoff;
	int ret;
	int i;
	char * str = NULL;

	for (i = 0; i < ehdr->e_shnum; i++) {
		if (shdr->sh_type == SHT_STRTAB) {
			printf(" - strings  off: %08x  size: %d\n", 
				   shdr->sh_offset, shdr->sh_size);
		}

		if (shdr->sh_type == SHT_SYMTAB) {
			printf(" - symbols off: %08x  size: %d  entsize: %d link %08x\n", 
				   shdr->sh_offset, shdr->sh_size, shdr->sh_entsize,
				   shdr->sh_link);
		}

		if ((shdr->sh_type == SHT_PROGBITS) && 
			(shdr->sh_flags & (SHF_ALLOC))) {
			printf(" - loadable off: %08x  size: %d  VMA: %08x\n", 
				   shdr->sh_offset, shdr->sh_size,  shdr->sh_addr);
		}

		shdr++;
	}

	return 0;
} */

