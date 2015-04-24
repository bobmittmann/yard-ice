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
#    File: tftp_reset.py
# Comment: YARD-ICE remote target reset utility
#  Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

import sys
import os
import getopt
import tftp

ver_major = 0
ver_minor = 1
def_host = "192.168.0.128"

# -------------------------------------------------------------------
# YARD-ICE (JTAG) scripts 
# -------------------------------------------------------------------

def show_usage():
	print("Usage: {0} [OPTION] Command...".format(progname))
	print("Send commands to YARD-ICE using the TFTP protocol")
	print("")
	print("  -h, --host     remote host addr."\
		" Default to: '{0}'".format(def_host))
	print("      --help     display this help and exit")
	print("  -V, --version  output version information and exit")
	print("")

def show_version():
	print("{0} - version {1:d}.{2:d}".format(progname, ver_major, ver_minor))
	print("")
	print("Writen by Bob Mittmann - bobmittmann@gmail.com")
	print("(C) Cpyright 2015 - Bob Mittmann")
	print("")

def error(msg):
#	print("", file=sys.stderr)
#	print("{0}: error: {1}".format(progname, msg), file=sys.stderr)
#	print("", file=sys.stderr)
#	sys.exit(2)
	print("")
	print("{0}: error: {1}".format(progname, msg))
	print("")
	sys.exit(2)

def main():
	global progname 

	progname = sys.argv[0]

	host = def_host

	try:
		opts, args = getopt.getopt(sys.argv[1:], "?vh:", \
			["help", "version", "host=" ])
	except err:
		error(str(err))

	for o, a in opts:
		if o in ("-?", "--help"):
			show_usage()
			sys.exit()
		elif o in ("-v", "--version"):
			show_version()
			sys.exit()
		elif o in ("-h", "--host"):
			host = a
		else:
			assert False, "unhandled option"

	if len(args) == 0:
		error("missing argument")

	script = ''
	for cmd in args:
		script += cmd + '\n'

	tclient = tftp.TftpClient(host)

	try:
		tclient.put(script, 'script', tftp.TFTP_MODE_NETASCII)
	except Exception as err:
		error(err)

if __name__ == "__main__":
	main()

