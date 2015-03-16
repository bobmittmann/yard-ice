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
#    File: fftp.py
# Comment: JTAG tftp upload utility
#  Author: Robinson Mittmann <bobmittmann@gmail.com>
# 

import socket
import time
import sys
import logging
import os
import struct

TFTP_MIN_BLKSIZE = 8
TFTP_DEF_BLKSIZE = 512
TFTP_MAX_BLKSIZE = 65536
TFTP_TIMEOUT_RETRIES = 5
TFTP_DEF_PORT = 69

TFTP_MODE_NETASCII = 0
TFTP_MODE_OCTET = 1

# --- constants
PY3 = sys.version_info >= (3, 0)

# ----------------------------------------------------------------------
# TFTP Exceptions
# ----------------------------------------------------------------------

class TftpException(Exception):
	"""Parent class of all TFTP exceptions."""
	pass

# ----------------------------------------------------------------------
# TFTP Packet Classes
# ----------------------------------------------------------------------

class TftpPacket(object):
	"""Abstract parent class of TFTP packet classes."""
	def __init__(self, opc):
		self.opc = opc 
		self.buf = bytearray()

	def encode(self):
		raise NotImplementedError("Abstract method")

	def decode(self):
		raise NotImplementedError("Abstract method")


class TftpPacketRRQ(TftpPacket):
	def __init__(self):
		TftpPacket.__init__(self, 1)


class TftpPacketWRQ(TftpPacket):
	def __init__(self, fname=None, mode=None, opt={}):
		TftpPacket.__init__(self, 2)
		self.fname = fname.encode('ascii')
		self.mode = mode
		if (opt == None):
			self.opt = {}
		else:
			self.opt = opt
	
		self.mode_str = { 
			TFTP_MODE_NETASCII: b'netascii', 
			TFTP_MODE_OCTET: b'octet'
			}

	def encode(self):
		"""Encode the packet's buffer from the instance variables."""
		mode_str = self.mode_str[self.mode]
		fmt = "!H{0:d}sx{1:d}sx".format(len(self.fname), len(mode_str))

		# Add options.
		opt_lst = []
		for key in self.opt:
			# Populate the option name
			fmt += "{0:d}sx".format(len(key))
			opt_lst.append(key.encode('ascii'))
			# Populate the option value
			fmt += "{0:d}sx".format(len(str(self.opt[key])))
			opt_lst.append(str(self.opt[key]).encode('ascii'))

		self.buf = struct.pack(fmt, int(self.opc), self.fname, mode_str, *opt_lst) 
		return self.buf


class TftpPacketDATA(TftpPacket):
	def __init__(self, blkno=0, data=None):
		TftpPacket.__init__(self, 3)
		self.blkno = blkno
		self.data = data 

	def encode(self):
		"""Encode the DATA packet"""
		fmt = "!HH{0:d}s".format(len(self.data))
		self.buf = struct.pack(fmt, int(self.opc), int(self.blkno), self.data)
		return self.buf


class TftpPacketACK(TftpPacket):
	def __init__(self):
		TftpPacket.__init__(self, 4)
		self.blkno = 0

	def decode(self):
		"""Decode the payload of an ACK packet and returns 
		   the acknlowledged block number"""
		(self.blkno,) = struct.unpack("!H", self.buf[2:])
		return self


class TftpPacketOACK(TftpPacket):
	def __init__(self):
		TftpPacket.__init__(self, 6)
		self.blkno = 0

	def decode_options(self, buf):
		"""This method decodes the section of the buf that contains an
		unknown number of options. It returns a dictionary of option names and
		values."""
		fmt = "!"
		opt = {}

		if len(buf) == 0:
			return {}
		
		# Count the nulls in the buf. Each one terminates a string.
		length = 0
		for c in buf:
			if PY3:
				octet = c
			else:
				octet = ord(c)
			
			if octet == 0:
				if length > 0:
					fmt += "{0:d}sx".format(length)
					length = -1
				else:
					raise TftpException("Invalid options in buf")
			length += 1


		mystruct = struct.unpack(fmt, buf)

		for i in range(0, len(mystruct), 2):
			opt[mystruct[i]] = mystruct[i + 1]

		return opt

	def decode(self):
		self.options = self.decode_options(self.buf[2:])
		return self

class TftpPacketERR(TftpPacket):
	def __init__(self):
		TftpPacket.__init__(self, 5)
		self.err = 0
		self.msg = None

		self.errmsgs = {
			1: "File not found",
			2: "Access violation",
			3: "Disk full or allocation exceeded",
			4: "Illegal TFTP operation",
			5: "Unknown transfer ID",
			6: "File already exists",
			7: "No such user",
			8: "Failed to negotiate options"
			}

	def decode(self):
		buflen = len(self.buf)
		if buflen == 4:
			fmt = "!H"
			(self.err,) = struct.unpack(fmt, self.buf[2:])
			self.msg = self.errmsgs[self.err]
		else:
			fmt = "!H{0:d}sx".format(buflen - 5)
			(self.err, txt) = struct.unpack(fmt, self.buf[2:])
			self.msg = txt.decode('ascii')
		raise TftpException("TFTP Error: {:d} - '{:s}'".format(self.err, self.msg))



