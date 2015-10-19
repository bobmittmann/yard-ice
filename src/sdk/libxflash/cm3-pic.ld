/* 
 * arm-pic.ld
 *
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the ThinkOS library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org
 */

/* entry point at _start */
/* ENTRY(_start) */

/* output sections  */
SECTIONS 
{
	/* start the location counter */
	. = 0;             

  .interp         : { *(.interp) }
  .hash           : { *(.hash) }
  .dynsym         : { *(.dynsym) }
  .dynstr         : { *(.dynstr) }
  .gnu.version    : { *(.gnu.version) }
  .gnu.version_d  : { *(.gnu.version_d) }
  .gnu.version_r  : { *(.gnu.version_r) }
  .rel.dyn        :
    {
      *(.rel.init)
      *(.rel.text .rel.text.* .rel.gnu.linkonce.t.*)
      *(.rel.fini)
      *(.rel.rodata .rel.rodata.* .rel.gnu.linkonce.r.*)
      *(.rel.data.rel.ro* .rel.gnu.linkonce.d.rel.ro.*)
      *(.rel.data .rel.data.* .rel.gnu.linkonce.d.*)
      *(.rel.tdata .rel.tdata.* .rel.gnu.linkonce.td.*)
      *(.rel.tbss .rel.tbss.* .rel.gnu.linkonce.tb.*)
      *(.rel.ctors)
      *(.rel.dtors)
      *(.rel.got)
      *(.rel.bss .rel.bss.* .rel.gnu.linkonce.b.*)
    }

	.init :
	{
		*(.init)
	}

	/* Code sections */ 
	.text :          
	{
		*(.boot)
		*(.init .init.*) 
		/* all .text sections (code)  */
		*(.text)
		/* all .glue_7 sections (interworking arm helpers) */
		*(.glue_7)
		/* all .glue_7t sections (interworking thumb helpers) */
		*(.glue_7t)
		/* all .rodata sections (constants, strings, etc.)  */
		*(.rodata .rodata.*) 
	}
 
	/* initialized .data */ 
	.data :
	{
		/* all .data sections  */
		*(.data)
	}

	/* uninitialized .bss sections */
	.bss :
	{
		/* all .bss sections  */
		*(.bss)
	}
}

