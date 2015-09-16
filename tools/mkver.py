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
#    File: mkver.py
# Comment: 
#  Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

import sys
import os
import codecs
import datetime

__version__ = '0.1'

# --- constants
PY3 = sys.version_info >= (3, 0)

def mkhead(f, sym):
  hdef = '__' + sym.upper() + '__'
  f.write('/*---------------------------------------------------------------------------\n')
  f.write('  This file was automatically generated by mkver.py. DO NOT edit!\n')
  f.write('  ---------------------------------------------------------------------------*/\n')
  f.write('\n')
  f.write('#ifndef ' + hdef + '\n')
  f.write('#define ' + hdef + '\n')
  f.write('\n')

def mktail(f, sym):
  hdef = '__' + sym.upper() + '__'
  f.write('\n\n')
  f.write('#endif /* ' + hdef + ' */ \n')
  
def mkver(f, name, major, minor, date):
  x = date.split('-')
  year = int(x[0])
  month = int(x[1])
  day = int(x[2])
  ref = datetime.datetime(year, month, day)
  now = datetime.datetime.now()
  diff = now - ref
  build = (diff.days * 24) + (diff.seconds / (60 * 60))
  f.write('\n')
  f.write('#define VERSION_NAME \"{:s}\"\n'.format(name))
  f.write('#define VERSION_MAJOR {:d}\n'.format(major))
  f.write('#define VERSION_MINOR {:d}\n'.format(minor))
  f.write('#define VERSION_BUILD {:d}\n'.format(build))
  f.write('#define VERSION_NUM \"{:d}.{:d}.{:d}\"\n'.\
		  format(major, minor, build))
  f.write('#define VERSION_STR \"{:s}-{:d}.{:d}.{:d}\"\n'.\
		  format(name, major, minor, build))
  f.write('#define VERSION_YEAR \"{:04d}\"\n'.format(year))
  f.write('#define VERSION_DATE \"{:04d}-{:02d}-{:02d}\"\n'.\
		  format(year, month, day))

#  f.write('#define VERSION_MACH \"$(MACH)\"\n')
#  f.write('#define VERSION_ARCH \"$(ARCH)\"\n')
#  f.write('#define VERSION_CPU \"$(CPU)\"\n')
# if [ "$(PRODUCT)" != "" ]; then \
#	f.write('#define PRODUCT_ID $(PRODUCT)')
#fi; \
#if [ "$(REVISION)" != "" ]; then \
#	f.write('#define PRODUCT_REV $(REVISION)')
#fi; \
#if [ "$(ASSEMBLY)" != "" ]; then \
#	f.write('#define ASSEMBLY '$(ASSEMBLY)'')
#fi; \

def replacelst(s, lst, val):
  for c in lst:
      s = s.replace(c, val)
  return s

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

  major = int(args[0])
  minor = int(args[1])
  date = args[2]

  if options.outfname:
    fout = open(options.outfname, 'w+')
    hdr = replacelst(os.path.basename(options.outfname), [' ', '-', '.'], '_') 
  else:
    fout = sys.stdout
    hdr = "version.h"

  if options.name:
    name = options.name

  mkhead(fout, hdr)
  mkver(fout, name, major, minor, date)
  mktail(fout, hdr)
