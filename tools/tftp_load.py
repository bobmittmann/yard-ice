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
#    File: fftp_load.py
# Comment: YARD-ICE JTAG TFTP upload utility
#  Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

import sys
import os
import getopt
import tftp
import time

ver_major = 0
ver_minor = 1
def_addr = 0x08000000
def_host = "192.168.0.128"
def_target = "stm32f"

# -------------------------------------------------------------------
# YARD-ICE (JTAG) scripts 
# -------------------------------------------------------------------

power_on_noisy = 'nrst set \n'\
	'power on\n'\
	'target null\n'\
	'beep 4 50\n'\
	'beep 6 50\n'\
	'beep 4 50\n'\
	'beep 6 50\n'\
	'nrst clr\n'\
	'rst\n'

power_on_quiet = 'nrst set \n'\
	'power on\n'\
	'target null\n'\
	'nrst clr\n'\
	'rst\n'

tgt_reset_noisy = 'rst\n'\
	'run\n'\
	'beep 7 125\n'\
	'beep 6 125\n'\
	'beep 5 125\n'\
	'connect\n'

tgt_reset_quiet = 'rst\n'\
	'run\n'\
	'connect\n'

tgt_init = 'target %s\n'\
	'connect\n'\
	'halt\n'\
	'init\n'

power_off = 'release\n'\
	'target null\n'\
	'trst clr\n'\
	'idle 1\n'\
	'power off\n'\
	'beep 1\n'\
	'sleep 100\n'\
	'beep 1\n'

error_power_off = 'release\n'\
	'target null\n'\
	'trst clr\n'\
	'idle 1\n'\
	'power off\n'\
	'beep 7 125\n'\
	'beep 6 125\n'\
	'beep 5 125\n'\
	'beep 4 125\n'\
	'beep 3 125\n'\
	'beep 2 125\n'


def show_usage():
	print >> sys.stderr, "Usage: %s [OPTION] FILE" % progname
	print >> sys.stderr, "Transfer FILE using the TFTP protocol"
	print >> sys.stderr, ""
	print >> sys.stderr, "  -a, --addr     upload address."\
		" Default to: 0x%08x" % def_addr
	print >> sys.stderr, "  -h, --host     remote host addr."\
		" Default to: '%s'" % def_host
	print >> sys.stderr, "  -t, --target   target platform."\
		" Default to: '%s'" % def_target
	print >> sys.stderr, "  -r, --reset    exec the reset script"
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
	addr = def_addr
	reset = False
	quiet = False
	target = def_target

	try:
		opts, args = getopt.getopt(sys.argv[1:], "?rqva:h:t:", \
			["help", "reset", "quiet", "version", "addr=", "host=", "target="])
	except GetoptError, err:
		error(str(err))

	for o, a in opts:
		if o in ("-?", "--help"):
			show_usage()
			sys.exit()
		elif o in ("-v", "--version"):
			show_version()
			sys.exit()
		elif o in ("-r", "--reset"):
			reset = True
		elif o in ("-q", "--quiet"):
			quiet = True
		elif o in ("-a", "--addr"):
			addr = int(a, 0)
		elif o in ("-h", "--host"):
			host = a
		elif o in ("-t", "--target"):
			target = a
		else:
			assert False, "unhandled option"

	if len(args) == 0:
		error("you must specify a file to transfer")

	if len(args) > 1:
		error("too many arguments")

	in_fname = args[0]
	dirname, fname = os.path.split(in_fname)
	basename, extension = os.path.splitext(fname)
	fsize = 0

	try:
		f = open(in_fname, mode='rb')
		bin_data = f.read()
		f.close()
	except Exception as err:
		error("can't open file: '%s'" % in_fname)

	fsize = len(bin_data)

	print " - File: '%s'" % fname
	print " - Size: %d bytes" % fsize
	print " - Remote host: %s" % host
	print " - Target: '%s'" % target
	print " - Upload address: 0x%08x" % addr
	sys.stdout.flush()

	options = {}
	options['blksize'] = 512
	timeout = 2
	port = 69
	tclient = tftp.TftpClient(host, port, options)

	if quiet:
		power_on = power_on_quiet 
		tgt_reset = tgt_reset_quiet 
	else:
		power_on = power_on_noisy
		tgt_reset = tgt_reset_noisy
#
#	try:
#		print " - Power on"
#		tclient.put(power_on, 'script', tftp.TFTP_MODE_NETASCII)
#	except Exception as err:
#		error(err)

	try:
		print " - Initializing remote target"
		sys.stdout.flush()
		script = tgt_init % target
		tclient.put(script, 'script', tftp.TFTP_MODE_NETASCII)
	except Exception as err:
		error(err)

	try:
		print " - Erasing 0x%08x (%d bytes)" % (addr, fsize)
		sys.stdout.flush()
		script = "erase 0x%08x %d\n" % (addr, fsize)
		tclient.put(script, 'script', tftp.TFTP_MODE_NETASCII)
	except Exception as err:
		error(err)

	try:
		print " - Loading binary file..."
		sys.stdout.flush()
		t0 = time.time()
		tclient.put(bin_data, '0x%08x' % addr, tftp.TFTP_MODE_OCTET, timeout)
		dt = time.time() - t0
		print " - %d bytes transferred in %.2f seconds (%.0f bytes/sec)" % \
			(fsize, dt, fsize/dt)
		sys.stdout.flush()
	except Exception as err:
		error(err)

	if reset:
		try:
			print " - Reseting remote target"
			tclient.put(tgt_reset, 'script', tftp.TFTP_MODE_NETASCII)
			sys.stdout.flush()
		except Exception as err:
			error(err)

if __name__ == "__main__":
	main()

