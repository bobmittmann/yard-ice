/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	arm-elf.h
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

#ifndef __ARM_ELF_H__
#define __ARM_ELF_H__

#include <elf.h>
#include <stdio.h>

void arm_elf_show_program_headers(Elf32_Ehdr * ehdr, Elf32_Phdr * phdr);

Elf32_Phdr * arm_elf_read_program_headers(FILE * f, Elf32_Ehdr * ehdr);

Elf32_Shdr * arm_elf_read_section_headers(FILE * f, Elf32_Ehdr * ehdr);

void * arm_elf_load_program(FILE * f, Elf32_Ehdr * ehdr, Elf32_Phdr * phdr);

FILE * arm_elf_open(const char * pathname, Elf32_Ehdr * ehdr);

void arm_elf_show_section_headers(Elf32_Ehdr * ehdr, Elf32_Shdr * shdr, 
								  char * strtab);

void arm_elf_show_symbols(Elf32_Shdr * shdr, char * shstr, 
						  Elf32_Sym * sym, char * symstr, int num);

char * arm_elf_load_strings(FILE * f, Elf32_Shdr * shdr);

Elf32_Sym * arm_elf_load_symbols(FILE * f, Elf32_Shdr * shdr, int * num);

#endif

