#!/usr/bin/python2

from math import *


def make_irq(f, num):

	f.write("/*\n")
	f.write(" * irq%d.S\n" % (num))
	f.write(" *\n")
	f.write(" * Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.\n")
	f.write(" *\n")
	f.write(" * This file is part of the YARD-ICE.\n")
	f.write(" *\n")
	f.write(" * This library is free software; you can redistribute it and/or\n")
	f.write(" * modify it under the terms of the GNU Lesser General Public\n")
	f.write(" * License as published by the Free Software Foundation; either\n")
	f.write(" * version 3.0 of the License, or (at your option) any later version.\n")
	f.write(" *\n")
	f.write(" * This library is distributed in the hope that it will be useful,\n")
	f.write(" * but WITHOUT ANY WARRANTY; without even the implied warranty of\n")
	f.write(" * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n")
	f.write(" * Lesser General Public License for more details.\n")
	f.write(" *\n")
	f.write(" * You can receive a copy of the GNU Lesser General Public License from\n")
	f.write(" * http://www.gnu.org/\n")
	f.write(" */\n")
	f.write("\n")
	f.write("\t.section .rom_vectors.%02x, \"ax\", %%progbits\n" % (num))
	f.write("\t.align  1\n")
	f.write("\t.global _irq%d_vec\n" % (num))
#	f.write("\t.type _irq%d_vec, %%object\n" % (num))
	f.write("\t.type _irq%d_vec, %%function\n" % (num))
	f.write("_irq%d_vec:\n" % (num))
	f.write("\t.word _irq%d_isr\n" % (num))
	f.write(".size _irq%d_vec, . - _irq%d_vec\n" % (num, num))
	f.write("\n")
	f.write(".end\n")

def main():

	i = 0
	lst = []

	while i < 128:
		nm = "cmx_irq%s.S" % i
#		print "- Generating %s..." % nm
		f = open(nm, "w+")
		make_irq(f, i)
		f.close()
		lst.append(nm)
		i = i + 1

	nm = "Makefile"
	print "- Generating %s..." % nm
	f = open(nm, "w+")

	f.write("#\n")
	f.write("# Copyright(C) 2013 Robinson Mittmann. All Rights Reserved.\n")
	f.write("# \n")
	f.write("# This file is part of the YARD-ICE.\n")
	f.write("#\n")
	f.write("# This library is free software; you can redistribute it and/or\n")
	f.write("# modify it under the terms of the GNU Lesser General Public\n")
	f.write("# License as published by the Free Software Foundation; either\n")
	f.write("# version 3.0 of the License, or (at your option) any later version.\n")
	f.write("# \n")
	f.write("# This library is distributed in the hope that it will be useful,\n")
	f.write("# but WITHOUT ANY WARRANTY; without even the implied warranty of\n")
	f.write("# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n")
	f.write("# Lesser General Public License for more details.\n")
	f.write("# \n")
	f.write("# You can receive a copy of the GNU Lesser General Public License from \n")
	f.write("# http://www.gnu.org/\n")
	f.write("\n")
	f.write("#\n")
	f.write("# File:   Makefile\n")
	f.write("# Author: Robinson Mittmann <bobmittmann@gmail.com>\n")
	f.write("# \n")
	f.write("\n")
	f.write("\n")
	f.write("LIB_STATIC = cmx\n")
	f.write("\n")
	f.write("SFILES = cmx-vec.S cmx-crt0.S\\\n")

	i = 0
	f.write("\t")
	for nm in lst:
		f.write("%s" % nm)
		i = i + 1
		if (i == 5):
			i = 0
			f.write("\\\n\t")
		else:
			f.write(" ")

	f.write("\n")
	f.write("\n")
	f.write("include ../../../mk/lib.mk\n")

	f.close()

if __name__ == "__main__":
    main()

