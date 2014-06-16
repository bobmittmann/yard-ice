#!/usr/bin/env python
# -*- coding: latin-1 -*-

import binascii  
import sys

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
  
def bin2hex(f, name):
  print 'const uint8_t ' + name + '[] = {'
  gen = dumpgen(f)
  for line in gen:
    print(line)
  print '};'
  print ''
  print 'sizeof_' + name + ' = sizeof(' + name + ');' 

if __name__ == '__main__':
  from optparse import OptionParser
  parser = OptionParser(usage='''
  %prog binfile
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
    name = 'bin'

  bin2hex(open(infile, 'rb'), name)

