#!/usr/bin/env python

import binascii  
import sys
import os
import re

__version__ = '0.1'

# --- constants
PY3 = sys.version_info >= (3, 0)

# --- - chunking helpers
def chunks(seq, size):
  d, m = divmod(len(seq), size)
  for i in range(d):
    yield '0x' + seq[i*size:(i+1)*size]
  if m:
    yield '0x' + seq[d*size:]

def chunkread(f, size):
  c = f.read(size)
  while len(c):
    yield c
    c = f.read(size)

# --- hex stuff
def dump(binary):
  hexstr = binascii.hexlify(binary)
  if PY3:
    hexstr = hexstr.decode('ascii')
  return ', '.join(chunks(hexstr.lower(), 2))

def dumpgen(f):
  generator = chunkread(f, 12)
  for d in generator:
    line = '\t' + dump(d) + ','
    yield line
  
def bin2hex(f, name, size):
  print('#include <stdint.h>')
  print('')
  print('#define SIZEOF_' + name.upper() + ' {:d}'.format(size) )
  print('')
  print('const uint8_t ' + name + '[{:d}]'.format(size) + ' = {' )
  gen = dumpgen(f)
  for line in gen:
    print(line)
  print('};')
  print('')
  print('const unsigned int sizeof_' + name + ' = sizeof(' + name + ');')

def replacelst(s, lst, val):
  for c in lst:
      s = s.replace(c, val)
  return s

file_size_mark = '/* File size */\n\n'
file_content_mark = '/* File content */\n\n'

def c_hdr_open(path):
  try:
    f = open(path, 'r+')
    f.seek(0, os.SEEK_END)
  except:
    f = open(path, 'w+')

  size = f.tell()
  if (size == 0):
    # create a file if non existent or empty 
    sym = replacelst(os.path.basename(path), [' ', '-', '.'], '_') 
    hdef = '__' + sym.upper() + '__'
    f.write('/* This file was automatically generated */\n')
    f.write('\n')
    f.write('#ifndef ' + hdef + '\n')
    f.write('#define ' + hdef + '\n')
    f.write('\n')
    f.write(file_size_mark)
    f.write('\n')
    f.write(file_content_mark)
    f.write('\n')
    f.write('#endif' '\n')
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
    mark = file_size_mark.replace('*', '\*').replace('\n', '\\n')
    if re.search(mark, htxt):
      htxt = re.sub('(' + mark + ')', 
					'\\1' + cdef + ' {:d}\\n'.format(size), htxt)
    else:
      print >> sys.stderr, 'WARNING: file size mark not found!'

  if re.search(decl, htxt):
    htxt = re.sub('(' + decl + '.*)', decl + '[];\\n', htxt)
  else:
    mark = file_content_mark.replace('*', '\*').replace('\n', '\\n')
    if re.search('(' + mark + ')', htxt):
      htxt = re.sub('(' + mark + ')', '\\1' + decl + '[];\\n', htxt)
    else:
      print >> sys.stderr, 'WARNING: file content mark not found!'
    	
#  print >> sys.stderr, '"' + htxt + '"'

  f.write(htxt)
  f.close()

if __name__ == '__main__':
  from optparse import OptionParser
  parser = OptionParser(usage='''
  %prog binfile
  %prog -n [name]''', version=__version__)
  parser.add_option("-o", "--output", dest="outfile", 
					help="write result to FILE", metavar="FILE")
  parser.add_option("-n", "--name", dest="name", 
					help="use variable NAME", metavar="NAME")
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

  f = open(infile, 'rb')
  f.seek(0, os.SEEK_END)
  size = f.tell()
  f.seek(0, os.SEEK_SET)

  bin2hex(f, name, size)
  f.close()

  if options.header:
    f = c_hdr_open(options.header)
    c_hdr_update(f, name, size)

