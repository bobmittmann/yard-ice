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

def isink(mode):
	
	r30 = 10000.0 # pwm output - SINK1
	r31 = 18200.0 # pwm output - SINK2
	r39 = 6190.0  # pwm output - SINK3
	r40 = 3090.0  # open drain - SINK4
	r32 = 6190.0

	r = [ r30, r31, r39, r40 ] ;

	i = 0
	# gnd connected equivalent resisor
	ra = r32
	while i < 4:
		if mode[i] == 0:
			ra = (ra * r[i]) / (ra + r[i])
		i = i + 1

	i = 0
	#vcc connected equivalent resistor
	rb = 10e9 
	while i < 4:
		if mode[i] == 1:
			rb = (rb * r[i]) / (rb + r[i])
		i = i + 1

	v = 3.3 * ra / (ra + rb)
	i = v / 4.99;

#	if (rb > 10e6):
#		print("RA={0:5.2f}K RB=~     ".format(ra / 1000, rb / 1000));
#	else:
#		print("RA={0:5.2f}K RB={1:5.2f}K "\
#			  "i={2:.0f}mA".format(ra / 1000, rb / 1000, i * 1000))

	return int(round(i * 1000))

#	print("RA={0:.2f}K, RB={1:.2f}K, i={2:.0f}mA"\
#		  .format(ra / 1000, rb/ 1000), i);

def v(x, y):
	return (x == y) or ((x > 0) and (y == 0))

def print_io(x, sink4):

	i = 0
	while i < 3:
		if (x[i][0] == 1):
			print("\t\tstm32_gpio_mode(SINK{0:d}, ALT_FUNC, "\
				  "PUSH_PULL | SPEED_HIGH);".format(i + 1))
		if (x[i][0] == 2): 
			print("\t\tstm32_gpio_mode(SINK{0:d}, ALT_FUNC, "\
				  "OPEN_DRAIN | SPEED_HIGH);".format(i + 1))
		if (x[i][0] == 0):
			print("\t\tstm32_gpio_mode(SINK{0:d}, OUTPUT, "\
				  "PUSH_PULL | SPEED_HIGH);".format(i + 1))
		i = i + 1

	if (sink4 == 1):
		print("\t\tstm32_gpio_set(SINK4);");
	else:
		print("\t\tstm32_gpio_clr(SINK4);");

	i = 0
	while i < 3:
		if (x[i][0] > 0) and (x[i][1] == 0):
#			print("\t\ttim->ccr{0:d} = t1;".format(i + 2))
			print("\t\tisink.s{0:d} = 1;".format(i + 1))
		else:
			print("\t\tisink.s{0:d} = 0;".format(i + 1))
#			print("\t\ttim->ccr{0:d} = 1;".format(i + 2))
		i = i + 1


def main():

	lo = []
	hi = []
	nm = [ 'GND ', 'VCC ', 'NC  ']

	z = 0
	while z < 3:
		y = 0
		while y < 3:
			x = 0
			while x < 3:
				i = isink([x, y, z, 0]);
				if (i > 1):
					lo.append([i, x, y, z])
				i = isink([x, y, z, 2]);
				if (i > 1):
					hi.append([i, x, y, z])
				x = x + 1
			y = y + 1
		z = z + 1

	lo.sort()
	hi.sort()

	pair = []
	n = 0
	while n < len(lo):
		m = 0
		while m < len(lo):
			i0 = lo[n][0]
			x0 = lo[n][1]
			y0 = lo[n][2]
			z0 = lo[n][3]
			i1 = lo[m][0]
			x1 = lo[m][1]
			y1 = lo[m][2]
			z1 = lo[m][3]
			if (i0 >= 2 * i1) and v(x0, x1) and v(y0, y1) and v(z0, z1):
				pair.append([lo[n], lo[m], 0])
			m = m + 1
		n = n + 1

	n = 0
	while n < len(hi):
		m = 0
		while m < len(hi):
			i0 = hi[n][0]
			x0 = hi[n][1]
			y0 = hi[n][2]
			z0 = hi[n][3]
			i1 = hi[m][0]
			x1 = hi[m][1]
			y1 = hi[m][2]
			z1 = hi[m][3]
			if (i0 >= 2 * i1) and v(x0, x1) and v(y0, y1) and v(z0, z1):
				pair.append([hi[n], hi[m], 1])
			m = m + 1
		n = n + 1
				

	pair.sort()

	print("\tswitch (mode) {")
	print("")

	n = 0
	while n < len(pair):
		i0 = pair[n][0][0]
		x0 = pair[n][0][1]
		y0 = pair[n][0][2]
		z0 = pair[n][0][3]
		i1 = pair[n][1][0]
		x1 = pair[n][1][1]
		y1 = pair[n][1][2]
		z1 = pair[n][1][3]
		s4 = pair[n][2]
		print("\tcase {0:d}:".format(n));
		print("\t/* ({:d} {:d})[mA] ({:d} {:d})[mV]"\
			  .format(i0, i1, i0 * 5, i1 * 5) + \
			  " [ " + nm[x0] + nm[y0] + nm[z0] + "]" + \
			  " [ " + nm[x1] + nm[y1] + nm[z1] + "]" + \
			  " {:s} */".format(nm[s4]))
		print_io([[x0, x1], [y0, y1], [z0, z1]], s4)
		print("\t\tbreak;")
		print("")
		n = n + 1

	print("\t}")
	print("")

	print("/*")
	print("\t| mode | i0(mA) | i1(mA) |")
	print("\t+------+--------+--------+")
	n = 0
	while n < len(pair):
		i0 = pair[n][0][0]
		i1 = pair[n][1][0]
		print("\t| {:4d} | {:6d} | {:6d} |" .format(n, i0, i1))
		n = n + 1

	print("\t+------+--------+--------+")
	print("*/");
	print("");

if __name__ == '__main__':
	main()

