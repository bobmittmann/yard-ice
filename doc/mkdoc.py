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
import re

__version__ = '0.1'

# --- constants
PY3 = sys.version_info >= (3, 0)

class DocApiObject:
  def __init__(self):
    print('DocApiObject.__init__')
    self._name = ''
    self._brief = ''
    self._desc = ''
  
class DocApiParameter(DocApiObject):
  def __init__(self, func, name, typ):
    super(DocApiObject, self ).__init__()
    self._name = name
    self._type = typ
    self._func = func

class DocApiFunction(DocApiObject):
  def __init__(self, group, name):
    super(DocApiObject, self ).__init__()
    self._name = name
    self._type = 'int'
    self._group = group 
    self._param = {}
  def parameter_add(self, name, typ):
    p = DocApiParameter(self, name, typ)
    self._param[name] = p
    print('parameter: ' + name + ' type: ' + typ) 
    return p

class DocApiGroup(DocApiObject):
  def __init__(self, name):
    super(DocApiGroup, self ).__init__()
    self._name = name
    self._function = {}

  def function_add(self, name):
    f = DocApiFunction(self, name)
    self._function[name] = f
    print('function: ' + name)
    return f

def getline(text):
  # Return a list of the lines, breaking at line boundaries.
  for line in text.splitlines():
    yield line

class DocTree:
  def __init__(self):
    self._name = ''
    self._group = {}
    self._blk = []

  def readfile(self, f):
    # Read the contents of the file into memory.
    data = f.read()  
    # Join comma split lines
    self._data = re.sub(',[ \t]*\n[ \t]*', ', ', data)
    print(self._data)

  def next_line(self):
    return next(self._lgen)

  def doxygen_block(self, first_line):
    doxygen_end = re.compile('[ \t]+\*/')
    doxygen_line = re.compile('[ \t]+\*[ \t]+')
    lst = []
    lst.append(first_line)
    while True:
      try:
        line = self.next_line()
      except StopIteration as e:
        break

      if doxygen_end.match(line):
        break

      if doxygen_line.match(line):
        s = doxygen_line.sub('', line)
      else:
        s = line

      lst.append(s)

    return lst

  def tokenize(self):
    # Return a list of the lines, breaking at line boundaries.
    text = self._data;
    self._lgen = getline(text);
    doxygen_begin = re.compile('/\*\*[ \t]+')
    while True:
      try:
        line = self.next_line()
      except StopIteration as e:
        break

      if doxygen_begin.match(line):
        lst = doxygen_block(doxygen_begin.sub('', line))
        print('Doxygen: ')
        print(lst)

  def group_add(self, name):
    g = DocApiGroup(name)
    self._group[name] = g
    print('group: ' + name)
    return g

  def group_dump(self):
    for g in self._group:
      print('group: ' + g)

  def parse(self):
    # Return a list of the lines, breaking at line boundaries.
    text = self._data.splitlines()
    tag_re = re.compile('(\.[a-z]+) (.*)')
    tab_re = re.compile('\t(\.[a-z]+)')
    s = ''
    for line in text:
      if tag_re.match(line):
        try:
          o._desc = s
        except:
          print('----')
          
        n = tag_re.split(line)[1]
        v = tag_re.split(line)[2]
        if n == 'defgroup':
          g = self.group_add(v)
          o = g
        elif n == '.func':
          print('function: ' + v)
          f = g.function_add(v)
          o = f
        elif n == '@param':
          l = v.rsplit(' ', 1)
          p = f.parameter_add(l[1], l[0])
          o = p
        elif n == '@return':
          print('return')
        else:
          print('"' + n + '"')
      elif tab_re.match(line):
        try:
          o._brief = line.replace('\t', '')
        except:
          print('----')

      else:
        s = s + line

