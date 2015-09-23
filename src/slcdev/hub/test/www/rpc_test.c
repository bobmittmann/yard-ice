/*
 * rpc_test.c
 *
 *  Created on: Sep 23, 2015
 *      Author: bmittmann
 */
#include <stdlib.h>
#include <stdio.h>
#include <simrpc.h>
#include <trace.h>

void rpc_test_file_write(FILE * f, int port)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;
	char s[128];
	int n;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
		return;
	}

	if (simrpc_file_create(sp, "cfg.js") < 0) {
		WARN("simrpc_file_create() failed!");
		simrpc_close(sp);
		return;
	}

	n = sprintf(s, "The quick brown fox jumps over the lazy dog!\n");

	if (simrpc_file_write(sp, s, n) < 0) {
		WARN("simrpc_file_write() failed!");
	}

	if (simrpc_file_close(sp) < 0) {
		WARN("simrpc_file_close() failed!");
	}

	simrpc_close(sp);
}

void rpc_test_file_read(FILE * f, int port)
{
	unsigned int daddr = port;
	struct simrpc_pcb * sp;
	char s[128];
	int n;

	fprintf(f, "%s(): ...\n", __func__);

	if ((sp = simrpc_open(daddr)) == NULL) {
		WARN("simrpc_open() failed!");
		return;
	}

	if (simrpc_file_open(sp, "cfg.js") < 0) {
		WARN("simrpc_file_create() failed!");
		simrpc_close(sp);
		return;
	}

	if ((n = simrpc_file_read(sp, s, 128)) < 0) {
		WARN("simrpc_file_read() failed!");
	} else {
		s[n] = '\0';
		fprintf(f, "\"%s\"", s);
	}

	if (simrpc_file_close(sp) < 0) {
		WARN("simrpc_file_close() failed!");
	}

	simrpc_close(sp);
}
