#!python

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

tab_lst = []

def make_tab(rate, lvl, offs, typ, nm, freq):
	i = 0


	if (freq == 0):
		n = 2
		over = 1
		err = 0;
	else:
		over = 1;

		freq = freq * 1.0

		x = ((over * rate) / freq);
		err = abs(round(x) - x);

		over_min = over
		x_min = x;
		err_min = err;

		while (over < 16) & (err > 0.05):
			over = over + 1
			x = ((over * rate) / freq);
			err = abs(round(x) - x);
			if (err < err_min):
				over_min = over
				err_min = err
				x_min = x


		over = over_min
		err = err_min
		x = x_min

		n = round(x)

	print("/* over={:d} err={:f} freq={:.3f} */".format(over, err, (over * rate) / n))

	print("static const {:s} {:s}[] = /* {:.3f} Hz */".format(typ, nm, freq))
	print("{")


	while (i < n):
		x = ((2.0 * over) * pi * i) / n
		y = sin(x)
		z = round(y * (lvl - 1)) + offs;
		if ((i % 8) == 0):
			print("\t{:6d},".format(z), end="")
		else:
			if ((i % 8) == 7):
				print("{:6d},".format(z))
			else:
				print(" {:6d},".format(z), end="")
		i = i + 1

	if ((i % 8) != 0):
		print("")

	print("};")
	print("")

	tab_lst.append((nm, freq, n))

def main():

	rate = 12000
	lvl = 128;
	offs = 128;
	typ = "uint8_t";

	rate = 11025
	lvl = 10000;
	offs = 0;
	typ = "int16_t";

	print("#ifndef __WAVETAB_H__")
	print("#define __WAVETAB_H__")
	print("")
	print("#include <stdint.h>")
	print("")
	print("#define SAMPLE_RATE {:d}".format(rate))
	print("")


	make_tab(rate, lvl, offs, typ, "wave_dc", 0)
	make_tab(rate, lvl, offs, typ, "wave_a3", 440)
	make_tab(rate, lvl, offs, typ, "wave_a4", 880)
	make_tab(rate, lvl, offs, typ, "wave_a4s", 932.328)
	make_tab(rate, lvl, offs, typ, "wave_b4", 987.767)
	make_tab(rate, lvl, offs, typ, "wave_c4", 1046.502)
	make_tab(rate, lvl, offs, typ, "wave_c4s", 1108.731)
	make_tab(rate, lvl, offs, typ, "wave_d4", 1174.659)
	make_tab(rate, lvl, offs, typ, "wave_d4s", 1244.508)
	make_tab(rate, lvl, offs, typ, "wave_e4", 1318.510)
	make_tab(rate, lvl, offs, typ, "wave_f4", 1396.913)
	make_tab(rate, lvl, offs, typ, "wave_f4s", 1479.978)
	make_tab(rate, lvl, offs, typ, "wave_g4", 1567.982)
	make_tab(rate, lvl, offs, typ, "wave_g4s", 1661.219)
	make_tab(rate, lvl, offs, typ, "wave_1khz", 1000)

	print("")

	print("static const struct {")
	print("\tconst {:s} * buf;".format(typ))
	print("\tuint32_t len;")
	print("\tuint32_t freq;")
	print("} wave_lut[] = {")
	for e in tab_lst:
		freq = e[1];
		print("\t{")
		print("\t\t.buf = {:s},".format(e[0]))
		print("\t\t.len = sizeof({:s})/sizeof({:s}),".format(e[0], typ))
		print("\t\t.freq = {:d}".format(int(freq)))
		print("\t},")
	print("};")
	print("")

	i = 0;
	for e in tab_lst:
		print("#define {:s} {:d}".format(e[0].upper(), i))
		i = i + 1
	print("")

	print("const unsigned int wave_max = {:d};".format(i - 1))
	print("")
	print("#ifdef __cplusplus")
	print("extern \"C\" {")
	print("#endif")
	print("")
	print("")
	print("#ifdef __cplusplus")
	print("}")
	print("#endif")
	print("#endif /* __WAVETAB_H__ */")
	print("")


if __name__ == "__main__":
    main()
