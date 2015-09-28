#!/usr/bin/env python

#
# copyright(c) 2012 robinson mittmann. all rights reserved.
# 
# this file is part of the yard-ice.
#
# this library is free software; you can redistribute it and/or
# modify it under the terms of the gnu lesser general public
# license as published by the free software foundation; either
# version 3.0 of the license, or (at your option) any later version.
# 
# this library is distributed in the hope that it will be useful,
# but without any warranty; without even the implied warranty of
# merchantability or fitness for a particular purpose.  see the gnu
# lesser general public license for more details.
# 
# you can receive a copy of the gnu lesser general public license from 
# http://www.gnu.org/

#
#    file: isink.py
# comment: current sink waveform 
#  author: robinson mittmann <bobmittmann@gmail.com>
# 

#!/usr/bin/env python

import socket

UDP_IP = "0.0.0.0"
UDP_PORT = 1111

def main():
	print('--------------------------------------------------')
	print('UDP port {:d}'.format(UDP_PORT))
	print('--------------------------------------------------')

	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
	sock.bind((UDP_IP, UDP_PORT))

	while True:
		data, addr = sock.recvfrom(1024) 
		print(data)

if __name__ == '__main__':
	main()

