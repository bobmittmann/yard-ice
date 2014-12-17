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
#    File: tftp_put.py
# Comment: TFTP upload utility
#  Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

#!/usr/bin/env python

import socket
import time
import sys
import os
import struct
from optparse import OptionParser
import tftp

def main():

	usage=""
	parser = OptionParser(usage=usage)
	parser.add_option('-p',
					  '--port',
					  default=69,
					  help='remote port to use (default: 69)')
	parser.add_option('-f',
					  '--filename',
					  help='remote filename')
	parser.add_option('-a',
					  '--ascii',
					  action='store_true',
					  default=False,
					  help='ASCII mode')
	parser.add_option('-b',
					  '--blksize',
					  help='udp packet size to use (default: 512)')
	parser.add_option('-s',
					  '--tsize',
					  action='store_true',
					  default=False,
					  help="ask client to send tsize option in download")
	parser.add_option('-t',
					  '--timeout',
					  default=1,
					  help="option to set the server's timeout interval")
	(options, args) = parser.parse_args()

	if len(args) != 2:
		parser.error("wrong number of arguments")

	host = args[0]
	path = args[1]

	if options.filename:
		filename = options.filename
	else:
		filename = os.path.basename(path)

	if options.ascii:
		mode = tftp.TFTP_MODE_NETASCII
	else:
		mode = tftp.TFTP_MODE_OCTET 

	port = int(options.port)
	timeout = int(options.timeout)

	tftp_options = {}
	if options.blksize:
		tftp_options['blksize'] = int(options.blksize)
	if options.tsize:
		tftp_options['tsize'] = 0

	if not os.path.exists(path):
		print >> sys.stderr, "File not found: %s" % path
		return 1

	try:
		f = open(path, "rb")
		data = f.read()
		f.close()
	except Exception as err:
		sys.stderr.write("%s\n" % str(err))
		sys.exit(1)

	tclient = tftp.TftpClient(host, port)

	try:
		tclient.put(data, filename, mode, timeout, tftp_options)
	except Exception as err:
		sys.stderr.write("%s\n" % str(err))
		sys.exit(1)
	except KeyboardInterrupt:
		pass

if __name__ == '__main__':
	main()

