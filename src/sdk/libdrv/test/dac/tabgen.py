#!/usr/bin/python2

from math import *

scale = ( \
	("A",  "", ( 55.000, 110.000, 220.000, 440.000,  880.000, 1760.000)), \
	("A", "#", ( 58.270, 116.541, 233.082, 466.164,  932.328, 1864.655)), \
	("B",  "", ( 61.735, 123.471, 246.942, 493.883,  987.767, 1975.533)), \
	("C",  "", ( 65.406, 130.813, 261.626, 523.251, 1046.502, 2093.005)), \
	("C", "#", ( 69.296, 138.591, 277.183, 554.365, 1108.731, 2217.461)), \
	("D",  "", ( 73.416, 146.832, 293.665, 587.330, 1174.659, 2349.318)), \
	("D", "#", ( 77.782, 155.563, 311.127, 622.254, 1244.508, 2489.016)), \
	("E",  "", ( 82.407, 164.814, 329.628, 659.255, 1318.510, 2637.020)), \
	("F",  "", ( 87.307, 174.614, 349.228, 698.456, 1396.913, 2793.826)), \
	("F", "#", ( 92.499, 184.997, 369.994, 739.989, 1479.978, 2959.955)), \
	("G",  "", ( 97.999, 195.998, 391.995, 783.991, 1567.982, 3135.963)), \
	("G", "#", (103.826, 207.652, 415.305, 830.609, 1661.219, 3322.438)))


freq = (100, 200, 300, 400, 441, 500, 600, 700, 800, 886.76, 900, 
		1000, 1102.5, 1024, 1100, 1200, 1300, 
		1400, 1500, 1600, 1700, 1800, 1900, 2000)

def make_tab(nm, freq, rate):
	print "uint8_t %s[] = /* %d */" % (nm, freq)
	print "{"

	i = 0
	if ((rate % freq) == 0):
		over = 1
	else:
	 	over = 4

	n = ((over * rate) + (freq / 2)) / freq
	while (i < n):
		x = ((2.0 * over) * pi * i) / n
		y = sin(x)
		z = round(y * 127) + 128;
		print "\t%3d," % (z)
		i = i + 1

	print "};"
	print ""

def main():

	rate = 16000

	print "#ifndef __WAVETAB_H__"
	print "#define __WAVETAB_H__"
	print ""
	print "#include <stdint.h>"
	print ""
	print "#define SAMPLE_RATE %d" % rate
	print ""

	make_tab("a4", 880, rate)
	make_tab("a4s", 932, rate)
	make_tab("b4", 988, rate)
	make_tab("c4", 1046, rate)
	make_tab("c4s", 1108, rate)
	make_tab("d4", 1175, rate)
	make_tab("d4s", 1244, rate)
	make_tab("e4", 1318, rate)
	make_tab("f4", 1397, rate)
	make_tab("f4s", 1480, rate)
	make_tab("g4", 1568, rate)
	make_tab("g4s", 1661, rate)
	make_tab("sin1khz", 1000, rate)

	print ""
	print "#ifdef __cplusplus"
	print "extern \"C\" {"
	print "#endif"
	print ""
	print ""
	print "#ifdef __cplusplus"
	print "}"
	print "#endif"
	print "#endif /* __WAVETAB_H__ */"
	print ""


if __name__ == "__main__":
    main()
