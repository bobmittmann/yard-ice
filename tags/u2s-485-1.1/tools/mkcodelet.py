#!/usr/bin/python
from struct import *
from getopt import *
import sys
import os
import re

def usage():
	global progname
	print >> sys.stderr, ""
	print >> sys.stderr, " Usage:", progname, "[options] fname"
	print >> sys.stderr, ""
	print >> sys.stderr, "Options"
	print >> sys.stderr, "  -h, --help   show this help message and exit"
	print >> sys.stderr, "  -o FILENAME, --addr=FILENAME"
	print >> sys.stderr, ""

def error(msg):
	print >> sys.stderr, ""
	print >> sys.stderr, "#error:", msg
	usage()
	sys.exit(2)

def mk_codelet(in_fname, out_fname, hdr_fname):

	try:
		in_file = open(in_fname, mode='r')
	except:
		print >> sys.stderr, "#error: can't open file: '%s'" % in_fname
		sys.exit(1)

	try:
		c_file = open(out_fname, mode='w')
	except:
		print >> sys.stderr, "#error: can't create file: %s" % out_fname
		sys.exit(1)

	try:
		h_file = open(hdr_fname, mode='w')
	except:
		print >> sys.stderr, "#error: can't create file: %s" % hdr_fname
		sys.exit(1)

	i = 0
	for line in in_file:
		if re.match("SYMBOL TABLE:", line):
			break

	s_pat = re.compile("([0-9a-f]{8}) ..*[0-9a-f]{8} ([.A-Za-z_][A-Za-z_0-9]*)")

	sym = {}

	for line in in_file:
		m = s_pat.findall(line)
		if m:
			addr = int(m[0][0], 16)
			name = m[0][1]
			sym[addr] = name 
		else:
			break

	for line in in_file:
		if re.match("Contents of section .text:", line):
			break

	token_pat = re.compile("([0-9a-f]{2})([0-9a-f]{2})([0-9a-f]{2})([0-9a-f]{2})")

	c_file.write("#include <stdint.h>\n\n")
	h_file.write("#include <stdint.h>\n\n")

	addr = 0
	i = 0
	for line in in_file:
		for a, b, c, d in token_pat.findall(line):
			try:
				sym[addr]
				if (i > 0):
					c_file.write("\n};\n\n")
				c_file.write("const uint32_t %s[] = {" % sym[addr])
				h_file.write("extern const uint32_t %s[];\n\n" % sym[addr])
				i = 0
			except KeyError:
				pass
			if ((i % 4) == 0):
				if (i > 0):
					c_file.write(",")
				c_file.write("\n\t0x" + d + c + b + a)
			else:
				c_file.write(", 0x" + d + c + b + a )
			i = i + 1;
			addr = addr + 4
	c_file.write("\n};\n")

	in_file.close()
	c_file.close()
	h_file.close()

	return

def main():
	global progname 

	progname = sys.argv[0]

	try:
		opts, args = getopt(sys.argv[1:], "ho:", \
							["help", "output="])
	except GetoptError, err:
		error(str(err))

	for o, a in opts:
		if o in ("-h", "--help"):
			usage()
			sys.exit()
		elif o in ("-o", "--output"):
			out_fname = a
		else:
			assert False, "unhandled option"

	if len(args) == 0:
		error("missing fname")

	if len(args) > 1:
		error("too many arguments")

	in_fname = args[0]

	try:
		out_fname	
	except NameError:
		dirname, fname = os.path.split(in_fname)
		basename, extension = os.path.splitext(fname)
		out_fname = basename + '.' + 'c'

	dirname, fname = os.path.split(out_fname)
	basename, extension = os.path.splitext(fname)
	hdr_fname = basename + '.' + 'h'

	mk_codelet(in_fname, out_fname, hdr_fname)

if __name__ == "__main__":
	main()
