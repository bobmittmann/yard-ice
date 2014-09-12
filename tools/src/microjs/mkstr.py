#!/usr/bin/env python
# -*- coding: latin-1 -*-

import sys
import re 

__version__ = '0.1'

# --- constants
PY3 = sys.version_info >= (3, 0)

def read_strings(str_file):
	with open(str_file, 'r') as f:
		lst = []
		for line in f:
			s = line.strip()
			s = re.sub(r'"(.*)"', '\\1', s)
			lst.append(s)
		return lst 

def mkstrings(str_lst, name):
# convert to a set to remove duplicates
	str_set = set(str_lst) 
# make sure the empty string is in the list
	str_set.add('')
# revert back to a list to be sorted
	str_lst = list(str_set)
	str_lst.sort()
	offs = []

# get the number of strings
	cnt = len(str_lst);
# get the position where the strings should start,
# skip the initial part of the struct 
# strings start just after the offset list (array of 16bits numbers)
	pos = 4 + cnt * 2;
# calculate the offsets
	for w in str_lst:
		offs.append(pos)
		pos = pos + len(w) + 1

	print('#ifndef __' + name.upper() + '_STR_H__')
	print('#define __' + name.upper() + '_STR_H__')
	print('')
	print('#include <stdint.h>')
	print('')
	print('#ifdef __DEF_' + name.upper() + '_STRBUF__')
	print('')
	print('const struct {')
	print('\tuint16_t cnt;')
	print('\tuint16_t pos;')
	print('\tuint16_t offs[%s];' % cnt)
	print('\tchar buf[];')
	print('} ' + name + '_strbuf = {')
	print('\t.cnt = %d, /* number of strings */' % cnt)
	print('\t.pos = %d, /* initial offset */' % (4 + cnt * 2))
	print('\t.offs = { /* string offsets */')
	for o in offs:
		print('\t\t%d,' % o)
	print('\t},')
	print('\t.buf = {')
	for w in str_lst:
		print('\t\t"'+ w + '\\0"')
	print('\t}')
	print('};')
	print('')
	print('#endif')
	print('')
	i = 0
	for w in str_lst:
		if w != '': # skip empty string
			s = re.sub(r'[^a-zA-Z_0-9]', '_', w)
			print('#define NM_%-16s %d' % (s.upper(), i))
		i = i + 1
	print('')
	print('#define %s_STRINGS_MAX %d' % (name.upper(), i))
	print('')
	print('#endif /* __' + name.upper() + '_STR_H__ */')
	print('')

if __name__ == '__main__':
	from optparse import OptionParser
	parser = OptionParser(usage='''
	%prog input 
	%prog -n [name]''', version=__version__)
	parser.add_option("-o", "--output", dest="outfile", 
					help="write result to FILE", metavar="FILE")
	parser.add_option("-n", "--name", dest="name", 
					help="use variable NAME", metavar="NAME")

	options, args = parser.parse_args()

	if not args:
		parser.print_help()
		sys.exit(-1)

	infile = args[0]

	if options.name:
		name = options.name
	else:
		name = 'strings'

	lst = read_strings(infile)

	mkstrings(lst, name)

