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


def main():

	rate = 8000

	print "#ifndef __WAVEGEN_H__"
	print "#define __WAVEGEN_H__"
	print ""
	print "#include <stdint.h>"
	print ""
	print "struct wavegen {"
	print "\tint16_t x;"
	print "\tint16_t y; "
	print "\tint16_t a; /* amplitude */"
	print "\tint16_t c; /* algorithm constant */"
	print "\tunsigned int frame_len; /* frame lenght */"
	print "};"
	print ""
	print "#define W_SAMPLE_RATE %d" % rate
	print ""

	lst = []
	i = 0
	while (i < 12):
		n = scale[i]
		f = n[2]
		j = 0
		while (j < 6):
			if n[1] == "#":
				sym = "%sS%d" % (n[0], j)
				note = "%s#%d" % (n[0], j)
			else:
				sym = "%s%d" % (n[0], j)
				note = "%s%d" % (n[0], j)

			w0 = (2.0 * pi * f[j]) / rate
			cf = 2.0 * sin(w0 / 2.0)
#			print nm, cmt
#			print "#define %-3s Q15(%7.4f) %s" % (nm, c, cmt)
			if (cf < 1.0):
				lst.append((f[j], sym, cf, note))
			j = j + 1
		i = i + 1 

	lst.sort()

	i = 0
	for e in lst:
		f = e[0]
		sym = e[1]
		c = round(e[2] * 32768)
		note = e[3]
		cmt = "/* %3s %8.3fHz */" % (note, f)
		print "#define W_%-3s %5d %s" % (sym, c, cmt)
		i = i + 1 

	print

	i = 0
	for f in freq:
		sym = "W_%dHZ" % (f)
		w0 = (2.0 * pi * f) / rate
		cf = 2.0 * sin(w0 / 2.0)
		if (cf < 1.0):
			c = round(cf * 32768)
			cmt = "/* %6.1fHz */" % (f)
			print "#define %-7s %5d %s" % (sym, c, cmt)
		i = i + 1 

	print ""
	print "#ifdef __cplusplus"
	print "extern \"C\" {"
	print "#endif"
	print ""
	print "void wavegen_init(struct wavegen * gen, int16_t c, int16_t ampl,"
	print "                  unsigned int frame_len);"

	print "void wavegen_apply(struct wavegen * gen, int16_t out[]);"
	print ""
	print "#ifdef __cplusplus"
	print "}"
	print "#endif"
	print "#endif /* __WAVEGEN_H__ */"
	print ""


if __name__ == "__main__":
    main()
