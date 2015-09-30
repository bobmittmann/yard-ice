/* 
 * Copyright(C) 2012 Robinson Mittmann. All Rights Reserved.
 * 
 * This file is part of the YARD-ICE.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file webserver.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <thinkos.h>
#include <trace.h>
#include "www/www.h"

struct httpddir httpd_dir[] = {
	{ .path = "/", .objlst = www_root },
	{ .path = "/lib", .objlst = www_lib },
	{ .path = "/img", .objlst = www_img },
	{ .path = "/ipcfg", .objlst = www_ipcfg },
	{ .path = NULL, .objlst = NULL }
};

int httpd_task(struct httpd * httpd)
{
	struct httpctl httpctl;
	struct httpctl * ctl = &httpctl;
	const struct httpdobj * obj;

	INF("Webserver started (thread %d).", thinkos_thread_self());

	for (;;) {
		if (http_accept(httpd, ctl) < 0) {
			ERR("tcp_accept() failed!");
			thinkos_sleep(100);
			continue;
		}

		if ((obj = http_obj_lookup(ctl)) != NULL) {
			switch (ctl->method) {
			case HTTP_GET:
				DBG("HTTP GET \"%s\"", obj->oid);
				http_get(ctl, obj);
				break;
			case HTTP_POST:
				DBG("HTTP POST \"%s\"", obj->oid);
				http_post(ctl, obj);
				break;
			}
		}

		http_close(ctl);
	}

	return 0;
}

#define HTTPD_STACK_SIZE (2048 + 512)
#define HTTPD_TASK_CNT 4

uint32_t httpd_stack[HTTPD_TASK_CNT][HTTPD_STACK_SIZE / 4] __attribute__((section (".ccm")));

const struct thinkos_thread_inf httpd_inf[HTTPD_TASK_CNT] = {
	{
		.stack_ptr = httpd_stack[0],
		.stack_size = HTTPD_STACK_SIZE,
		.priority = 32,
		.thread_id = 7,
		.paused = false,
		.tag = "HTTPD1"
	},
	{
		.stack_ptr = httpd_stack[1],
		.stack_size = HTTPD_STACK_SIZE,
		.priority = 32,
		.thread_id = 8,
		.paused = false,
		.tag = "HTTPD2"
	},
	{
		.stack_ptr = httpd_stack[2],
		.stack_size = HTTPD_STACK_SIZE,
		.priority = 32,
		.thread_id = 9,
		.paused = false,
		.tag = "HTTPD4"
	},
	{
		.stack_ptr = httpd_stack[3],
		.stack_size = HTTPD_STACK_SIZE,
		.priority = 32,
		.thread_id = 10,
		.paused = false,
		.tag = "HTTPD4"
	},
};

int webserver_start(void)
{
	struct httpd * httpd;
	int i;

	httpd = httpd_alloc();

	if (httpd_init(httpd, 80, 4, httpd_dir, NULL) < 0) {
		INF("Webserver initialization failed!");
		return -1;
	}

	for (i = 0; i < HTTPD_TASK_CNT; ++i) {
		thinkos_thread_create_inf((void *)httpd_task, httpd, &httpd_inf[i]);
	}

	return 0;
}
