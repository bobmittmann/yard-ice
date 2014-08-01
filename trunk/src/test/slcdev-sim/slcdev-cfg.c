#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/dcclog.h>

struct slcdev_obj {
	uint32_t device_type : 7;
	uint32_t advanced_protocol : 1;

	uint32_t enabled : 1;
	uint32_t poll_flash : 1;
	uint32_t isink_pulse_level : 4;
	uint32_t isink_slewrate : 2;

	uint32_t isink_width_err : 3;
	uint32_t isink_latency : 5;  /* tm = (x + 1) * 5 ( 5us .. 160us) */

	uint32_t isink_pulse_pre : 5; /* tm = (x + 1) * 5 ( 5us .. 160us) */
};


struct ratio_u16 {
	int16_t num;
	uint16_t den;
};

const struct ratio_u16 sim_err_ltu[] = {
	{    1,     1 }, /*   0 % */
	{  1020, 1000 }, /*   2 % */
	{  1040, 1000 }, /*   4 % */
	{  1080, 1000 }, /*   8 % */
	{   840, 1000 }, /* -16 % */
	{   920, 1000 }, /*  -8 % */
	{   960, 1000 }, /*  -4 % */
	{   920, 1000 }  /*  -2 % */
};

int slcdev_sim_set(void)
{

	return 0;
}