def mkheader(f):
  s = '<?xml version="1.0" encoding="UTF-8"?>' + \
  '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">' + \
  '<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">' + \
  '<head>\n' + \
  '<style>\n' + \
  '* {\n' + \
  '\tborder: 0;\n' + \
  '\tmargin: 0;\n' + \
  '\tpadding:1;\n' + \
  '}\n' + \
  'body {\n' + \
  '\tbackground: #f8f8f8;\n' + \
  '\tcolor: #555;\n' + \
  '\tfont: 0.9em Arial,Helvetica, "bitstream vera sans", sans-serif;\n' + \
  '\tmargin: 10px 10px 25px 10px;\n' + \
  '}\n' + \
  'a {\n' + \
  '\tcolor: #779;\n' + \
  '\ttext-decoration:none;\n' + \
  '}\n' + \
  'a:hover {\n' + \
  '\tcolor: #335;\n' + \
  '\ttext-decoration: none;\n' + \
  '}\n' + \
  'p {\n' + \
  '\tcolor: #111;\n' + \
  '\ttext-align: justify;\n' + \
  '\tmargin: 5px 0px 5px 0px;\n' + \
  '}\n' + \
  'i {\n' + \
  '\tcolor:#272;\n' + \
  '}\n' + \
  'code {\n' + \
  '\tcolor: #262;\n' + \
  '\tfont: 1.0em;\n' + \
  '\tcolor:#222;\n' + \
  '\tfont-family: monospace, courier;\n' + \
  '\tfont-weight: bold;\n' + \
  '}\n' + \
  'h1 {\n' + \
  '\tcolor: #248;\n' + \
  '\tfont: 0.9em;\n' + \
  '\ttext-align: left;\n' + \
  '\tmargin: 10px 0px 10px 0px;\n' + \
  '}\n' + \
  'h2 {\n' + \
  'color: #468;\n' + \
  '\tfont: 0.9em;\n' + \
  '\ttext-align: left;\n' + \
  '\tmargin: 0.6em 0 0.2em 0;\n' + \
  '}\n' + \
  'h3 {\n' + \
  '\tfont: 0.9em;\n' + \
  '\ttext-align: left;\n' + \
  '\tmargin: 1.6em 0 .2em 0;\n' + \
  '}\n' + \
  'h4 {\n' + \
  '\tfont: 0.8em;\n' + \
  '\ttext-align: left;\n' + \
  '\tmargin: .6em 0 .1em 0;\n' + \
  '}\n' + \
  '.typ {\n' + \
  '\tfont-style: bold;\n' + \
  '\tcolor:#224;\n' + \
  '}\n' + \
  '.fun {\n' + \
  '\tfont-weight: bold;\n' + \
  '\tcolor:#112;\n' + \
  '}\n' + \
  '.var {\n' + \
  '\tfont-style: italic;\n' + \
  '\tcolor:#272;\n' + \
  '}\n' + \
  '.desc {\n' + \
  '\tmargin-left: 2em;\n' + \
  '}\n' + \
  '.code {\n' + \
  '\tcolor:#227;\n' + \
  '\tfont-family: monospace, courier;\n' + \
  '/*\tfont-weight: bold; */\n' + \
  '\tmargin: 0.4em 0 1em 0;\n' + \
  '}\n' + \
  'hr {\n' + \
  '\tbackground-color:#114;\n' + \
  '\tcolor:#112;\n' + \
  '\twidth: 100%;\n' + \
  '\theight: 1px;\n' + \
  '\tmargin: 10px 0px 5px 0px;\n' + \
  '}\n' + \
  'table {\n' + \
  '\tborder-collapse: collapse;\n' + \
  '}\n' + \
  '</style>\n' + \
  '<link rel="shortcut icon" href="img/thinkos16x16.png"/>\n' + \
  '<meta name="copyright" content="Copyright (c) Bob Mittmann 2014-2015"/>\n' + \
  '<title>ThinkOS</title>\n' + \
  '</head>\n'
  f.write(s.replace('\n', '\r\n'))

def mkfooter(f):
  s = '<h3>Author</h3>\n' + \
  '<p>Bob Mittmann&lt;bobmittmann@gmail.com&gt;</p>\n' + \
  '<hr>\n' + \
  '<a href="https://github.com/bobmittmann/yard-ice">\n' + \
  '<img src="img/thinkos57x24.png"></a> - Cortex-M Operating System -\n' + \
  '<i><a href="https://github.com/bobmittmann/yard-ice">YARD-ICE</a></i>\n' + \
  '<br>&copy; Copyright 2013-2015, Bob Mittmann<br>\n' + \
  '</body>\n' + \
  '</html>\n'
  f.write(s.replace('\n', '\r\n'))

def mkapifunction(f, fun):
  s = '\n<h3>{:s}</h3>\n<p>{:s}</p>\n'.format(fun._name, fun._desc)
  s += '<h4>Synopsys</h4>\n<div class="code">\n'\
  '#include &#60;thinkos.h&#62;<br /><br />\n'
  s += '<span class="typ">{:s}</span>\n'.format(fun._type)
  s += '<span class="fun">{:s}('.format(fun._name)

def mkapigroup(f, grp):
  s = '\n<div class="group">\n<hr />\n' \
	  '<h2>{:s}</h2>\n<p>{:s}</p>\n\n'.format(grp._name, grp._desc)
  f.write(s.replace('\n', '\r\n'))

  for func in grp._function.values():
      mkapifunction(f, func)

#							   int (* <span class="var">task_start</span>)(void *), \n'
#'<span class="typ">void *</span> <span class="var">task_arg</span>, \n'
#'<span class="typ">void *</span> <span class="var">stack_ptr</span>,\n'
#'<span class="typ">unsigned int</span> <span class="var">opt</span>);\n'
  s += '</div>\n\n'
  f.write(s.replace('\n', '\r\n'))

def mkapidoc(f, tree):
  s = '<body>\n' + \
  '<h1>ThinkOS</h1>\n' + \
  '<h1>Index</h1>\n' + \
  '<h1>Brief</h1>\n' + \
  '<p>ThinkOS is a low latency configurable real-time operating system ' + \
  'specifically designed for ARM Cortex-M processors.</p>\n' + \
  '<h1>API</h1>\n' + \
  '<p>ThinkOS supports several synchronisation and IPC mechanisms</p>\n'
  f.write(s.replace('\n', '\r\n'))

  for grp in tree._group.values():
    mkapigroup(f, grp)

def parseinput(f):
  tree = DocTree()
  tree.readfile(f)
  tree.tokenize()
#  tree.parse()
  return tree

if __name__ == '__main__':
  from optparse import OptionParser
  parser = OptionParser(usage='''
  %prog binfile
  %prog -n [name]''', version=__version__)
  parser.add_option("-o", "--output", dest="outfname", 
					help="write result to FILE", metavar="FILE")

  options, args = parser.parse_args()

  if options.outfname:
    fout = open(options.outfname, 'w+')
  else:
    fout = open("thinkos_api.html", 'w+')
#    fout = sys.stdout

#  if options.infname:
#    fin = open(options.infname, 'r')
#  else:
  fin = open("thinkos_api.txt", 'r')

  parseinput(fin)
#  tree.group_dump()

#  mkheader(fout)

#  mkapidoc(fout, tree)

#  mkfooter(fout)