# ----------------------------------------------------------------------
# TFTP Packet Factory
# ----------------------------------------------------------------------

class TftpPacketFactory(object):
    """This class generates TftpPacket objects. It is responsible for parsing
    raw buffers off of the wire and returning objects representing them, via
    the parse() method."""
    def __init__(self):
        self.classes = {
            1: TftpPacketRRQ,
            2: TftpPacketWRQ,
            3: TftpPacketDATA,
            4: TftpPacketACK,
            5: TftpPacketERR,
            6: TftpPacketOACK
            }

    def parse(self, buf):
        """This method is used to parse an existing datagram into its
        corresponding TftpPacket object. The buffer is the raw bytes off of
        the network."""
        (opc,) = struct.unpack("!H", buf[:2])
        pkt = self.__create(opc)
        pkt.buf = buf
        return pkt.decode()

    def __create(self, opc):
        """This method returns the appropriate class object corresponding to
        the passed opcode."""
        pkt = self.classes[opc]()
        return pkt


class TftpClientSession(object):

	def __init__(self, host, port):
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.host = host
		inet_addr = socket.gethostbyname(host)
		self.server_addr = (inet_addr, port)
		self.conn_addr = None
		self.recv_addr = None
		self.connected = False
		self.factory = TftpPacketFactory()

	def timeout(self, tmo):
		self.sock.settimeout(tmo)

	def cycle(self, pkt):
		"""Send a packet and wait for a reply. 
		   Return a packet object """

		# Encode the packet object and return the raw packet data
		buf = pkt.encode()

		retry_count = 0
		while (1):
			try:
				if self.connected:
					addr = self.conn_addr
				else:
					addr = self.server_addr

				self.sock.sendto(buf, addr)
				while (1):
					(buf, raddr) = self.sock.recvfrom(TFTP_MAX_BLKSIZE)

					if (self.connected):
						# Remote IP address and port number must match
						if (raddr == self.conn_addr):
							break
					else:
						# The IP address must match
						if (raddr[0] == self.server_addr[0]):
							break
						
				break
			except socket.timeout:
				if (retry_count >= TFTP_TIMEOUT_RETRIES):
					retry_count += 1
				else:
					raise TftpException("hit max retries, giving up")

		# Record the last sent packet
		self.last_pkt = pkt
		self.recv_addr = raddr

		# Parse the buffer and return a packet object
		pkt = self.factory.parse(buf)

		# Decode the packet object
		pkt.decode()

		return pkt

	def connect(self):
		if (self.recv_addr):
			self.conn_addr = self.recv_addr
		else:
			self.conn_addr = self.server_addr
		self.connected = True

	def disconnect(self):
		self.conn_addr = None
		self.connected = False


class TftpClient(object):

	def __init__(self, host, port=69):

		self.host = host
		self.port = port 
		self.session = TftpClientSession(self.host, self.port)

	def put(self, data, filename, mode, timeout=2, options={}):

		self.session.timeout(timeout)

		if (mode == TFTP_MODE_NETASCII): 
			s = data.replace('\n', '\r\n')
			bin_data = s.encode('ascii')
		else:
			bin_data = data

		if 'blksize' in options:
			opt_blksize = options['blksize']
			if opt_blksize < TFTP_MIN_BLKSIZE or opt_blksize > TFTP_MAX_BLKSIZE:
				raise TftpException("Invalid blksize: {:d}".format(opt_blksize))
		else:
			opt_blksize = TFTP_DEF_BLKSIZE

		self.blksize = TFTP_DEF_BLKSIZE
		data_len = len(bin_data)
		data_rem = data_len
		data_pos = 0
		blkno = 0
		last_data = False

		# prepare a write request
		wrq = TftpPacketWRQ(filename, mode, options)

		# send and receive 
		pkt = self.session.cycle(wrq)

		if isinstance(pkt, TftpPacketOACK):
			self.blksize = opt_blksize
		else:
			# we should receive ACK
			if not isinstance(pkt, TftpPacketACK):
				raise TftpException("Invalid response.")		

		# the block number must match our initial block number
		if (pkt.blkno != blkno):
			raise TftpException("Invalid block number {0:d}.".format(pkt.blkno))

		# connect to the remote host
		self.session.connect()

		while (not last_data):

			#increment block counter
			blkno += 1

			if (data_rem < self.blksize):
				n = data_rem
				last_data = True
			else:
				n = self.blksize

			# get next chunk from data buffer 
			buf = bin_data[data_pos:data_pos + n]
			data_pos += n;
			data_rem -= n;
			
			# create a new DATA packet
			pktData = TftpPacketDATA(blkno, buf)

			pkt = self.session.cycle(pktData)

			if not isinstance(pkt, TftpPacketACK):
				raise TftpException("Invalid response.")

			# FIXME: retransmit lost packet
			if (pkt.blkno != blkno):
				raise TftpException("Invalid block number {0:d}.".format(pkt.blkno))

		# disconnect from the remote host
		self.session.disconnect()

	def end(self):
		pass


