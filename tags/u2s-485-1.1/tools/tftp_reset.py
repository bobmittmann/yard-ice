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

tgt_reset_noisy = 'rst\n'\
	'beep 7 125\n'\
	'beep 6 125\n'\
	'beep 5 125\n'\
	'connect\n'

tgt_reset_quiet = 'rst\n'\
	'connect\n'


def show_usage():
	print >> sys.stderr, "Usage: %s [OPTION]" % progname
	print >> sys.stderr, "Reset remote target using the TFTP protocol"
	print >> sys.stderr, ""
	print >> sys.stderr, "  -h, --host     remote host addr."\
		" Default to: '%s'" % def_host
	print >> sys.stderr, "  -q, --quiet    silent mode (no beeps)"
	print >> sys.stderr, "      --help     display this help and exit"
	print >> sys.stderr, "  -V, --version  output version information and exit"
	print >> sys.stderr, ""

def show_version():
	print "%s - version %d.%d" % (progname, ver_major, ver_minor)
	print ""
	print "Writen by Bob Mittmann - bobmittmann@gmail.com"
	print "(C) Cpyright 2014 - Bob Mittmann"
	print ""

def error(msg):
	print >> sys.stderr, ""
	print >> sys.stderr, "%s: error: %s" % (progname, msg)
	print >> sys.stderr, ""
	sys.exit(2)

def main():
	global progname 

	progname = sys.argv[0]

	host = def_host
	quiet = False

	try:
		opts, args = getopt.getopt(sys.argv[1:], "?qvh:", \
			["help", "quiet", "version", "host=" ])
	except GetoptError, err:
		error(str(err))

	for o, a in opts:
		if o in ("-?", "--help"):
			show_usage()
			sys.exit()
		elif o in ("-v", "--version"):
			show_version()
			sys.exit()
		elif o in ("-q", "--quiet"):
			quiet = True
		elif o in ("-h", "--host"):
			host = a
		else:
			assert False, "unhandled option"

	if len(args) > 0:
		error("too many arguments")

	tclient = tftp.TftpClient(host)

	if quiet:
		tgt_reset = tgt_reset_quiet 
	else:
		tgt_reset = tgt_reset_noisy

	try:
		tclient.put(tgt_reset, 'script', tftp.TFTP_MODE_NETASCII)
	except Exception as err:
		error(err)

if __name__ == "__main__":
	main()

