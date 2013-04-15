#include <stdio.h>
#include <stdlib.h>
#include <sys/dcclog.h>

#include "trace.h"
#include "fft.h"
#include "vt100.h"
#include "spectrum.h"

#define FFT_LEN 256

const int16_t hann_window[FFT_LEN] = {
	0x0000, 0x0005, 0x0014, 0x002d, 0x0050, 0x007c, 0x00b3, 0x00f3,
	0x013d, 0x0191, 0x01ef, 0x0256, 0x02c7, 0x0341, 0x03c5, 0x0452,
	0x04e9, 0x0588, 0x0631, 0x06e3, 0x079d, 0x0861, 0x092d, 0x0a01,
	0x0ade, 0x0bc3, 0x0cb1, 0x0da6, 0x0ea3, 0x0fa8, 0x10b4, 0x11c7,
	0x12e2, 0x1404, 0x152d, 0x165c, 0x1792, 0x18ce, 0x1a10, 0x1b58,
	0x1ca6, 0x1df9, 0x1f51, 0x20af, 0x2211, 0x2378, 0x24e4, 0x2654,
	0x27c7, 0x293f, 0x2aba, 0x2c38, 0x2dba, 0x2f3e, 0x30c5, 0x324e,
	0x33d9, 0x3567, 0x36f6, 0x3886, 0x3a17, 0x3baa, 0x3d3d, 0x3ed0,
	0x4064, 0x41f8, 0x438b, 0x451e, 0x46af, 0x4840, 0x49d0, 0x4b5e,
	0x4cea, 0x4e75, 0x4ffd, 0x5182, 0x5305, 0x5485, 0x5602, 0x577b,
	0x58f1, 0x5a63, 0x5bd0, 0x5d3a, 0x5e9e, 0x5ffe, 0x615a, 0x62af,
	0x6400, 0x654b, 0x6690, 0x67cf, 0x6908, 0x6a3b, 0x6b67, 0x6c8c,
	0x6daa, 0x6ec1, 0x6fd1, 0x70da, 0x71db, 0x72d4, 0x73c5, 0x74ae,
	0x758f, 0x7668, 0x7738, 0x7800, 0x78bf, 0x7975, 0x7a22, 0x7ac7,
	0x7b62, 0x7bf3, 0x7c7c, 0x7cfb, 0x7d71, 0x7ddd, 0x7e3f, 0x7e98,
	0x7ee7, 0x7f2c, 0x7f68, 0x7f99, 0x7fc1, 0x7fdf, 0x7ff3, 0x7ffd,
	0x7ffd, 0x7ff3, 0x7fdf, 0x7fc1, 0x7f99, 0x7f68, 0x7f2c, 0x7ee7,
	0x7e98, 0x7e3f, 0x7ddd, 0x7d71, 0x7cfb, 0x7c7c, 0x7bf3, 0x7b62,
	0x7ac7, 0x7a22, 0x7975, 0x78bf, 0x7800, 0x7738, 0x7668, 0x758f,
	0x74ae, 0x73c5, 0x72d4, 0x71db, 0x70da, 0x6fd1, 0x6ec1, 0x6daa,
	0x6c8c, 0x6b67, 0x6a3b, 0x6908, 0x67cf, 0x6690, 0x654b, 0x6400,
	0x62af, 0x615a, 0x5fff, 0x5e9e, 0x5d3a, 0x5bd0, 0x5a63, 0x58f1,
	0x577b, 0x5602, 0x5485, 0x5305, 0x5182, 0x4ffd, 0x4e75, 0x4cea,
	0x4b5e, 0x49d0, 0x4840, 0x46af, 0x451e, 0x438b, 0x41f8, 0x4064,
	0x3ed0, 0x3d3d, 0x3baa, 0x3a17, 0x3886, 0x36f6, 0x3567, 0x33d9,
	0x324e, 0x30c5, 0x2f3e, 0x2dba, 0x2c38, 0x2aba, 0x293f, 0x27c7,
	0x2654, 0x24e4, 0x2378, 0x2211, 0x20af, 0x1f51, 0x1df9, 0x1ca6,
	0x1b58, 0x1a10, 0x18ce, 0x1792, 0x165c, 0x152d, 0x1404, 0x12e2,
	0x11c7, 0x10b4, 0x0fa8, 0x0ea3, 0x0da6, 0x0cb1, 0x0bc3, 0x0ade,
	0x0a01, 0x092d, 0x0861, 0x079d, 0x06e3, 0x0631, 0x0588, 0x04e9,
	0x0452, 0x03c5, 0x0341, 0x02c7, 0x0256, 0x01ef, 0x0191, 0x013d,
	0x00f3, 0x00b3, 0x007c, 0x0050, 0x002d, 0x0014, 0x0005, 0x0000 };

