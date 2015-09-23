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
	
	r40 = 66500.0 # pwm output - SINK1
	r30 = 44200.0 # pwm output - SINK2
	r35 = 39200.0  # pwm output - SINK3
	r39 = 24000.0  # open drain - SINK4
	r32 = 6190.0

	r = [ r40, r30, r35, r39 ];

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

def print_io(x):
	i = 0
	while i < 4:
		if (x[i][0] == 1):
			print("\t\tstm32_gpio_mode(IO_SINK{0:d}, ALT_FUNC, "\
				  "PUSH_PULL | SPEED_HIGH);".format(i + 1))
		if (x[i][0] == 2): 
			print("\t\tstm32_gpio_mode(IO_SINK{0:d}, ALT_FUNC, "\
				  "OPEN_DRAIN | SPEED_HIGH);".format(i + 1))
		if (x[i][0] == 0):
			print("\t\tstm32_gpio_mode(IO_SINK{0:d}, OUTPUT, "\
				  "PUSH_PULL | SPEED_HIGH);".format(i + 1))
		i = i + 1

	i = 0
	while i < 4:
		if x[i][1] == 0:
			print("\t\ts{:d} = -1;".format(i + 1))
		else:
			print("\t\ts{:d} = 0;".format(i + 1))
		i = i + 1


def v(x, y):
	return (x == y) or ((x > 0) and (y == 0))

def main():

	i_lst = []
	nm = [ 'GND ', 'VCC ', 'NC  ']

	z = 0
	while z < 3:
		y = 0
		while y < 3:
			x = 0
			while x < 3:
				w = 0
				while w < 3:
					i = isink([x, y, z, w]);
					if (i > 1):
						i_lst.append([i, w, x, y, z])
					w = w + 1
				x = x + 1
			y = y + 1
		z = z + 1

	i_lst.sort()

	q_lst = []
	for n in i_lst:
		for m in i_lst:
			i0 = n[0]
			w0 = n[1]
			x0 = n[2]
			y0 = n[3]
			z0 = n[4]
			i1 = m[0]
			w1 = m[1]
			x1 = m[1]
			y1 = m[2]
			z1 = m[3]
			if (i0 >= 1.8 * i1) and v(w0, w1) and v(x0, x1) \
					  and v(y0, y1) and v(z0, z1):
				q_lst.append([n, m])

	p_lst = q_lst
	for x in q_lst:
		for y in q_lst:
			d0 = abs(x[0][0] - y[0][0])
			d1 = abs(x[1][0] - y[1][0])
			r0 = x[0][0] * 0.05
			r1 = x[1][0] * 0.05
			if (d1 < r1) and (d0 < r0):
				p_lst.remove(y)
		

	print("static void isink_io_cfg(unsigned int mode)")
	print("{")
	print("\tint32_t s1;")
	print("\tint32_t s2;")
	print("\tint32_t s3;")
	print("\tint32_t s4;")
	print("")
	print("\tswitch (mode) {")
	print("")

	j = 0
	for n in p_lst:
		i0 = n[0][0]
		w0 = n[0][1]
		x0 = n[0][2]
		y0 = n[0][3]
		z0 = n[0][4]
		i1 = n[1][0]
		w1 = n[1][1]
		x1 = n[1][2]
		y1 = n[1][3]
		z1 = n[1][4]
		print("\tcase {0:d}:".format(j))
		j = j + 1
		s = "\t/* ("
		s += "{:d} {:d})[mA] ({:d} {:d})[mV]".format(i0, i1, i0 * 5, i1 * 5)
		s += " [ " + nm[w0] + nm[x0] + nm[y0] + nm[z0] + "]" 
		s += " [ " + nm[w1] + nm[x1] + nm[y1] + nm[z1] + "] */"
		print(s)

		print_io([[w0, w1], [x0, x1], [y0, y1], [z0, z1]])

		print("\t\tbreak;")
		print("")
	print("\t}")
	print("")

	print("\tisink_drv.s1 = s1;")
	print("\tisink_drv.s2 = s2;")
	print("\tisink_drv.s3 = s3;")
	print("\tisink_drv.s4 = s4;")
	print("}")
	print("")

	print("/*")
	print("\t| mode | i0(mA) | i1(mA) |")
	print("\t+------+--------+--------+")
	j = 0
	for n in p_lst:
		i0 = n[0][0]
		i1 = n[1][0]
		print("\t| {:4d} | {:6d} | {:6d} |" .format(j, i0, i1))
		j = j + 1

	print("\t+------+--------+--------+")
	print("*/");
	print("");

if __name__ == '__main__':
	main()

