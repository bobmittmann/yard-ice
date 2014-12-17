#!/usr/bin/env python
# -*- coding: latin-1 -*-

import sys
import re 

__version__ = '0.1'

# --- constants
PY3 = sys.version_info >= (3, 0)

def read_words(words_file):
	with open(words_file, 'r') as f:
		ret = []
		for line in f:
			ret += line.split()
		return ret

def mkstrings(words, name):
	words = list(set(words))
	words.sort()
	offs = []

	print('#ifndef __' + name.upper() + '_H__')
	print('#define __' + name.upper() + '_H__')
	print('')
	print('#include <stdint.h>')
	print('')
	print('#ifdef __DEF_' + name.upper() + '__')
	print('')
	print('const char ' + name + '_buf[] = {')
	print('\t"\\0"')
	offs.append(0)
	pos = 1
	for w in words:
		print('\t"'+ w + '\\0"')
		offs.append(pos)
		pos = pos + len(w) + 1
	print('};')
	print('')
	print('const struct str_pool ' + name + '_pool = {')
	print('\t.buf = (char *)' + name + '_buf,')
	print('\t.cnt = %d,' % (len(words) + 1))
	print('\t.offs = {')
	for o in offs:
		print('\t%d,' % o)
	print('\t}')
	print('};')
	print('')
	print('#else ')
	print('')
	print('extern const char ' + name + '_buf[];')
	print('')
	print('extern const struct str_pool ' + name + '_pool;')
	print('')
	print('#endif')
	print('')

	i = 1
	for w in words:
		s = re.sub(r'[^a-zA-Z_0-9]', '_', w)
		print('#define NM_%-12s %d' % (s.upper(), i))
		i = i + 1
	print('')
	print('#define CONST_STRINGS_MAX %d' % (i))
	print('')
	print('#endif /* __' + name.upper() + '_H__ */')
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

	lst = read_words(infile)

	mkstrings(lst, name)

