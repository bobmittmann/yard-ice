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
		self.buf = None

	def encode(self):
		raise NotImplementedError, "Abstract method"

	def decode(self):
		raise NotImplementedError, "Abstract method"


class TftpPacketRRQ(TftpPacket):
	def __init__(self):
		TftpPacket.__init__(self, 1)


class TftpPacketWRQ(TftpPacket):
	def __init__(self, fname=None, mode=None, opt={}):
		TftpPacket.__init__(self, 2)
		self.fname = fname
		self.mode = mode
		self.opt = opt
	
		self.mode_str = { 
			TFTP_MODE_NETASCII: "netascii", 
			TFTP_MODE_OCTET: "octet" 
			}

	def encode(self):
		"""Encode the packet's buffer from the instance variables."""
		mode_str = self.mode_str[self.mode]
		fmt = "!H%dsx%dsx" % (len(self.fname), len(mode_str))
		self.buf = struct.pack(fmt, self.opc, self.fname, mode_str) 
		return self.buf


class TftpPacketDATA(TftpPacket):
	def __init__(self, blkno=0, data=None):
		TftpPacket.__init__(self, 3)
		self.blkno = blkno
		self.data = data 

	def encode(self):
		"""Encode the DATA packet"""
		fmt = "!HH%ds" % len(self.data)
		self.buf = struct.pack(fmt, self.opc, self.blkno, self.data)
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
			fmt = "!H%dsx" % (buflen - 5)
			(self.err, self.msg) = struct.unpack(fmt, self.buf[2:])

		raise TftpException, "TFTP Error: %d - '%s'" % (self.err, self.msg)


class TftpPacketOACK(TftpPacket):
	def __init__(self):
		TftpPacket.__init__(self, 6)

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
					raise TftpException, "hit max retries, giving up"

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

	def __init__(self, host, port=69, options={}):

		self.options = options
		self.host = host
		self.port = port 
		self.options = options
		self.session = TftpClientSession(self.host, self.port)

		if self.options.has_key('blksize'):
			size = self.options['blksize']
			if size < TFTP_MIN_BLKSIZE or size > TFTP_MAX_BLKSIZE:
				raise TftpException, "Invalid blksize: %d" % size
		else:
		 	size = TFTP_DEF_BLKSIZE

		# FIXME: negotiate block size
		self.blksize = size 

	def put(self, data, filename, mode, timeout=2):

		self.session.timeout(timeout)

		if (mode == TFTP_MODE_NETASCII): 
			data = data.replace('\n', '\r\n')

		data_len = len(data)
		data_rem = data_len
		data_pos = 0
		blkno = 0
		last_data = False

		# prepare a write request
		wrq = TftpPacketWRQ(filename, mode, timeout)

		# send and receive 
		pkt = self.session.cycle(wrq)

		# we should receive ACK
		if not isinstance(pkt, TftpPacketACK):
			raise TftpException, "Invalid response."
		
		# the block number must match our initial block number
		if (pkt.blkno != blkno):
			raise TftpException, "Invalid block number %d." % pkt.blkno

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
			buf = data[data_pos:data_pos + n]
			data_pos += n;
			data_rem -= n;
			
			# create a new DATA packet
			pktData = TftpPacketDATA(blkno, buf)

			pkt = self.session.cycle(pktData)

			if not isinstance(pkt, TftpPacketACK):
				raise TftpException, "Invalid response."

			# FIXME: retransmit lost packet
			if (pkt.blkno != blkno):
				raise TftpException, "Invalid block number %d." % pkt.blkno

		# disconnect from the remote host
		self.session.disconnect()

	def end(self):
		pass