void spectrum_run(struct spectrum * sa)
{
	cplx16_t fft_in[FFT_LEN];
	cplx16_t fft_out[FFT_LEN];
	sndbuf_t * buf;
	int32_t v;
	int pos;
	int i;
	int j;

	sa->locked = true;
	pos = sa->frm_cnt;

	for (j = 0; j < (FFT_LEN / SNDBUF_LEN); ++j) {
		buf = sa->ring[pos++ & ((FFT_LEN / SNDBUF_LEN) - 1)];
		if (buf == NULL) {
			DCC_LOG(LOG_WARNING, "buf == NULL!");
			for (i = 0; i < SNDBUF_LEN; ++i) {
				fft_in[j * SNDBUF_LEN + i].re = 0;
				fft_in[j * SNDBUF_LEN + i].im = 0;
			}
		} else {
			for (i = 0; i < SNDBUF_LEN; ++i) {
				v = Q15_MUL(buf->data[i], hann_window[j * SNDBUF_LEN + i]);
//				v = buf->data[i];
				fft_in[j * SNDBUF_LEN + i].re = v;
				fft_in[j * SNDBUF_LEN + i].im = 0;
			}
		}
	}

	sa->locked = false;

	/* FFT calculation */
	fftR4(fft_out, fft_in, FFT_LEN);

	for (i = 0; i < (FFT_LEN / 2); ++i) {
		/* Accumulate the absolute values */
		sa->mag[i] += cplx16_abs(fft_out[i]);
	}

	sa->run_cnt++;
}

void spectrum_normalize(struct spectrum * sa)
{
	int i;

	if (sa->run_cnt == 0)
		return;

	for (i = 0; i < (FFT_LEN / 2); ++i) {
		sa->mag[i] /= sa->run_cnt;
	}

	sa->run_cnt = 1;
}

void spectrum_reset(struct spectrum * sa)
{
	int i;

	for (i = 0; i < (FFT_LEN / 2); ++i)
		sa->mag[i] = 0;

	sa->run_cnt = 0;
}

void spectrum_init(struct spectrum * sa, unsigned int rate)
{
	int i;

	for (i = 0; i < (FFT_LEN / 2); ++i)
		sa->mag[i] = 0;

	for (i = 0; i < (FFT_LEN / SNDBUF_LEN); ++i)
		sa->ring[i] = NULL;

	sa->run_cnt = 0;
	sa->frm_cnt = 0;
	sa->locked = false;
	sa->rate = rate;
}

void spectrum_rec(struct spectrum * sa, sndbuf_t * buf)
{
	int pos;

	/* spectrum analizer is locked */
	if (sa->locked) {
		DCC_LOG(LOG_TRACE, "locked!");
		return;
	}

	/* increment the buffer reference counter */
	if ((buf = sndbuf_use(buf)) == NULL) {
		DCC_LOG(LOG_TRACE, "buffer invalid!");
		return;
	}

	/* insert into the queue */
	pos = sa->frm_cnt++ & ((FFT_LEN / SNDBUF_LEN) - 1);

	if (sa->ring[pos] != NULL) {
		/* discard old buffers */
		sndbuf_free(sa->ring[pos]);
	}

	sa->ring[pos] = buf;
}

void spectrum_print(struct spectrum * sa)
{
	int max = 0;
	int i;
	int f;
	int j;
	int n;

	for (i = 0; i < FFT_LEN / 2; ++i) {
		if (sa->mag[i] > max)
			max = sa->mag[i];
	}

	for (i = 0; i < FFT_LEN / 2; ++i) {

		f = (i * sa->rate + (FFT_LEN / 2)) / FFT_LEN;
		printf("%4d %5d ", f, sa->mag[i]);
		n = (sa->mag[i] * 64) / max;
		for (j = 0; j < n; j++)
			printf("#");
		printf("\n");
	}
}

void spectrum_pwr_show(struct spectrum * sa)
{
	uint32_t pwr[FFT_LEN / 2];
	char ln[160];
	char * cp;
	int i;
	int x;
	int y;
	char mark;	

	printf(VT100_CURSOR_HIDE); 

	for (i = 0; i < (FFT_LEN / 2); ++i) {
		pwr[i] = Q15_UMUL(sa->mag[i], sa->mag[i]);
	}

	for (y = 4; y < 41; ++y) {
		cp = ln;
		cp += sprintf(cp, VT100_GOTO, y - 2, 1); 
		mark = (y % 10) ? ' ' : '-';

		for (x = 0; x < 80; ++x) {
			cp[x] = (pwr[x] >= q15_db2pwr_ltu[y]) ? '|' : mark;
		}
		cp[x] = '\0';
		printf(ln); 
	}
}

void spectrum_mag_show(struct spectrum * sa)
{
	char ln[160];
	char mark;	
	char * cp;
	int x;
	int y;

	printf(VT100_CURSOR_HIDE); 

	for (y = 1; y < 41; ++y) {
		cp = ln;
		cp += sprintf(cp, VT100_GOTO, y + 1, 1); 
		mark = (y % 10) ? ' ' : '-';

		for (x = 0; x < 80; ++x) {
			cp[x] = (sa->mag[x] >= q15_db2amp_ltu[y * 2]) ? '|' : mark;
		}
		cp[x] = '\0';
		printf(ln); 
	}
}

