#!/usr/bin/env python

#
# Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
# 
# This file is part of the YARD-ICE.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3.0 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You can receive a copy of the GNU Lesser General Public License from 
# http://www.gnu.org/

#
#    File: bin2hex.py
# Comment: binary to C hex array utility
#  Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

import binascii  
import sys
import os
import re
import gzip
import codecs

__version__ = '0.2'

# --- constants
PY3 = sys.version_info >= (3, 0)

def chunks(seq, size):
  d, m = divmod(len(seq), size)
  for i in range(d):
    yield '0x' + seq[i*size:(i+1)*size]
  if m:
    yield '0x' + seq[d*size:]

def hexdump(binary):
  hexstr = binascii.hexlify(binary)
  if PY3:
    hexstr = hexstr.decode('ascii')
  return ', '.join(chunks(hexstr.lower(), 2))

# generate a line of C hex encoded data */
def c_hexgen(data, size):
  pos = 0
  rem = len(data)
  while rem:
   n = min(rem, size)
   d = data[pos:pos+n]
   rem = rem - n
   pos = pos + n
   if rem:
     line = '\t' + hexdump(d) + ','
   else:
     line = '\t' + hexdump(d)
   yield line
  
def bin2hex(f, name, data, size):
  f.write('/*---------------------------------------------------------------------------\n')
  f.write('  This file was automatically generated by bin2hex.py\n')
  f.write('  ---------------------------------------------------------------------------*/\n')
  f.write('\n')
  f.write('#include <stdint.h>\n')
  f.write('\n\n')
  f.write('#define SIZEOF_' + name.upper() + ' {:d}'.format(size) + '\n')
  f.write('\n')
  f.write('const uint8_t ' + name + '[{:d}]'.format(size) + ' = {' + '\n')
  gen = c_hexgen(data, 12)
  for line in gen:
    fout.write(line + '\n')
  f.write('};\n')
  f.write('\n')
  f.write('const unsigned int sizeof_' + name + ' = sizeof(' + name + ');\n')
  f.write('\n')

def replacelst(s, lst, val):
  for c in lst:
      s = s.replace(c, val)
  return s

file_size_mark = 'bin2hex.py: file size'
file_content_mark = 'bin.hex.py: file content'

def c_hdr_open(path):
  try:
#    f = codecs.open(path, 'r+', encoding='utf8')
    f = open(path, 'r+')
    f.seek(0, os.SEEK_END)
  except:
    f = codecs.open(path, 'w+', encoding='utf8')

  size = f.tell()
  if (size == 0):
    # create a file if non existent or empty 
    sym = replacelst(os.path.basename(path), [' ', '-', '.'], '_') 
    hdef = '__' + sym.upper() + '__'
    f.write('/*---------------------------------------------------------------------------\n')
    f.write('  This file was automatically generated by bin2hex.py\n')
    f.write('  ---------------------------------------------------------------------------*/\n')
    f.write('\n')
    f.write('#ifndef ' + hdef + '\n')
    f.write('#define ' + hdef + '\n')
    f.write('\n')
    f.write('/* --- ' + file_size_mark + ' --- */\n')
    f.write('\n\n')
    f.write('/* --- ' + file_content_mark + ' --- */\n')
    f.write('\n\n')
    f.write('#endif /* ' + hdef + ' */ \n')
  f.seek(0, os.SEEK_SET)

  return f

def c_hdr_update(f, sym, size):
  cdef = '#define SIZEOF_' + sym.upper()
  decl = 'extern const uint8_t ' + sym 
  f.seek(0, os.SEEK_SET)
  htxt = f.read();
  f.seek(0, os.SEEK_SET)

  if re.search(cdef + '[ ]*', htxt):
    htxt = re.sub('(' + cdef + '.*)', cdef + ' {:d}'.format(size), htxt)
  else:
    mark = file_size_mark
    if re.search(mark, htxt):
      htxt = re.sub('(.*' + mark + '.*)', 
					'\\1' + '\n\n' + cdef + ' {:d}'.format(size), htxt)
    else:
      print >> sys.stderr, 'WARNING: file size mark not found!'

  if re.search(decl, htxt):
    htxt = re.sub('(' + decl + '.*)', decl + '[];', htxt)
  else:
    mark = file_content_mark
    if re.search(mark, htxt):
      htxt = re.sub('(.*' + mark + '.*)', 
					'\\1' + '\n\n' + decl + '[];', htxt)
    else:
      print >> sys.stderr, 'WARNING: file content mark not found!'
    	
  f.write(htxt)
  f.close()

if __name__ == '__main__':
  from optparse import OptionParser
  parser = OptionParser(usage='''
  %prog binfile
  %prog -n [name]''', version=__version__)
  parser.add_option("-o", "--output", dest="outfname", 
					help="write result to FILE", metavar="FILE")
  parser.add_option("-n", "--name", dest="name", 
					help="use variable NAME", metavar="NAME")
  parser.add_option("-z", "--gzip", dest="compress", default=False,
					action="store_true", help="compress before converting")
  parser.add_option("-H", "--header", dest="header", 
					help="write header file", metavar="HEADER")

  options, args = parser.parse_args()

  if not args:
    parser.print_help()
    sys.exit(-1)

  infile = args[0]

  if options.name:
    name = options.name
  else:
    name = replacelst(os.path.basename(infile), [' ', '-', '.'], '_') 

  if options.outfname:
#    fout = open(options.outfname, 'w+')
    fout = codecs.open(options.outfname, 'w+', encoding='utf8')
  else:
    fout = sys.stdout

  fin = open(infile, 'rb')
  data = fin.read();
  fin.close()

  if options.compress:
  	data = gzip.compress(data);

  size = len(data);
  bin2hex(fout, name, data, size)

  if options.header:
    fin = c_hdr_open(options.header)
    c_hdr_update(fin, name, size)

