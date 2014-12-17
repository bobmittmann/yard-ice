#ifndef __WAVEGEN_H__
#define __WAVEGEN_H__

#include <stdint.h>

struct wavegen {
	int16_t x;
	int16_t y; 
	int16_t a; /* amplitude */
	int16_t c; /* algorithm constant */
	unsigned int frame_len; /* frame lenght */
};

#define W_SAMPLE_RATE 8000

#define W_A0   1415 /*  A0   55.000Hz */
#define W_AS0  1500 /* A#0   58.270Hz */
#define W_B0   1589 /*  B0   61.735Hz */
#define W_C0   1683 /*  C0   65.406Hz */
#define W_CS0  1783 /* C#0   69.296Hz */
#define W_D0   1889 /*  D0   73.416Hz */
#define W_DS0  2001 /* D#0   77.782Hz */
#define W_E0   2120 /*  E0   82.407Hz */
#define W_F0   2246 /*  F0   87.307Hz */
#define W_FS0  2380 /* F#0   92.499Hz */
#define W_G0   2521 /*  G0   97.999Hz */
#define W_GS0  2671 /* G#0  103.826Hz */
#define W_A1   2830 /*  A1  110.000Hz */
#define W_AS1  2998 /* A#1  116.541Hz */
#define W_B1   3176 /*  B1  123.471Hz */
#define W_C1   3365 /*  C1  130.813Hz */
#define W_CS1  3565 /* C#1  138.591Hz */
#define W_D1   3777 /*  D1  146.832Hz */
#define W_DS1  4001 /* D#1  155.563Hz */
#define W_E1   4239 /*  E1  164.814Hz */
#define W_F1   4490 /*  F1  174.614Hz */
#define W_FS1  4757 /* F#1  184.997Hz */
#define W_G1   5039 /*  G1  195.998Hz */
#define W_GS1  5338 /* G#1  207.652Hz */
#define W_A2   5655 /*  A2  220.000Hz */
#define W_AS2  5990 /* A#2  233.082Hz */
#define W_B2   6345 /*  B2  246.942Hz */
#define W_C2   6721 /*  C2  261.626Hz */
#define W_CS2  7119 /* C#2  277.183Hz */
#define W_D2   7541 /*  D2  293.665Hz */
#define W_DS2  7987 /* D#2  311.127Hz */
#define W_E2   8460 /*  E2  329.628Hz */
#define W_F2   8960 /*  F2  349.228Hz */
#define W_FS2  9489 /* F#2  369.994Hz */
#define W_G2  10049 /*  G2  391.995Hz */
#define W_GS2 10641 /* G#2  415.305Hz */
#define W_A3  11268 /*  A3  440.000Hz */
#define W_AS3 11930 /* A#3  466.164Hz */
#define W_B3  12631 /*  B3  493.883Hz */
#define W_C3  13372 /*  C3  523.251Hz */
#define W_CS3 14155 /* C#3  554.365Hz */
#define W_D3  14982 /*  D3  587.330Hz */
#define W_DS3 15855 /* D#3  622.254Hz */
#define W_E3  16778 /*  E3  659.255Hz */
#define W_F3  17751 /*  F3  698.456Hz */
#define W_FS3 18777 /* F#3  739.989Hz */
#define W_G3  19859 /*  G3  783.991Hz */
#define W_GS3 20999 /* G#3  830.609Hz */
#define W_A4  22200 /*  A4  880.000Hz */
#define W_AS4 23462 /* A#4  932.328Hz */
#define W_B4  24788 /*  B4  987.767Hz */
#define W_C4  26181 /*  C4 1046.502Hz */
#define W_CS4 27641 /* C#4 1108.731Hz */
#define W_D4  29170 /*  D4 1174.659Hz */
#define W_DS4 30769 /* D#4 1244.508Hz */
#define W_E4  32437 /*  E4 1318.510Hz */

#define W_100HZ  2573 /*  100.0Hz */
#define W_200HZ  5142 /*  200.0Hz */
#define W_300HZ  7703 /*  300.0Hz */
#define W_400HZ 10252 /*  400.0Hz */
#define W_441HZ 11293 /*  441.0Hz */
#define W_500HZ 12785 /*  500.0Hz */
#define W_600HZ 15299 /*  600.0Hz */
#define W_700HZ 17789 /*  700.0Hz */
#define W_800HZ 20252 /*  800.0Hz */
#define W_886HZ 22363 /*  886.8Hz */
#define W_900HZ 22683 /*  900.0Hz */
#define W_1000HZ 25080 /* 1000.0Hz */
#define W_1102HZ 27496 /* 1102.5Hz */
#define W_1024HZ 25649 /* 1024.0Hz */
#define W_1100HZ 27437 /* 1100.0Hz */
#define W_1200HZ 29753 /* 1200.0Hz */
#define W_1300HZ 32022 /* 1300.0Hz */

#ifdef __cplusplus
extern "C" {
#endif

void wavegen_init(struct wavegen * gen, int16_t c, int16_t ampl,
                  unsigned int frame_len);
void wavegen_apply(struct wavegen * gen, int16_t out[]);

#ifdef __cplusplus
}
#endif
#endif /* __WAVEGEN_H__ */
