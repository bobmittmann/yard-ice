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
 * @file debugger.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <trace.h>

#define LOG_LEVEL LOG_MSG
#include <sys/dcclog.h>

#include <yard-ice/drv.h>

#include "var.h"

#include "dbglog.h"
#include "debugger.h"
#include "jtag.h"
#include "cm3ice.h"
#include "armice.h"

#include "fmt.h"
#include "module.h"
#include "nand.h"

#ifndef DBG_BREAKPOINT_MAX
#define DBG_BREAKPOINT_MAX 16
#endif

#ifndef DBG_WATCHPOINT_MAX
#define DBG_WATCHPOINT_MAX 16
#endif

#define ENABLE_ICE_POLLING 1

/* Debugger state data */
struct debugger debugger;

/***********************************************************************
 Breakpoint management
 ***********************************************************************/

/* Breakpoint allocation data */
static struct dbg_bp dbg_bp_pool[DBG_BREAKPOINT_MAX];

/* Breakpoint management data */
/* This is a list of breakpoints ordered from the most to the least recently
   used.
   In most cases the target can support a limited number of active 
   breakpoints, in this case the top of this list will 
   correspond to the target's hardware ones.
 */
static struct dbg_bp * dbg_bp_list[DBG_BREAKPOINT_MAX];

static void dbg_bp_init(struct dbg_bp_ctrl * bpctl)
{
	struct dbg_bp * bp;
	int i;

	/* initialize the breakpoint poll.
	 The breakpoint poll consist of a linked list of free
	 breakpoint structures */
	bpctl->free = dbg_bp_pool;
	bpctl->lst = dbg_bp_list;
	bpctl->cnt = 0;

	bp = bpctl->free;
	for (i = 1; i < DBG_BREAKPOINT_MAX; i++) {
		bp->next = &dbg_bp_pool[i];
		bp = &dbg_bp_pool[i];
	}
	bp->next = NULL;

}

static struct dbg_bp * dbg_bp_new(struct dbg_bp_ctrl * bpctl, 
								  uint32_t addr, uint32_t size)
{
	struct dbg_bp * bp;

	if ((bp = bpctl->free) != NULL)
		bpctl->free = bp->next;

	bp->addr = addr;
	bp->size = size;
	bp->active = 0;

	/* insert at the list's tail,
	   if the list is full drop the last entry */

	if (bpctl->cnt < DBG_BREAKPOINT_MAX)
		bpctl->cnt++;

	bpctl->lst[bpctl->cnt - 1] = bp;
	
	DCC_LOG1(LOG_INFO, "cnt=%d", bpctl->cnt);

	return bp;
}

static void dbg_bp_move_to_head(struct dbg_bp_ctrl * bpctl, struct dbg_bp * bp)
{
	struct dbg_bp * tmp = NULL;
	struct dbg_bp * prev = bp;
	int i;

	DCC_LOG1(LOG_INFO, "bpctl->cnt=%d", bpctl->cnt);

	/* moving the list (shifting) */
	for (i = 0; i < bpctl->cnt; i++) {
		DCC_LOG1(LOG_INFO, "i=%d", i);
		tmp = bpctl->lst[i];
		bpctl->lst[i] = prev;
		if (tmp == bp) {
			break;
		}
		prev = tmp;
	}
}

static int dbg_bp_delete(ice_drv_t * ice, struct dbg_bp_ctrl * bpctl, 
						 struct dbg_bp * bp)
{
	int i;
	int n;

	/* look up for this breakpoint in the list */
	for (i = 0; i < bpctl->cnt; i++) {
		if (bp == bpctl->lst[i])
			break;
	}

	n = i;

	if (n == bpctl->cnt) {
		DCC_LOG(LOG_EXCEPT, "not in the list!!!");
		abort();
	}

	/* one less item in the list */
	bpctl->cnt--;

	/* moving the remaining items in the list one position to the front */
	for (i = n; i < bpctl->cnt; i++) {
		bpctl->lst[i] = bpctl->lst[i + 1];
	}

	DCC_LOG3(LOG_INFO, "n=%d cnt=%d ice->opt.bp_max=%d", 
			 n, bpctl->cnt, ice->opt.bp_max);

	if (bp->active) {
		/* clear the current breakpoint */
		DCC_LOG1(LOG_INFO, "clearing active BP: %d", bp->hw_id);
		ice_bp_clr(ice, bp->hw_id);

		/* activate the next if not disabled */
		if (bpctl->cnt >= ice->opt.bp_max) {
			struct dbg_bp * nxt;
			uint32_t id;

			nxt = bpctl->lst[ice->opt.bp_max - 1];
			if (nxt->enabled) {
				DCC_LOG(LOG_INFO, "seting next BP on list");
				if (ice_bp_set(ice, nxt->addr, nxt->size, &id) < 0) {
					DCC_LOG(LOG_EXCEPT, "ice_bp_set() error!");
				}
				nxt->active = 1;
				nxt->hw_id = id;
			}
		}
	} 

	/* release memory */
	bp->next = bpctl->free;
	bpctl->free = bp;

	return 0;
}

static int dbg_bp_enable_all(ice_drv_t * ice, struct dbg_bp_ctrl * bpctl)
{
	struct dbg_bp * bp = NULL;
	uint32_t id;
	int ret = 0;
	int n;
	int i;

	/* activate the breakpoints */
	n = (bpctl->cnt > ice->opt.bp_max) ? ice->opt.bp_max: bpctl->cnt;
	for (i = 0; i < n; i++) {
		bp = bpctl->lst[i];
		if (!bp->active) {
			DCC_LOG(LOG_INFO, "activating breakpoint...");
			if ((ret = ice_bp_set(ice, bp->addr, bp->size, &id)) < 0) {
				DCC_LOG(LOG_ERROR, "ice_bp_set() error!");
				return ret;
			}
			bp->active = 1;
			bp->hw_id = id;
		}
		bp->enabled = 1;
	}
	/* enable the remaining */
	for (i = n; i < bpctl->cnt; i++) {
		bp = bpctl->lst[i];
		bp->enabled = 1;
	}

	return ret;
}

static int dbg_bp_disable_all(ice_drv_t * ice, struct dbg_bp_ctrl * bpctl)
{
	struct dbg_bp * bp = NULL;
	int ret = 0;
	int i;

	/* disable all the breakpoints */
	for (i = 0; i < bpctl->cnt; i++) {
		bp = bpctl->lst[i];
		if (bp->active) {
			/* deactivate the breakpoints */
			DCC_LOG1(LOG_INFO, "deactivating breakpoint %d", i);
			if ((ret = ice_bp_clr(ice, bp->hw_id)) < 0) {
				DCC_LOG(LOG_ERROR, "ice_bp_clr() error!");
				return ret;
			}
			bp->active = 0;
			bp->hw_id = -1;
		}
		bp->enabled = 0;
	}

	return ret;
}

static int dbg_bp_activate(ice_drv_t * ice, struct dbg_bp_ctrl * bpctl, 
						   struct dbg_bp * bp)
{
	uint32_t id;

	if (bp->active)
		return 0;

	if (bpctl->cnt > ice->opt.bp_max) {
		struct dbg_bp * nxt;
		/* we are short of hardware breakpoints,
		   deactivate the least recently active */
		nxt = bpctl->lst[ice->opt.bp_max];
		if (nxt->active) {
			id = nxt->hw_id;
			nxt->active = 0;
			DCC_LOG1(LOG_INFO, "deactivating breakpoint %d", id);
			ice_bp_clr(ice, id);
		}
	} 

	DCC_LOG2(LOG_INFO, "activating breakpoint: 0x%08x(%d) ...", 
			bp->addr, bp->size);

	if (ice_bp_set(ice, bp->addr, bp->size, &id) < 0) {
		DCC_LOG(LOG_ERROR, "ice_bp_set() error!");
		bp->active = 0;
		bp->hw_id = -1;
		return -1;
	} 

	DCC_LOG1(LOG_INFO, "HW breakpoint id=%d", id);
	bp->active = 1;
	bp->hw_id = id;
	return 0;
}

static int dbg_bp_deactivate(ice_drv_t * ice, struct dbg_bp_ctrl * bpctl, 
						   struct dbg_bp * bp)
{
	uint32_t id;

	if (!bp->active)
		return 0;

	id = bp->hw_id;
	bp->active = 0;
	DCC_LOG1(LOG_INFO, "deactivating breakpoint %d", id);
	ice_bp_clr(ice, id);

	return 0;
}

static int dbg_bp_activate_all(ice_drv_t * ice, struct dbg_bp_ctrl * bpctl)
{
	struct dbg_bp * bp = NULL;
	uint32_t id;
	int ret = 0;
	int n;
	int i;

	n = (bpctl->cnt > ice->opt.bp_max) ? ice->opt.bp_max: bpctl->cnt;
	for (i = 0; i < n; i++) {
		bp = bpctl->lst[i];
		if (bp->enabled && !bp->active) {
			DCC_LOG(LOG_INFO, "activating breakpoint...");
			if ((ret = ice_bp_set(ice, bp->addr, bp->size, &id)) < 0) {
				DCC_LOG(LOG_ERROR, "ice_bp_set() error!");
				return ret;
			}
			bp->active = 1;
			bp->hw_id = id;
		}
	}

	return ret;
}

static int dbg_bp_deactivate_all(ice_drv_t * ice, struct dbg_bp_ctrl * bpctl)
{
	struct dbg_bp * bp = NULL;
	int ret = 0;
	int n;
	int i;

	n = (bpctl->cnt > ice->opt.bp_max) ? ice->opt.bp_max: bpctl->cnt;
	for (i = 0; i < n; i++) {
		bp = bpctl->lst[i];
		if (bp->active) {
			/* deactivate the breakpoints */
			DCC_LOG1(LOG_INFO, "deactivating breakpoint %d", i);
			if ((ret = ice_bp_clr(ice, bp->hw_id)) < 0) {
				DCC_LOG(LOG_ERROR, "ice_bp_clr() error!");
				return ret;
			}
			bp->active = 0;
			bp->hw_id = -1;
		}
	}

	return ret;
}

static struct dbg_bp * dbg_bp_get_next(struct dbg_bp_ctrl * bpctl, 
									   struct dbg_bp * bp)
{
	int i;

	if (bpctl->cnt == 0)
		return NULL;

	/* look up for this breakpoint in the list */
	for (i = 0; i < bpctl->cnt; i++) {
		if (bp == bpctl->lst[i]) {
			/* the breakpoint is in the list, return the next item */
			i++;
			if (i < bpctl->cnt)
				return bpctl->lst[i];
			return NULL;
		}
	}

	return bpctl->lst[0];
}

static struct dbg_bp * dbg_bp_lookup(struct dbg_bp_ctrl * bpctl, 
									 uint32_t addr, uint32_t size)
{
	struct dbg_bp * bp;
	int pos;

	/* check if a breakpoint with the same address and size 
	 already exist */
	for (pos = 0; pos < bpctl->cnt; pos++) {
		bp = bpctl->lst[pos];
		if ((bp->addr == addr) && (bp->size == size)) {
			return bp;
		}
	}

	return NULL;
}

/***********************************************************************
 Watchpoint management
 ***********************************************************************/

/* Watchpoint allocation data */
static struct dbg_wp dbg_wp_pool[DBG_WATCHPOINT_MAX];

/* Watchpoint management data */
/* This is a list of watchpoints ordered from the most to the least recently
   used.
   In most cases the target can support a limited number of active 
   watchpoints, in this case the top of this list will 
   correspond to the target's hardware ones.
 */
static struct dbg_wp * dbg_wp_list[DBG_WATCHPOINT_MAX];

static void dbg_wp_init(struct dbg_wp_ctrl * wpctl)
{
	struct dbg_wp * wp;
	int i;

	/* initialize the watchpoint poll.
	 The watchpoint poll consist of a linked list of free
	 watchpoint structures */
	wpctl->free = dbg_wp_pool;
	wpctl->lst = dbg_wp_list;
	wpctl->cnt = 0;

	wp = wpctl->free;
	for (i = 1; i < DBG_WATCHPOINT_MAX; i++) {
		wp->next = &dbg_wp_pool[i];
		wp = &dbg_wp_pool[i];
	}
	wp->next = NULL;

}

static struct dbg_wp * dbg_wp_new(struct dbg_wp_ctrl * wpctl, 
								  uint32_t addr, uint32_t size)
{
	struct dbg_wp * wp;

	if ((wp = wpctl->free) != NULL)
		wpctl->free = wp->next;

	wp->addr = addr;
	wp->size = size;
	wp->active = 0;

	/* insert at the list's tail,
	   if the list is full drop the last entry */

	if (wpctl->cnt < DBG_WATCHPOINT_MAX)
		wpctl->cnt++;

	wpctl->lst[wpctl->cnt - 1] = wp;
	
	DCC_LOG1(LOG_INFO, "cnt=%d", wpctl->cnt);

	return wp;
}

static void dbg_wp_move_to_head(struct dbg_wp_ctrl * wpctl, struct dbg_wp * wp)
{
	struct dbg_wp * tmp = NULL;
	struct dbg_wp * prev = wp;
	int i;

	DCC_LOG1(LOG_INFO, "wpctl->cnt=%d", wpctl->cnt);

	/* moving the list (shifting) */
	for (i = 0; i < wpctl->cnt; i++) {
		DCC_LOG1(LOG_INFO, "i=%d", i);
		tmp = wpctl->lst[i];
		wpctl->lst[i] = prev;
		if (tmp == wp) {
			break;
		}
		prev = tmp;
	}
}

static int dbg_wp_delete(ice_drv_t * ice, struct dbg_wp_ctrl * wpctl, 
						 struct dbg_wp * wp)
{
	int i;
	int n;

	/* look up for this watchpoint in the list */
	for (i = 0; i < wpctl->cnt; i++) {
		if (wp == wpctl->lst[i])
			break;
	}

	n = i;

	if (n == wpctl->cnt) {
		DCC_LOG(LOG_EXCEPT, "not in the list!!!");
		abort();
	}

	/* one less item in the list */
	wpctl->cnt--;

	/* moving the remaining items in the list one position to the front */
	for (i = n; i < wpctl->cnt; i++) {
		wpctl->lst[i] = wpctl->lst[i + 1];
	}

	DCC_LOG3(LOG_INFO, "n=%d cnt=%d ice->opt.wp_max=%d", 
			 n, wpctl->cnt, ice->opt.wp_max);

	if (wp->active) {
		/* clear the current watchpoint */
		DCC_LOG1(LOG_INFO, "clearing active BP: %d", wp->hw_id);
		ice_wp_clr(ice, wp->hw_id);

		/* activate the next if not disabled */
		if (wpctl->cnt >= ice->opt.wp_max) {
			struct dbg_wp * nxt;
			uint32_t id;

			nxt = wpctl->lst[ice->opt.wp_max - 1];
			if (nxt->enabled) {
				DCC_LOG(LOG_INFO, "seting next BP on list");
				if (ice_wp_set(ice, nxt->addr, nxt->size, &id) < 0) {
					DCC_LOG(LOG_EXCEPT, "ice_wp_set() error!");
				}
				nxt->active = 1;
				nxt->hw_id = id;
			}
		}
	} 

	/* release memory */
	wp->next = wpctl->free;
	wpctl->free = wp;

	return 0;
}

static int dbg_wp_enable_all(ice_drv_t * ice, struct dbg_wp_ctrl * wpctl)
{
	struct dbg_wp * wp = NULL;
	uint32_t id;
	int ret = 0;
	int n;
	int i;

	/* activate the watchpoints */
	n = (wpctl->cnt > ice->opt.wp_max) ? ice->opt.wp_max: wpctl->cnt;
	for (i = 0; i < n; i++) {
		wp = wpctl->lst[i];
		if (!wp->active) {
			DCC_LOG(LOG_INFO, "activating watchpoint...");
			if ((ret = ice_wp_set(ice, wp->addr, wp->size, &id)) < 0) {
				DCC_LOG(LOG_ERROR, "ice_wp_set() error!");
				return ret;
			}
			wp->active = 1;
			wp->hw_id = id;
		}
		wp->enabled = 1;
	}
	/* enable the remaining */
	for (i = n; i < wpctl->cnt; i++) {
		wp = wpctl->lst[i];
		wp->enabled = 1;
	}

	return ret;
}

static int dbg_wp_disable_all(ice_drv_t * ice, struct dbg_wp_ctrl * wpctl)
{
	struct dbg_wp * wp = NULL;
	int ret = 0;
	int i;

	/* disable all the watchpoints */
	for (i = 0; i < wpctl->cnt; i++) {
		wp = wpctl->lst[i];
		if (wp->active) {
			/* deactivate the watchpoints */
			DCC_LOG1(LOG_INFO, "deactivating watchpoint %d", i);
			if ((ret = ice_wp_clr(ice, wp->hw_id)) < 0) {
				DCC_LOG(LOG_ERROR, "ice_wp_clr() error!");
				return ret;
			}
			wp->active = 0;
			wp->hw_id = -1;
		}
		wp->enabled = 0;
	}

	return ret;
}

static int dbg_wp_activate(ice_drv_t * ice, struct dbg_wp_ctrl * wpctl, 
						   struct dbg_wp * wp)
{
	uint32_t id;

	if (wp->active)
		return 0;

	if (wpctl->cnt > ice->opt.wp_max) {
		struct dbg_wp * nxt;
		/* we are short of hardware watchpoints,
		   deactivate the least recently active */
		nxt = wpctl->lst[ice->opt.wp_max];
		if (nxt->active) {
			id = nxt->hw_id;
			nxt->active = 0;
			DCC_LOG1(LOG_INFO, "deactivating watchpoint %d", id);
			ice_wp_clr(ice, id);
		}
	} 

	DCC_LOG2(LOG_INFO, "activating watchpoint: 0x%08x(%d) ...", 
			wp->addr, wp->size);

	if (ice_wp_set(ice, wp->addr, wp->size, &id) < 0) {
		DCC_LOG(LOG_ERROR, "ice_wp_set() error!");
		wp->active = 0;
		wp->hw_id = -1;
		return -1;
	} 

	DCC_LOG1(LOG_INFO, "HW watchpoint id=%d", id);
	wp->active = 1;
	wp->hw_id = id;
	return 0;
}

static int dbg_wp_deactivate(ice_drv_t * ice, struct dbg_wp_ctrl * wpctl, 
						   struct dbg_wp * wp)
{
	uint32_t id;

	if (!wp->active)
		return 0;

	id = wp->hw_id;
	wp->active = 0;
	DCC_LOG1(LOG_INFO, "deactivating watchpoint %d", id);
	ice_wp_clr(ice, id);

	return 0;
}

static int dbg_wp_activate_all(ice_drv_t * ice, struct dbg_wp_ctrl * wpctl)
{
	struct dbg_wp * wp = NULL;
	uint32_t id;
	int ret = 0;
	int n;
	int i;

	n = (wpctl->cnt > ice->opt.wp_max) ? ice->opt.wp_max: wpctl->cnt;
	for (i = 0; i < n; i++) {
		wp = wpctl->lst[i];
		if (wp->enabled && !wp->active) {
			DCC_LOG(LOG_INFO, "activating watchpoint...");
			if ((ret = ice_wp_set(ice, wp->addr, wp->size, &id)) < 0) {
				DCC_LOG(LOG_ERROR, "ice_wp_set() error!");
				return ret;
			}
			wp->active = 1;
			wp->hw_id = id;
		}
	}

	return ret;
}

static int dbg_wp_deactivate_all(ice_drv_t * ice, struct dbg_wp_ctrl * wpctl)
{
	struct dbg_wp * wp = NULL;
	int ret = 0;
	int n;
	int i;

	n = (wpctl->cnt > ice->opt.wp_max) ? ice->opt.wp_max: wpctl->cnt;
	for (i = 0; i < n; i++) {
		wp = wpctl->lst[i];
		if (wp->active) {
			/* deactivate the watchpoints */
			DCC_LOG1(LOG_INFO, "deactivating watchpoint %d", i);
			if ((ret = ice_wp_clr(ice, wp->hw_id)) < 0) {
				DCC_LOG(LOG_ERROR, "ice_wp_clr() error!");
				return ret;
			}
			wp->active = 0;
			wp->hw_id = -1;
		}
	}

	return ret;
}

static struct dbg_wp * dbg_wp_get_next(struct dbg_wp_ctrl * wpctl, 
									   struct dbg_wp * wp)
{
	int i;

	if (wpctl->cnt == 0)
		return NULL;

	/* look up for this watchpoint in the list */
	for (i = 0; i < wpctl->cnt; i++) {
		if (wp == wpctl->lst[i]) {
			/* the watchpoint is in the list, return the next item */
			i++;
			if (i < wpctl->cnt)
				return wpctl->lst[i];
			return NULL;
		}
	}

	return wpctl->lst[0];
}

static struct dbg_wp * dbg_wp_lookup(struct dbg_wp_ctrl * wpctl, 
									 uint32_t addr, uint32_t size)
{
	struct dbg_wp * wp;
	int pos;

	/* check if a watchpoint with the same address and size 
	 already exist */
	for (pos = 0; pos < wpctl->cnt; pos++) {
		wp = wpctl->lst[pos];
		if ((wp->addr == addr) && (wp->size == size)) {
			return wp;
		}
	}

	return NULL;
}

/***********************************************************************
 Target Debug API
 ***********************************************************************/

/***********************************************************************
 Target Reset methods
 ***********************************************************************/

static int soft_reset(FILE * f, const ice_drv_t * ice,
					  const target_info_t * target,
					  const ice_mem_entry_t * mem)
{
	target_script_t script;
	int ret;

	if ((script = (target_script_t)target->reset_script) == NULL)
		return -1;

	if ((ret = ice_mem_lock(ice)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->mem_lock() fail");
		return ret;
	}

	if ((ret = script(f, ice, mem)) < 0) {
		/* XXX: script error ?? */
		DCC_LOG(LOG_WARNING, "script() fail!");
	} else { 
		ice_mem_unlock(ice);
	}

	return ret;
}

static int hw_reset(const ice_drv_t * ice, const target_info_t * target)
{
	DCC_LOG(LOG_TRACE, "reset...");
	int ms = 100;

	/* FIXME: trst is TCK synchronous ... */
	if (target->has_trst == YES)
		jtag_trst(true);

	if (target->has_nrst == YES)
		jtag_nrst_pulse(ms);
	else
		thinkos_sleep(ms);

	if (target->has_trst == YES)
		jtag_trst(false);

	jtag_tap_reset();

	return OK;
}

static int dbg_reset(const ice_drv_t * ice, const target_info_t * target)
{
	return OK;
}


/***********************************************************************
 Status, Profiling and console Polling
 ***********************************************************************/

static int dbg_poll_task(struct debugger * dbg, int id)
{
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ice_st;

	DCC_LOG1(LOG_INFO, "thread start: <%d>...", id);

	thinkos_mutex_lock(dbg->ice_mutex);
	for (;;) {
		while (!dbg->poll_enabled) {
			/* synchronize */
			DCC_LOG(LOG_TRACE, "cond wait .........");
			thinkos_cond_wait(dbg->poll_cond, dbg->ice_mutex);
		}
		INF("ICE poll start!");

		ice_st = ice_poll(ice, &dbg->comm);

		if (!dbg->poll_enabled) {
			INF("ICE poll stop!");
		} else
			dbg->poll_enabled = false;

		if (ice_st & ICE_ST_HALT) {
			INF("ICE break!!!!");
			DCC_LOG(LOG_TRACE, "break!!!!");
			thinkos_cond_broadcast(dbg->halt_cond);
		}

		if (ice_st & ICE_ST_FAULT) {
			INF("ICE fault!!!!");
			DCC_LOG(LOG_WARNING, "fault!!!!");
		}
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	return 0;
}

static int poll_start(struct debugger * dbg)
{
#if (ENABLE_ICE_POLLING)	
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;

	if (dbg->state != DBG_ST_RUNNING)
		return OK;

	if (!dbg->cfg.enable_ice_polling)
		return OK;

	/* sync */
	if (!dbg->poll_enabled) {
		/* wakeup the poll thread */
		dbg->poll_enabled = true;
		/* send a signal to the ICE driver, requesting to start polling */
		ice_signal(ice, ICE_SIG_POLL_START);
		/* wakeup the poll thread */
		thinkos_cond_signal(dbg->poll_cond);
	}
#endif

	return OK;
}

static int poll_stop(struct debugger * dbg)
{
#if (ENABLE_ICE_POLLING)	
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;

	if (dbg->poll_enabled) {
		/* request stop polling */
		dbg->poll_enabled = false;
		/* send a signal to the ICE driver, requesting to
		 stop polling */
		ice_signal(ice, ICE_SIG_POLL_STOP);
	}
#endif
	return OK;
}


static int dbg_status(struct debugger * dbg)
{
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ice_st;
	int ret;

	DCC_LOG(LOG_MSG, "start.");

	if (dbg->state > DBG_ST_UNCONNECTED) {

		if ((ice_st = ice_status(ice)) < 0) {
			DCC_LOG(LOG_WARNING, "ice_status() failed!");
			WARN("ice_status() failed!");
			return ice_st;
		};

		if (ice_st & ICE_ST_HALT) {
			INF("ice_status() halted!");
			if (dbg->state != DBG_ST_HALTED) {
				DCC_LOG(LOG_MSG, "not halted, stop polling.");
				poll_stop(dbg);
				/* deactivate all low level breakpoints ... */
				if ((ret = dbg_bp_deactivate_all(ice, &dbg->bp_ctrl)) < 0) {
					DCC_LOG(LOG_WARNING, "dbg_bp_deactivate_all() failed!");
				}
				/* deactivate all low level watchpoints ... */
				if ((ret = dbg_wp_deactivate_all(ice, &dbg->wp_ctrl)) < 0) {
					DCC_LOG(LOG_WARNING, "dbg_wp_deactivate_all() failed!");
				}
				DCC_LOG(LOG_MSG, "[DBG_ST_HALTED]");
				dbg->state = DBG_ST_HALTED;
			} else {
				DCC_LOG(LOG_MSG, "already halted do nothing ...");
			}
		} else {
			INF("ice_status() running!");
			if (dbg->state != DBG_ST_RUNNING) {
				DCC_LOG(LOG_MSG, "[DBG_ST_RUNNING], start polling...");
				dbg->state = DBG_ST_RUNNING;
				poll_start(dbg);
			} else {
				DCC_LOG(LOG_MSG, "already running do nothing ...");
			}
		}

	} else {
		WARN("unconnected!");
		DCC_LOG(LOG_MSG, "unconnected!");
	}

	DCC_LOG(LOG_MSG, "done.");

	return dbg->state;
}

/* */

int target_status(void)
{
	struct debugger * dbg = &debugger;
	int status;


	DCC_LOG1(LOG_MSG, "try_lock(%d)", dbg->target_mutex);

	if (thinkos_mutex_trylock(dbg->target_mutex) < 0) {
		DCC_LOG(LOG_TRACE, "thinkos_mutex_trylock() failed!");
		return DBG_ST_BUSY;
	}

	status = dbg_status(dbg);

	DCC_LOG(LOG_JABBER, ".");

	thinkos_mutex_unlock(dbg->target_mutex);

	return status;
}

int target_halt_wait(int tmo)
{
	struct debugger * dbg = &debugger;
	int status;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	/* halt wait */
	while (dbg->poll_enabled) {
		DCC_LOG(LOG_TRACE, "poll enabled waiting...");
		if (thinkos_cond_timedwait(dbg->halt_cond, dbg->target_mutex, 
								   tmo) < 0) {
			thinkos_mutex_unlock(dbg->target_mutex);
			return ERR_TIMEOUT;
		}
	}

	status = dbg_status(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return status;
}

int target_connect(int force)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	target_cpu_t * cpu = (target_cpu_t *)dbg->target->arch->cpu;
	int ret;

	DCC_LOG1(LOG_TRACE, "target=0x%p", dbg->target);

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_UNCONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	if ((dbg->state == DBG_ST_RUNNING) || (dbg->state == DBG_ST_HALTED)) {
		DCC_LOG(LOG_TRACE, "already connected");
		if (!force) {
			thinkos_mutex_unlock(dbg->target_mutex);
			return OK;
		}
	}

	thinkos_mutex_lock(dbg->ice_mutex);

	if (dbg->target->clk_slow_on_connect) {
		DCC_LOG(LOG_TRACE, "set JTAG clock to slow");
		jtag_tck_freq_set(dbg->target->jtag_clk_slow);
	}

	if ((ret = ice_connect(ice, cpu->idmask, cpu->idcomp)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->connect() failed!");
	} else { 
		dbg->state = DBG_ST_CONNECTED;
		DCC_LOG(LOG_TRACE, "[DBG_ST_CONNECTED]");
		if ((ret = dbg_bp_activate_all(ice, &dbg->bp_ctrl)) < 0) {
			DCC_LOG(LOG_WARNING, "dbg_bp_activate_all() failed!");
		} else if ((ret = dbg_wp_activate_all(ice, &dbg->wp_ctrl)) < 0) {
			DCC_LOG(LOG_WARNING, "dbg_wp_activate_all() failed!");
		} else {
			DCC_LOG(LOG_TRACE, "done.");
		}
	}

	thinkos_mutex_unlock(dbg->ice_mutex);

	if (ret >= 0)
		dbg_status(dbg);

	INF("target connected!");

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_release(void)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);

	if ((ret = ice_release(ice)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->release() fail");
		dbg->state = DBG_ST_OUTOFSYNC;
		DCC_LOG(LOG_TRACE, "[DBG_ST_OUTOFSYNC]");
	} else {
		dbg->state = DBG_ST_UNCONNECTED;
		DCC_LOG(LOG_TRACE, "[DBG_ST_UNCONNECTED]");
	}

	INF("target disconnected!");

	thinkos_mutex_unlock(dbg->ice_mutex);
	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

/* stop the target */
int target_halt(int method)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret = OK;

	DCC_LOG(LOG_TRACE, "-----------------------------------------"); 

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state != DBG_ST_RUNNING) {
		if (dbg->state != DBG_ST_HALTED) {
			DCC_LOG(LOG_WARNING, "invalid state"); 
			ret = ERR_STATE;
		}
		thinkos_mutex_unlock(dbg->target_mutex);
		return ret;
	}

	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	/* request the core to stop */
	ret = ice_halt_req(ice);
	thinkos_mutex_unlock(dbg->ice_mutex);
	
	/* request the core to stop */
	if (ret < 0) {
		DCC_LOG(LOG_WARNING, "drv->halt() fail!");
		dbg->state = DBG_ST_OUTOFSYNC;
		DCC_LOG(LOG_TRACE, "[DBG_ST_OUTOFSYNC]");
	} else {
		dbg_status(dbg);
	}

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_run(void)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	DCC_LOG(LOG_TRACE, "-----------------------------------------"); 

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		WARN("invalid state!"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	if (dbg->state == DBG_ST_RUNNING) {
		WARN("running!"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return OK;
	}

	thinkos_mutex_lock(dbg->ice_mutex);

	if ((ret = dbg_bp_activate_all(ice, &dbg->bp_ctrl)) < 0) {
		DCC_LOG(LOG_WARNING, "dbg_bp_activate_all() failed!");
	} else if ((ret = dbg_wp_activate_all(ice, &dbg->wp_ctrl)) < 0) {
		DCC_LOG(LOG_WARNING, "dbg_wp_activate_all() failed!");
	} else if ((ret = ice_run(ice)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->run() fail!");
		dbg->state = DBG_ST_OUTOFSYNC;
		DCC_LOG(LOG_TRACE, "[DBG_ST_OUTOFSYNC]");
	} else {
		/* set the state as CONNECTED. The core may stop
		   due to a breakpoint befor this functions exit. */
		dbg->state = DBG_ST_CONNECTED;
		INF("run: [CONNECTED]"); 
		DCC_LOG(LOG_TRACE, "[DBG_ST_CONNECTED]");
	}

	thinkos_mutex_unlock(dbg->ice_mutex);

	if (ret >= 0) {
		dbg_status(dbg);
	}

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_step(void)
{ 
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state != DBG_ST_HALTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_step(ice)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->step() fail!");
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_context_show(FILE * f)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	DCC_LOG(LOG_TRACE, "-----------------------------------------"); 

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state != DBG_ST_HALTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	thinkos_mutex_lock(dbg->ice_mutex);
	/* FIXME: read the context and then decode */
	if ((ret = ice_context_show(ice, f)) < 0) {
		DCC_LOG(LOG_WARNING, "ice_context_show() fail!");
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

/*-----------------------------------------------------------------------------
  Disassemble
  ---------------------------------------------------------------------------*/
/*
int target_insn_fetch(uint32_t addr, void * insn)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);
	if ((ret = ice_insn_fetch(ice, addr, insn)) < 0) {
		DCC_LOG(LOG_WARNING, "ice_insn_fetch() fail!");
	}
	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_insn_show(FILE * f, uint32_t addr, void * insn)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);
	if ((ret = ice_insn_show(ice, addr, insn, f)) < 0) {
		DCC_LOG(LOG_WARNING, "ice_show_insn() fail!");
	}
	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}
*/


static int read_memory(bfd_vma addr, uint8_t * buf, unsigned int len, 
				struct disassemble_info *dinfo)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	
	DCC_LOG2(LOG_TRACE, "addr=0x%08x len=%d", addr, len);

	if (len == 0)
		return 0;

	if (ice_mem_read(ice, dbg->mem, addr, buf, len) < 0) {
		DCC_LOG(LOG_WARNING, "dbg_mem_read(), fail");
		return -1;
	}

	DCC_LOG4(LOG_INFO, "%02x %02x %02x %02x", buf[0], buf[1], buf[2], buf[3]);

	return 0;
}

static void memory_error(int status, bfd_vma addr, 
						 struct disassemble_info *dinfo)
{
	dinfo->fprintf_func(dinfo->stream, "ERROR: Memory Read @ %08x\n", addr);
}

static void print_address(bfd_vma addr, struct disassemble_info *dinfo)
{
	dinfo->fprintf_func(dinfo->stream, "%08x", addr);
}

static int symbol_at_address(bfd_vma addr, struct disassemble_info *dinfo)
{
	return 0;
}

int target_print_insn(FILE * f, uint32_t addr)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	struct disassemble_info dinfo;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	/* stop polling */
	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_mem_lock(ice)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->mem_lock() fail");
		dbg->state = DBG_ST_OUTOFSYNC;
		DCC_LOG(LOG_TRACE, "[DBG_ST_OUTOFSYNC]");
		thinkos_mutex_unlock(dbg->target_mutex);
	} else {
		memset(&dinfo, 0, sizeof(struct disassemble_info));
		dinfo.fprintf_func = (fprintf_ftype)fprintf;
		dinfo.stream = f;
		dinfo.read_memory_func = read_memory;
		dinfo.memory_error_func = memory_error;
		dinfo.print_address_func = print_address;
		dinfo.symbol_at_address_func = symbol_at_address;
		ice_mem_unlock(ice);
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	if (ret >= 0) {
		fprintf(f, "%08x:  ", addr & ~1);
		ret = ice_print_insn(ice, addr, &dinfo);
		fprintf(f, "\n");
	}

	/* restart polling */
	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}


/*-----------------------------------------------------------------------------
  Registers
  ---------------------------------------------------------------------------*/

int target_register_get(int reg, uint32_t * val)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state != DBG_ST_HALTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_reg_get(ice, reg, val)) < 0) {
		DCC_LOG(LOG_WARNING, "ice_reg_get() fail!");
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_register_set(int reg, uint32_t val)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state != DBG_ST_HALTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_reg_set(ice, reg, val)) < 0) {
		DCC_LOG(LOG_WARNING, "ice_reg_set() fail!");
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}
/*
int target_pc_get(uint32_t * val)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);
	if ((ret = ice_pc_get(ice, val)) < 0) {
		DCC_LOG(LOG_WARNING, "ice_pc_get() fail!");
	}
	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_pc_set(uint32_t val)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);
	if ((ret = ice_pc_set(ice, val)) < 0) {
		DCC_LOG(LOG_WARNING, "ice_pc_set() fail!");
	}
	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_sp_get(uint32_t * val)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);
	if ((ret = ice_sp_get(ice, val)) < 0) {
		DCC_LOG(LOG_WARNING, "ice_sp_get() fail!");
	}
	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_sp_set(uint32_t val)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);
	if ((ret = ice_sp_set(ice, val)) < 0) {
		DCC_LOG(LOG_WARNING, "ice_sp_set() fail!");
	}
	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}
*/

int target_ifa_get(uint32_t * val)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);
	
	if (dbg->state != DBG_ST_HALTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_ifa_get(ice, val)) < 0) {
		DCC_LOG(LOG_WARNING, "ice_ifa_get() fail!");
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_ifa_set(uint32_t val)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state != DBG_ST_HALTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_ifa_set(ice, val)) < 0) {
		DCC_LOG(LOG_WARNING, "ice_ifa_set() fail!");
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_goto(uint32_t addr, int opt)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state != DBG_ST_HALTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_goto(ice, addr)) < 0) {
		DCC_LOG(LOG_WARNING, "ice_go_to() fail!");
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

/* read a memory region from target */
int target_mem_read(uint32_t addr, void * ptr, int len)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;
	
	DCC_LOG2(LOG_TRACE, "addr=0x%08x len=%d", addr, len);

	if (len == 0)
		return 0;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	/* stop polling */
	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_mem_lock(ice)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->mem_lock() fail");
	} else {
		if ((ret = ice_mem_read(ice, dbg->mem, addr, ptr, len)) < 0) {
			DCC_LOG(LOG_WARNING, "dbg_mem_read(), fail");
		}
		ice_mem_unlock(ice);
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

/*
 * write to the target memory.
 */
int target_mem_write(uint32_t addr, const void * ptr, int len)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	DCC_LOG2(LOG_TRACE, "addr=0x%08x len=%d", addr, len);

	if (len == 0)
		return 0;

	thinkos_mutex_lock(dbg->target_mutex);
	
	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	/* stop polling */
	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_mem_lock(ice)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->mem_lock() fail");
	} else {
		if ((ret = ice_mem_write(ice, dbg->mem, addr, ptr, len)) < 0)  {
			DCC_LOG(LOG_WARNING, "dbg_mem_write(), fail");
		}
		ice_mem_unlock(ice);
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);
	
	return ret;
}

/* erase a memory region in the target */
int target_mem_erase(uint32_t addr, int len)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	DCC_LOG2(LOG_INFO, "addr=0x%08x len=%d", addr, len);

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	/* stop polling */
	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_mem_lock(ice)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->mem_lock() fail");
	} else {
		if ((ret = ice_mem_erase(ice, dbg->mem, addr, len)) < 0) {
			DCC_LOG(LOG_WARNING, "dbg_mem_erase(), fail");
		}
		ice_mem_unlock(ice);
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);
	
	return ret;
}

/*****************************************************************************
 * NAND Memory 
 *****************************************************************************/

#if (ENABLE_NAND)

int target_nand_bb_check(uint32_t block)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	nand_dev_t * nand;
	int ret;
	
	DCC_LOG1(LOG_INFO, "block=%d", block);

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	/* stop polling */
	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_mem_lock(ice)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->mem_lock() fail");
	} else {
		nand = nand_dev_select(0);
		if ((ret = nand_bb_check(nand, block)) < 0) {
			DCC_LOG(LOG_WARNING, "nand_bb_check(), fail");
		}
		ice_mem_unlock(ice);
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_nand_block_erase(uint32_t block, bool force)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	nand_dev_t * nand;
	int ret;
	
	DCC_LOG1(LOG_INFO, "block=%d", block);

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	/* stop polling */
	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_mem_lock(ice)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->mem_lock() fail");
	} else {
		bool badblock = false;
		nand = nand_dev_select(0);

		if (!force) {
			if ((ret = nand_bb_check(nand, block)) < 0) {
				DCC_LOG(LOG_WARNING, "nand_bb_check(), fail");
				badblock = true;
			}
		}

		if (!badblock) {
			if ((ret = nand_block_erase(nand, block)) < 0) {
				DCC_LOG(LOG_WARNING, "nand_block_erase(), fail");
			}
		}

		ice_mem_unlock(ice);
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}


int target_nand_dev_get(int dev_id, nand_dev_t ** nandp)
{
	struct debugger * dbg = &debugger;

	if (nandp == NULL)
		return -1;
	
	thinkos_mutex_lock(dbg->target_mutex);

	*nandp = nand_dev_get(dev_id);

	thinkos_mutex_unlock(dbg->target_mutex);

	return 0;
}

int target_nand_chip_get(int dev_id, int chip_id, nand_chip_t ** chipp)
{
	struct debugger * dbg = &debugger;

	if (chipp == NULL)
		return -1;
	
	thinkos_mutex_lock(dbg->target_mutex);

	*chipp = nand_chip_get(dev_id, chip_id);

	thinkos_mutex_unlock(dbg->target_mutex);

	return 0;
}

#endif

/*****************************************************************************
 * Breakpoints and Watchpoins
 *****************************************************************************/

int target_breakpoint_get(struct dbg_bp * bp, struct dbg_bp ** next)
{
	struct debugger * dbg = &debugger;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	bp = dbg_bp_get_next(&dbg->bp_ctrl, bp);

	thinkos_mutex_unlock(dbg->target_mutex);

	if (bp == NULL) {
		DCC_LOG(LOG_MSG, "bp == NULL!");
		return -1;
	}

	if (next != NULL)
		*next = bp;

	return 0;
}

int target_breakpoint_set(uint32_t addr, uint32_t size)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	struct dbg_bp * bp = NULL;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	/* use default size if zero */
	size = (size == 0) ? dbg->ice.opt.bp_defsz : size;

	DCC_LOG2(LOG_INFO, "addr=0x%08x size=%d", addr, size);

	/* check if a breakpoint with the same address and size 
	 already exist */
	if ((bp = dbg_bp_lookup(&dbg->bp_ctrl, addr, size)) == NULL) {
		/* no breakpoint exist, create a new one */
		if ((bp = dbg_bp_new(&dbg->bp_ctrl, addr, size)) == NULL) {
			DCC_LOG(LOG_WARNING, "breakpoint allocation fail!");
			thinkos_mutex_unlock(dbg->target_mutex);
			return -1;
		};
		DCC_LOG1(LOG_INFO, "new breakpoint: %p", bp);
	}
	
	/* move the to the head */
	dbg_bp_move_to_head(&dbg->bp_ctrl, bp);

	/* enable the breakpoint */
	bp->enabled = 1;

	if (!bp->active) {
		poll_stop(dbg);
		thinkos_mutex_lock(dbg->ice_mutex);
		/* activate the hardware break point */
		dbg_bp_activate(ice, &dbg->bp_ctrl, bp);
		thinkos_mutex_unlock(dbg->ice_mutex);
		poll_start(dbg);
	} 

	thinkos_mutex_unlock(dbg->target_mutex);

	return 0;
}

int target_breakpoint_clear(uint32_t addr, uint32_t size)
{
	struct debugger * dbg = &debugger;
	struct dbg_bp * bp;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	size = (size == 0) ? dbg->ice.opt.bp_defsz : size;

	DCC_LOG2(LOG_INFO, "addr=0x%08x size=%d", addr, size);

	/* check if a breakpoint with this address and size exists */
	if ((bp = dbg_bp_lookup(&dbg->bp_ctrl, addr, size)) == NULL) {
		DCC_LOG(LOG_WARNING, "breakpoint not found!");
		ret = -1;
	} else {
		poll_stop(dbg);
		thinkos_mutex_lock(dbg->ice_mutex);
		ret = dbg_bp_delete(&dbg->ice, &dbg->bp_ctrl, bp);
		thinkos_mutex_unlock(dbg->ice_mutex);
		poll_start(dbg);
	}

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_breakpoint_enable(uint32_t addr, uint32_t size)
{
	struct debugger * dbg = &debugger;
	struct dbg_bp * bp;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	size = (size == 0) ? dbg->ice.opt.bp_defsz : size;

	DCC_LOG2(LOG_INFO, "addr=0x%08x size=%d", addr, size);

	/* check if a breakpoint with the same address and size 
	 already exist */
	if ((bp = dbg_bp_lookup(&dbg->bp_ctrl, addr, size)) == NULL) {
		DCC_LOG(LOG_WARNING, "breakpoint not found!");
		thinkos_mutex_unlock(dbg->target_mutex);
		return -1;
	} 
	
	/* enable the breakpoint */
	bp->enabled = 1;

	poll_stop(dbg);
	thinkos_mutex_lock(dbg->ice_mutex);
	/* activate the breakpoint */
	ret = dbg_bp_activate(&dbg->ice, &dbg->bp_ctrl, bp);
	thinkos_mutex_unlock(dbg->ice_mutex);
	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_breakpoint_disable(uint32_t addr, uint32_t size)
{
	struct debugger * dbg = &debugger;
	struct dbg_bp * bp;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	/* use default size if zero */
	size = (size == 0) ? dbg->ice.opt.bp_defsz : size;

	DCC_LOG2(LOG_INFO, "addr=0x%08x size=%d", addr, size);

	/* check if a breakpoint with this address and size exists */
	if ((bp = dbg_bp_lookup(&dbg->bp_ctrl, addr, size)) == NULL) {
		DCC_LOG(LOG_WARNING, "breakpoint not found!");
		thinkos_mutex_unlock(dbg->target_mutex);
		return -1;
	} 
	
	poll_stop(dbg);
	thinkos_mutex_lock(dbg->ice_mutex);
	/* deactivate the breakpoint */
	ret = dbg_bp_deactivate(&dbg->ice, &dbg->bp_ctrl, bp);
	thinkos_mutex_unlock(dbg->ice_mutex);
	poll_start(dbg);

	/* disable the breakpoint */
	bp->enabled = 0;

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}


int target_breakpoint_delete(struct dbg_bp * bp)
{
	struct debugger * dbg = &debugger;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);
	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	poll_stop(dbg);
	thinkos_mutex_lock(dbg->ice_mutex);
	ret = dbg_bp_delete(&dbg->ice, &dbg->bp_ctrl, bp);
	thinkos_mutex_unlock(dbg->ice_mutex);
	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_breakpoint_all_disable(void)
{
	struct debugger * dbg = &debugger;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);
	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	poll_stop(dbg);
	thinkos_mutex_lock(dbg->ice_mutex);
	ret = dbg_bp_disable_all(&dbg->ice, &dbg->bp_ctrl);
	thinkos_mutex_unlock(dbg->ice_mutex);
	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_breakpoint_all_enable(void)
{
	struct debugger * dbg = &debugger;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	poll_stop(dbg);
	thinkos_mutex_lock(dbg->ice_mutex);
	ret = dbg_bp_enable_all(&dbg->ice, &dbg->bp_ctrl);
	thinkos_mutex_unlock(dbg->ice_mutex);
	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

/*****************************************************************************
 * Watchpoins
 *****************************************************************************/

int target_watchpoint_get(struct dbg_wp * wp, struct dbg_wp ** next)
{
	struct debugger * dbg = &debugger;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	wp = dbg_wp_get_next(&dbg->wp_ctrl, wp);

	thinkos_mutex_unlock(dbg->target_mutex);

	if (wp == NULL) {
		DCC_LOG(LOG_MSG, "wp == NULL!");
		return -1;
	}

	if (next != NULL)
		*next = wp;

	return 0;
}

int target_watchpoint_set(uint32_t addr, uint32_t size)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	struct dbg_wp * wp = NULL;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	/* use default size if zero */
	size = (size == 0) ? dbg->ice.opt.wp_defsz : size;

	DCC_LOG2(LOG_INFO, "addr=0x%08x size=%d", addr, size);

	/* check if a watchpoint with the same address and size 
	 already exist */
	if ((wp = dbg_wp_lookup(&dbg->wp_ctrl, addr, size)) == NULL) {
		/* no watchpoint exist, create a new one */
		if ((wp = dbg_wp_new(&dbg->wp_ctrl, addr, size)) == NULL) {
			DCC_LOG(LOG_WARNING, "watchpoint allocation fail!");
			thinkos_mutex_unlock(dbg->target_mutex);
			return -1;
		};
		DCC_LOG1(LOG_INFO, "new watchpoint: %p", wp);
	}
	
	/* move the to the head */
	dbg_wp_move_to_head(&dbg->wp_ctrl, wp);

	/* enable the watchpoint */
	wp->enabled = 1;

	if (!wp->active) {
		poll_stop(dbg);
		thinkos_mutex_lock(dbg->ice_mutex);
		/* activate the hardware watch point */
		dbg_wp_activate(ice, &dbg->wp_ctrl, wp);
		thinkos_mutex_unlock(dbg->ice_mutex);
		poll_start(dbg);
	} 

	thinkos_mutex_unlock(dbg->target_mutex);

	return 0;
}

int target_watchpoint_clear(uint32_t addr, uint32_t size)
{
	struct debugger * dbg = &debugger;
	struct dbg_wp * wp;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	size = (size == 0) ? dbg->ice.opt.wp_defsz : size;

	DCC_LOG2(LOG_INFO, "addr=0x%08x size=%d", addr, size);

	/* check if a watchpoint with this address and size exists */
	if ((wp = dbg_wp_lookup(&dbg->wp_ctrl, addr, size)) == NULL) {
		DCC_LOG(LOG_WARNING, "watchpoint not found!");
		ret = -1;
	} else {
		poll_stop(dbg);
		thinkos_mutex_lock(dbg->ice_mutex);
		ret = dbg_wp_delete(&dbg->ice, &dbg->wp_ctrl, wp);
		thinkos_mutex_unlock(dbg->ice_mutex);
		poll_start(dbg);
	}

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_watchpoint_enable(uint32_t addr, uint32_t size)
{
	struct debugger * dbg = &debugger;
	struct dbg_wp * wp;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	size = (size == 0) ? dbg->ice.opt.wp_defsz : size;

	DCC_LOG2(LOG_INFO, "addr=0x%08x size=%d", addr, size);

	/* check if a watchpoint with the same address and size 
	 already exist */
	if ((wp = dbg_wp_lookup(&dbg->wp_ctrl, addr, size)) == NULL) {
		DCC_LOG(LOG_WARNING, "watchpoint not found!");
		thinkos_mutex_unlock(dbg->target_mutex);
		return -1;
	} 
	
	/* enable the watchpoint */
	wp->enabled = 1;

	poll_stop(dbg);
	thinkos_mutex_lock(dbg->ice_mutex);
	/* activate the watchpoint */
	ret = dbg_wp_activate(&dbg->ice, &dbg->wp_ctrl, wp);
	thinkos_mutex_unlock(dbg->ice_mutex);
	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_watchpoint_disable(uint32_t addr, uint32_t size)
{
	struct debugger * dbg = &debugger;
	struct dbg_wp * wp;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	/* use default size if zero */
	size = (size == 0) ? dbg->ice.opt.wp_defsz : size;

	DCC_LOG2(LOG_INFO, "addr=0x%08x size=%d", addr, size);

	/* check if a watchpoint with this address and size exists */
	if ((wp = dbg_wp_lookup(&dbg->wp_ctrl, addr, size)) == NULL) {
		DCC_LOG(LOG_WARNING, "watchpoint not found!");
		thinkos_mutex_unlock(dbg->target_mutex);
		return -1;
	} 
	
	poll_stop(dbg);
	thinkos_mutex_lock(dbg->ice_mutex);
	/* deactivate the watchpoint */
	ret = dbg_wp_deactivate(&dbg->ice, &dbg->wp_ctrl, wp);
	thinkos_mutex_unlock(dbg->ice_mutex);
	poll_start(dbg);

	/* disable the watchpoint */
	wp->enabled = 0;

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}


int target_watchpoint_delete(struct dbg_wp * wp)
{
	struct debugger * dbg = &debugger;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);
	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}


	poll_stop(dbg);
	thinkos_mutex_lock(dbg->ice_mutex);
	ret = dbg_wp_delete(&dbg->ice, &dbg->wp_ctrl, wp);
	thinkos_mutex_unlock(dbg->ice_mutex);
	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_watchpoint_all_disable(void)
{
	struct debugger * dbg = &debugger;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);
	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	poll_stop(dbg);
	thinkos_mutex_lock(dbg->ice_mutex);
	ret = dbg_wp_disable_all(&dbg->ice, &dbg->wp_ctrl);
	thinkos_mutex_unlock(dbg->ice_mutex);
	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_watchpoint_all_enable(void)
{
	struct debugger * dbg = &debugger;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	poll_stop(dbg);
	thinkos_mutex_lock(dbg->ice_mutex);
	ret = dbg_wp_enable_all(&dbg->ice, &dbg->wp_ctrl);
	thinkos_mutex_unlock(dbg->ice_mutex);
	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

/*****************************************************************************
 * Reset
 *****************************************************************************/

int target_reset(FILE * f, int mode)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	target_info_t * target = (target_info_t *)dbg->target; 
	int ret = 0;

	thinkos_mutex_lock(dbg->target_mutex);

	if (mode == RST_AUTO)
		mode = target->reset_mode;

	poll_stop(dbg);

	if (mode == RST_HARD)  {
		if (dbg->state >= DBG_ST_CONNECTED) {
			if ((ret = ice_release(ice)) < 0) {
				DCC_LOG(LOG_WARNING, "drv->release() fail");
				dbg->state = DBG_ST_OUTOFSYNC;
				DCC_LOG(LOG_TRACE, "[DBG_ST_OUTOFSYNC]");
			} else {
				dbg->state = DBG_ST_UNCONNECTED;
				DCC_LOG(LOG_TRACE, "[DBG_ST_UNCONNECTED]");
			}
		}
	} else if (mode == RST_SOFT) {
		if (dbg->state < DBG_ST_CONNECTED) {
			DCC_LOG(LOG_WARNING, "invalid state"); 
			thinkos_mutex_unlock(dbg->target_mutex);
			return ERR_STATE;
		}
	}

	thinkos_mutex_lock(dbg->ice_mutex);
	switch (mode) {
	case RST_SOFT:
		fprintf(f, " - software reset...\n");
		DCC_LOG(LOG_TRACE, "software reset...");
		if ((ret = soft_reset(f, ice, dbg->target, dbg->mem)) < 0) {
			DCC_LOG(LOG_WARNING, "software reset failed!");
			/* XXX: ERROR */
		}
		break;
	case RST_HARD:
		fprintf(f, " - hardware reset...\n");
		DCC_LOG(LOG_TRACE, "hardware reset...");
		if ((ret = hw_reset(ice, target)) < 0) {
			DCC_LOG(LOG_WARNING, "hardware reset failed!");
		}
		break;
	case RST_CORE:
		fprintf(f, " - core reset...\n");
		DCC_LOG(LOG_TRACE, "core reset...");
		if ((ret = ice_core_reset(ice)) < 0) {
			DCC_LOG(LOG_WARNING, "core reset failed!");
		}
		break;
	case RST_SYS:
		fprintf(f, " - system reset...\n");
		DCC_LOG(LOG_TRACE, "system reset...");
		if ((ret = ice_system_reset(ice)) < 0) {
			DCC_LOG(LOG_WARNING, "system reset failed!");
		}
		break;
	case RST_DBG:
		fprintf(f, " - debug reset...\n");
		DCC_LOG(LOG_TRACE, "debug reset...");
		if ((ret = dbg_reset(ice, target)) < 0) {
			DCC_LOG(LOG_WARNING, "debug reset failed!");
		}
		break;
	default:
		/* XXX: ERROR */
		DCC_LOG1(LOG_WARNING, "invalid mode: %d", mode);
		ret = ERR_PARM;
	}

	if (dbg->state > DBG_ST_CONNECTED) {
		if (ice_status(ice) & ICE_ST_HALT) {
			DCC_LOG(LOG_TRACE, "[DBG_ST_HALTED].");
			dbg->state = DBG_ST_HALTED;
		} else {
			DCC_LOG(LOG_TRACE, "[DBG_ST_RUNNING], start polling...");
			poll_start(dbg);
			dbg->state = DBG_ST_RUNNING;
		}
	}

	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_init(FILE * f)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	target_info_t * target = (target_info_t *)dbg->target; 
	target_script_t script;
	int ret = 0;

	DCC_LOG1(LOG_TRACE, "target=0x%p", target);

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);

	if (dbg->target->clk_slow_on_connect) {
		DCC_LOG(LOG_TRACE, "set JTAG clock to slow");
		jtag_tck_freq_set(dbg->target->jtag_clk_slow);
	}

	if ((script = (target_script_t)target->on_init) != NULL) {
		DCC_LOG1(LOG_TRACE, "on_init.script()=0x%p", script);
		if ((ret = ice_mem_lock(ice)) < 0) {
			DCC_LOG(LOG_WARNING, "drv->mem_lock() fail");
		} else {
			if ((ret = script(f, ice, dbg->mem)) < 0) {
				DCC_LOG(LOG_WARNING, "script() fail!");
			}
			ice_mem_unlock(ice);
		}
	}

	if ((ret >= 0) && (dbg->target->clk_def_on_init)) {
		DCC_LOG(LOG_TRACE, "set default JTAG clock");
		jtag_tck_freq_set(dbg->target->jtag_clk_def);
	}

	if (ice_status(ice) & ICE_ST_HALT) {
		dbg->state = DBG_ST_HALTED;
	} else {
		DCC_LOG(LOG_TRACE, "[DBG_ST_RUNNING], start polling...");
		poll_start(dbg);
		dbg->state = DBG_ST_RUNNING;
	}

	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_probe(FILE * f)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	target_script_t script;
	int ret = 0;

	DCC_LOG(LOG_INFO, ".");

	thinkos_mutex_lock(dbg->target_mutex);

	/* stop polling */
	poll_stop(dbg);

	if ((script = (target_script_t)dbg->target->probe) != NULL) {
		thinkos_mutex_lock(dbg->ice_mutex);
		DCC_LOG(LOG_INFO, "probe.script()");
		if ((ret = ice_mem_lock(ice)) < 0) {
			DCC_LOG(LOG_WARNING, "ice_mem_lock() fail");
		} else {
			if ((ret = script(f, ice, dbg->mem)) < 0) {
				DCC_LOG(LOG_WARNING, "script() fail!");
			}
			ice_mem_unlock(ice);
		}
		thinkos_mutex_unlock(dbg->ice_mutex);
	}

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_tap_trst(unsigned int mode)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	DCC_LOG(LOG_INFO, ".");

	thinkos_mutex_lock(dbg->target_mutex);

	/* release the target */
	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_release(ice)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->release() fail");
		dbg->state = DBG_ST_OUTOFSYNC;
		DCC_LOG(LOG_TRACE, "[DBG_ST_OUTOFSYNC]");
	} else {
		dbg->state = DBG_ST_UNCONNECTED;
		DCC_LOG(LOG_TRACE, "[DBG_ST_UNCONNECTED]");
	}

	switch (mode) {
	case TARGET_IO_CLR:
		ret = jtag_trst(true);
//		jtag_run_test(1, JTAG_TAP_IDLE);
		break;
	case TARGET_IO_SET:
		ret = jtag_trst(false);
//		jtag_run_test(1, JTAG_TAP_IDLE);
		break;
	default:
		if ((ret = jtag_trst(true)) == JTAG_OK) {
			jtag_run_test(1, JTAG_TAP_IDLE);
			ret = jtag_trst(false);
			jtag_tap_reset();
		}
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_nrst(unsigned int mode, unsigned int ms)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	DCC_LOG(LOG_INFO, ".");

	thinkos_mutex_lock(dbg->target_mutex);

	/* release the target */
	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((ret = ice_release(ice)) < 0) {
		DCC_LOG(LOG_WARNING, "drv->release() fail");
		dbg->state = DBG_ST_OUTOFSYNC;
		DCC_LOG(LOG_TRACE, "[DBG_ST_OUTOFSYNC]");
	} else {
		dbg->state = DBG_ST_UNCONNECTED;
		DCC_LOG(LOG_TRACE, "[DBG_ST_UNCONNECTED]");
	}

	switch (mode) {
	case 0:
		ret = jtag_nrst(0);
		break;
	case 1:
		ret = jtag_nrst(1);
		break;
	default:
		if ((ret = jtag_nrst(1)) == JTAG_OK) {
			thinkos_sleep(ms);
			ret = jtag_nrst(0);
		}
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_tap_reset(void)
{
	struct debugger * dbg = &debugger;

	DCC_LOG(LOG_INFO, ".");

	thinkos_mutex_lock(dbg->target_mutex);

	thinkos_mutex_lock(dbg->ice_mutex);
	jtag_tap_reset();
	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return 0;
}

int target_power_ctl(bool on)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret = 0;

	DCC_LOG(LOG_INFO, ".");

	thinkos_mutex_lock(dbg->target_mutex);

	if (on) {
		ext_pwr_on();
		dbg->ext_pwr = 1;
	} else {
		/* release the target */
		poll_stop(dbg);

		thinkos_mutex_lock(dbg->ice_mutex);
		if ((ret = ice_release(ice)) < 0) {
			DCC_LOG(LOG_WARNING, "drv->release() fail");
			dbg->state = DBG_ST_OUTOFSYNC;
			DCC_LOG(LOG_TRACE, "[DBG_ST_OUTOFSYNC]");
		} else {
			dbg->state = DBG_ST_UNCONNECTED;
			DCC_LOG(LOG_TRACE, "[DBG_ST_UNCONNECTED]");
		}

		/* force 0 on trst */
		jtag_trst(true);
		/* force 0 in TMS and TDI */
		jtag_run_test(1, JTAG_TAP_IDLE);
		thinkos_mutex_unlock(dbg->ice_mutex);

		ext_pwr_off();

		dbg->ext_pwr = 0;
	}	

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_power_stat(FILE * f)
{
	int ovr = ext_pwr_mon();

	if (ovr)
		fprintf(f, "Trbl\n");
	else
		fprintf(f, "%s\n", ext_pwr_stat() ? "On" : "Off");

	return 0;
}


int target_relay(bool on)
{
	struct debugger * dbg = &debugger;

	DCC_LOG(LOG_INFO, ".");

	thinkos_mutex_lock(dbg->target_mutex);

	if (on)
		relay_on();
	else
		relay_off();

	thinkos_mutex_unlock(dbg->target_mutex);

	return 0;
}


struct target_info * get_target_info(void)
{
	struct debugger * dbg = &debugger;

	return (struct target_info *)dbg->target;
}

uint32_t target_strtoaddr(const char * s)
{
	struct debugger * dbg = &debugger;
//	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	struct target_sym * sym;

	if (target_sym_lookup(dbg->target, s, &sym) == 0)
		return sym->addr;

	return strtoul(s, NULL, 0);
}

int target_send(int data)
{
	struct debugger * dbg = &debugger;
//	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;

	thinkos_mutex_lock(dbg->target_mutex);

	/* TODO: check the return ... */
//	arm7ice_dcc_write(data);

//	if (dbg->state == ST_RUNNING)
//		ice->poll(ice->arg);

	thinkos_mutex_unlock(dbg->target_mutex);
	return 0;
}

int mod_mem_register(const struct ice_mem_entry * mem);

union {
	ice_ctrl_t ctrl;
	armice_ctrl_t armice_ctrl;
	cm3ice_ctrl_t cm3ice_ctrl;
} dbg_ice_ctrl_buf;


int ice_drv_select(struct debugger * dbg, const ice_drv_info_t * info)
{
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (ice->info != info) {
		DCC_LOG(LOG_TRACE, "ICE driver change...");

		if (ice_close(ice) < 0) {
			/* closing the ICE controller driver */
			DCC_LOG(LOG_WARNING, "ICE controller close fail!");
		}

		dbg->state = DBG_ST_UNDEF;
		DCC_LOG(LOG_TRACE, "[DBG_ST_UNDEF]");

		memset(&dbg_ice_ctrl_buf, 0, sizeof(dbg_ice_ctrl_buf));

		/* load the ICE controller driver */
		if ((ret = ice_open(ice, info, &dbg_ice_ctrl_buf.ctrl)) < 0) {
			DCC_LOG(LOG_ERROR, "ICE controller open fail!");
			thinkos_mutex_unlock(dbg->target_mutex);
			return ret;
		}

		INF("- ICE driver: %s - %s - %s",
			   ice->info->name, ice->info->version, ice->info->vendor);
	} 

	thinkos_mutex_unlock(dbg->target_mutex);

	return 0;

}

int target_ice_configure(FILE * f, const struct target_info * target, 
						 int force)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	const ice_drv_info_t * info;
	uint8_t irlen[32];
	jtag_tap_t * tap;
	ice_val_t jtag_clk;
	jtag_idcode_t idcode;
	int tap_pos;
	uint8_t * irpath;
	unsigned int cnt;
	char s[16];

	int ret;
	int i;

	if (target == NULL) {
		DCC_LOG(LOG_ERROR, "NULL target!");
		return ERR_PARM;
	}

	if (dbg->state == DBG_ST_FAULT ) {
		DCC_LOG(LOG_ERROR, "Invalid state!");
		return ERR_STATE;
	}

	if ((info = target->ice_drv) == NULL) {
		DCC_LOG(LOG_ERROR, "NULL ice driver info!");
		return ERR_PARM;
	}

	if (!dbg->ext_pwr) { 
		ext_pwr_on();
		/* FIXME: configurable power on time */
		thinkos_sleep(200);
		dbg->ext_pwr = 1;
	}

	thinkos_mutex_lock(dbg->target_mutex);

	if ((target == dbg->target) && (!force)) {
		DCC_LOG1(LOG_TRACE, "Keeping target: '%s'", target->name);
		thinkos_mutex_unlock(dbg->target_mutex);
		return 0;
	}

	DCC_LOG1(LOG_TRACE, "Changing target: '%s'", target->name);

	if (dbg->mem_mod_id != -1) {
		DCC_LOG(LOG_TRACE, "Memory module unregister...");
		module_unregister(dbg->mem_mod_id); 
		dbg->mem_mod_id = -1;
	}

	/* release the target */
	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	ice_release(ice);

	dbg->target = &target_null;
	dbg->mem = target_null.mem;

	if (ice->info != info) {
		DCC_LOG(LOG_TRACE, "ICE driver change...");

		if (ice_close(ice) < 0) {
			/* closing the ICE controller driver */
			DCC_LOG(LOG_WARNING, "ICE controller close fail!");
		}

		dbg->state = DBG_ST_UNDEF;
		DCC_LOG(LOG_TRACE, "[DBG_ST_UNDEF]");

		memset(&dbg_ice_ctrl_buf, 0, sizeof(dbg_ice_ctrl_buf));

		/* load the ICE controller driver */
		if ((ret = ice_open(ice, info, &dbg_ice_ctrl_buf.ctrl)) < 0) {
			DCC_LOG(LOG_ERROR, "ICE controller open fail!");
			thinkos_mutex_unlock(dbg->ice_mutex);
			thinkos_mutex_unlock(dbg->target_mutex);
			return ret;
		}

		fprintf(f, " - ICE driver: %s - %s - %s\n",
				ice->info->name, ice->info->version, ice->info->vendor);
		INF("- ICE driver: %s - %s - %s",
			   ice->info->name, ice->info->version, ice->info->vendor);
	} 

	/* cache of frequently accessed structures ... */
	dbg->target = target;
	dbg->mem = target->mem;
	dbg->tcp_port = 9;

	/* TODO: modular link layer  to support other
	   access ports than JTAG... */

	if (target->clk_slow_on_connect)
		jtag_clk = target->jtag_clk_slow;
	else
		jtag_clk = target->jtag_clk_def;

	fprintf(f, " - Slow clock: %s.\n", fmt_freq(s, target->jtag_clk_slow));
	fprintf(f, " - Fast clock: %s.\n", fmt_freq(s, target->jtag_clk_def));

	jtag_rtck_freq_set(target->jtag_clk_slow);

	if (target->has_rtck) {
		jtag_tck_freq_set(target->jtag_clk_max);
		/* The preferred clock method is adaptive (RTCK) */
		if (target->prefer_rtck) {
			fprintf(f, " - RTCK enabled.\n");
			DCC_LOG(LOG_TRACE, "enabling RTCK.");
			jtag_rtck_enable();
		} else {
			jtag_rtck_disable();
		}
	} else {
		/* adjust the JTAG TCK frequency */
		if ((ret = jtag_tck_freq_set(jtag_clk)) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_clk_set()!");
			thinkos_mutex_unlock(dbg->ice_mutex);
			thinkos_mutex_unlock(dbg->target_mutex);
			return ret;
		}

		jtag_rtck_disable();
	}

	if (target->reset_on_config) {

#if 0
		switch (target->reset_mode) {
		case RST_HARD:
			fprintf(f, " - Hardware reset on config...\n");
			DCC_LOG(LOG_TRACE, "hardware reset...");
			if ((ret = hw_reset(ice, target)) < 0) {
				DCC_LOG(LOG_WARNING, "hardware reset failed!");
			}
			break;
		case RST_CORE:
			fprintf(f, " - Core reset on config...\n");
			DCC_LOG(LOG_TRACE, "core reset...");
			if ((ret = core_reset(ice, target)) < 0) {
				DCC_LOG(LOG_WARNING, "core reset failed!");
			}
			break;
		case RST_SOFT:
		case RST_DBG:
			fprintf(f, " - Debug reset on config...\n");
			DCC_LOG(LOG_TRACE, "debug reset...");
			if ((ret = dbg_reset(ice, target)) < 0) {
				DCC_LOG(LOG_WARNING, "debug reset failed!");
			}
			break;
		}
#endif
		fprintf(f, " - Hardware reset on config...\n");
		DCC_LOG(LOG_TRACE, "hardware reset...");
		if ((ret = hw_reset(ice, target)) < 0) {
			DCC_LOG(LOG_WARNING, "hardware reset failed!");
		}
	}

	/* configure the scan path */
	if (target->jtag_probe) {

		fprintf(f, " - JTAG probe...\n");

#if 0
		DCC_LOG(LOG_MSG, "TAP reset...");
		/* assert the JTAG TRST signal (low) */
		jtag_trst(true);
		jtag_run_test(1, JTAG_TAP_IDLE);
		/* deasser the JTAG TRST signal (high) */
		jtag_trst(false);

		jtag_tap_reset();
#endif
		DCC_LOG(LOG_TRACE, "Dynamic JTAC config ...");

		/* dynamic configuration */
		if (jtag_chain_probe(irlen, 32, &cnt) != JTAG_OK) {
			if (target->arch->cpu->irlength == 0) {
				DCC_LOG(LOG_ERROR, "IR length !");
				thinkos_mutex_unlock(dbg->ice_mutex);
				thinkos_mutex_unlock(dbg->target_mutex);
				return -1;
			} 
			irlen[0] = target->arch->cpu->irlength;
			cnt = 1;
		}
		irpath = irlen;
		(void)irpath;
		tap_pos = -1;
	} else {
		/* TODO: preconfigured scan chain */
		fprintf(f, " #NOTICE: target->jtag_probe flag not set!!!.\n");
		cnt = 0;
	}

	DCC_LOG1(LOG_TRACE, "TAPS: %d", cnt);

	if (cnt == 0) {
		DCC_LOG(LOG_WARNING, "No TAPs defined!");
		thinkos_mutex_unlock(dbg->ice_mutex);
		thinkos_mutex_unlock(dbg->target_mutex);
		return 0;
	}

	/* reset the TAPs to put the IDCODE in the DR scan */
	jtag_tap_reset();

	/* initializing the jtag chain */
	if ((ret = jtag_chain_init(irlen, cnt)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "JTAG chain fail!");
		thinkos_mutex_unlock(dbg->ice_mutex);
		thinkos_mutex_unlock(dbg->target_mutex);
		return ret;
	}

	if (target->pre_config) {
		DCC_LOG(LOG_TRACE, "Target pre config callback...");
		if ((ret = target->pre_config(f, ice, target)) < 0) {
			DCC_LOG(LOG_ERROR, "target->pre_config() fail!");
			thinkos_mutex_unlock(dbg->ice_mutex);
			thinkos_mutex_unlock(dbg->target_mutex);
			return ret;
		}
		cnt = jtag_tap_tell();
	} else {
		DCC_LOG(LOG_TRACE, "target->pre_config callback undefined!");
	}

	if (tap_pos < 0) {
		DCC_LOG(LOG_TRACE, "Detecting the TAP position...");

		for (i = 0; i < cnt; i++) {
			if ((ret = jtag_tap_get(&tap, i)) != JTAG_OK) {
				DCC_LOG(LOG_ERROR, "jtag_tap_get()!");
				thinkos_mutex_unlock(dbg->ice_mutex);
				thinkos_mutex_unlock(dbg->target_mutex);
				return ret;
			}

			if ((ret = jtag_tap_idcode(tap, &idcode)) != JTAG_OK) {
				DCC_LOG(LOG_ERROR, "jtag_tap_idcode()!");
				thinkos_mutex_unlock(dbg->ice_mutex);
				thinkos_mutex_unlock(dbg->target_mutex);
				return ret;
			}

			if ((idcode & target->arch->cpu->idmask) == 
				target->arch->cpu->idcomp) {
				DCC_LOG1(LOG_TRACE, "match, idcode:%08x", idcode);

				fprintf(f, " - JTAG IDCODE: 0x%08x\n", idcode); 
				tap_pos = i;
				break;
			}
		}

		if (tap_pos < 0) {
			DCC_LOG(LOG_WARNING, "no suitable CPU found()!");
			thinkos_mutex_unlock(dbg->ice_mutex);
			thinkos_mutex_unlock(dbg->target_mutex);
			return -1;
		}
	} else {
		if (tap_pos > cnt) {
			DCC_LOG1(LOG_ERROR, "TAP position (%d) is out of bounds!", tap_pos);
			thinkos_mutex_unlock(dbg->ice_mutex);
			thinkos_mutex_unlock(dbg->target_mutex);
			/* XXX: this is a JTAG error and shuld not be used in
			   a high level function... */
			return JTAG_ERR_INVALID_TAP;
		}

		if ((ret = jtag_tap_get(&tap, tap_pos)) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_tap_get()!");
			thinkos_mutex_unlock(dbg->ice_mutex);
			thinkos_mutex_unlock(dbg->target_mutex);
			return ret;
		}
		if ((ret = jtag_tap_idcode(tap, &idcode)) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_tap_idcode()!");
			thinkos_mutex_unlock(dbg->ice_mutex);
			thinkos_mutex_unlock(dbg->target_mutex);
			return ret;
		}

		DCC_LOG1(LOG_TRACE, "IDCODE:%08x", idcode);
		fprintf(f, " - JTAG IDCODE: 0x%08x\n", idcode); 

		if ((idcode & target->arch->cpu->idmask) != target->arch->cpu->idcomp) {
			DCC_LOG(LOG_TRACE, "invalid IDCODE");
			thinkos_mutex_unlock(dbg->ice_mutex);
			thinkos_mutex_unlock(dbg->target_mutex);
			return -1;
		}

		DCC_LOG1(LOG_TRACE, "match, idcode:%08x", idcode);
	}

	tap->idmask = target->arch->cpu->idmask;
	tap->idcomp = target->arch->cpu->idcomp;

	memset(&ice->opt, 0, sizeof(ice_opt_t));

	if (ice_configure(ice, tap, &ice->opt, target->ice_cfg) < 0) {
		DCC_LOG(LOG_ERROR, "ICE controller configurarion fail!");
		thinkos_mutex_unlock(dbg->ice_mutex);
		thinkos_mutex_unlock(dbg->target_mutex);
		return -1;
	}

	thinkos_mutex_unlock(dbg->ice_mutex);

	DCC_LOG(LOG_TRACE, "Memory module register...");
	dbg->mem_mod_id = mod_mem_register(target->mem);

#if 0
	for (i = 0; i < DBG_BREAKPOINT_MAX; i++) {
		dbg->bp[i].addr = 0;
		dbg->bp[i].size = 0;
	}
	for (i = 0; i < DBG_WATCHPOINT_MAX; i++) {
		dbg->wp[i].addr = 0;
		dbg->wp[i].mask = 0xffffffff;
	}
	dbg->bp_cnt = 0;
#endif
	dbg->dasm.base = dbg->target->start_addr;
	dbg->transf.base = dbg->target->start_addr;

	dbg->state = DBG_ST_UNCONNECTED;
	DCC_LOG(LOG_TRACE, "[DBG_ST_UNCONNECTED]");

	thinkos_mutex_unlock(dbg->target_mutex);

	return 0;
}

int target_config(FILE * f)
{
	struct debugger * dbg = &debugger;
	const struct target_info * target;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if ((target = dbg->target) == NULL) {
		DCC_LOG(LOG_ERROR, "NULL target!");
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_NULL_TARGET;
	}

	if (dbg->state < DBG_ST_UNCONNECTED) {
		DCC_LOG(LOG_ERROR, "Invalid state!");
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	if (dbg->state >= DBG_ST_CONNECTED) {
		poll_stop(dbg);
		thinkos_mutex_lock(dbg->ice_mutex);

		if ((ret = ice_release(ice)) < 0) {
			DCC_LOG(LOG_WARNING, "drv->release() failed! [DBG_ST_OUTOFSYNC]");
			dbg->state = DBG_ST_OUTOFSYNC;
			thinkos_mutex_unlock(dbg->target_mutex);
			thinkos_mutex_unlock(dbg->ice_mutex);
			return ERR_OUTOFSYNC;
		} 
		
		dbg->state = DBG_ST_UNCONNECTED;
		DCC_LOG(LOG_TRACE, "[DBG_ST_UNCONNECTED]");
	} else
		thinkos_mutex_lock(dbg->ice_mutex);


	/* Execute the target specific configuration */
	if (target->pos_config) {
		DCC_LOG(LOG_TRACE, "Target pos config callback...");
		if ((ret = target->pos_config(f, ice, target)) < 0) {
			DCC_LOG(LOG_ERROR, "target->pos_config() fail!");
		}
	} else {
		DCC_LOG(LOG_TRACE, "target->pos_config callback undefined!");
		ret = 0;
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_ice_test(FILE * f, uint32_t req, uint32_t argc, uint32_t argv[])
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret = 0;

	DCC_LOG(LOG_TRACE, ".");

	thinkos_mutex_lock(dbg->target_mutex);

	/* stop polling */
	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	ret = ice_test(ice, f, req, argc, argv);
	thinkos_mutex_unlock(dbg->ice_mutex);

	if (dbg->state > DBG_ST_CONNECTED) {
		/* force a status update */
		dbg->state = DBG_ST_CONNECTED;
		DCC_LOG(LOG_TRACE, "[DBG_ST_CONNECTE].");
	}

	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_ice_info(FILE * f, uint32_t which)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret = 0;

	DCC_LOG(LOG_TRACE, ".");

	thinkos_mutex_lock(dbg->target_mutex);

	/* stop polling */
	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	ret = ice_info(ice, f, which);
	thinkos_mutex_unlock(dbg->ice_mutex);

	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_test(FILE * f, uint32_t val)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	target_info_t * target = (target_info_t *)dbg->target; 
	target_test_t test;
	int ret = 0;

	DCC_LOG(LOG_TRACE, ".");

	thinkos_mutex_lock(dbg->target_mutex);

	/* stop polling */
	poll_stop(dbg);

	thinkos_mutex_lock(dbg->ice_mutex);
	if ((test = target->test) != NULL) {
		DCC_LOG1(LOG_TRACE, "target.test(): %08x", test);
		if ((ret = ice_mem_lock(ice)) < 0) {
			DCC_LOG(LOG_WARNING, "ice_mem_lock() fail");
		} else {
			if ((ret = test(f, ice, dbg->mem, val)) < 0) {
				DCC_LOG(LOG_WARNING, "target.test() fail!");
			}
			ice_mem_unlock(ice);
		}
	}
	thinkos_mutex_unlock(dbg->ice_mutex);

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}


static const char * const dbg_errtab[] = {
	"debugger OK",
	"debugger undefined error",
	"debugger invalid parameter",
	"debugger target_mutex (locked)",
	"debugger invalid state",
	"debugger driver fail"
};

char * target_strerror(int errno)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;

	if (errno > 0)
		errno = 0;

	if (errno < -200) {
		/* redirect to the LINK driver error message helper */
		/* FIXME: support to multiple link drivers */
		return jtag_strerror(errno);
	}

	if (errno < -100) {
		/* redirect to the ICE driver error message helper */
		return ice_strerror(ice, errno);
	}

	/* range check */
	if (errno < -4)
		errno = -1;

	return (char *)dbg_errtab[-errno];
}

void debugger_except(const char * msg)
{
	struct debugger * dbg = &debugger;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state > DBG_ST_UNCONNECTED) {
		/* FIXME: if the target is connected a proper shutdown sohuld be 
		   performed prior to flag a fault */
		DCC_LOG(LOG_ERROR, "undefined behavior"); 
	}

	INF("EXCEPTION: %s", msg);

	dbg->state = DBG_ST_FAULT;
	DCC_LOG(LOG_TRACE, "[DBG_ST_FAULT]");

	thinkos_mutex_unlock(dbg->target_mutex);
}

int mod_ice_register(struct debugger * dbg);

uint32_t dbg_poll_stack[96];

const struct thinkos_thread_inf dbg_poll_inf = {
	.stack_ptr = dbg_poll_stack, 
	.stack_size = sizeof(dbg_poll_stack),
	.priority = 128,
	.thread_id = 32,
	.paused = false,
	.tag = "DBG_POL"
};

void debugger_init(void)
{
	struct debugger * dbg = &debugger;

	dbg->target_mutex = thinkos_mutex_alloc();
	DCC_LOG1(LOG_TRACE, "debugger target_mutex mutex: %d", dbg->target_mutex);

	/* initialize the breakpoint management */
	dbg_bp_init(&dbg->bp_ctrl);

	/* initialize the watchpoint management */
	dbg_wp_init(&dbg->wp_ctrl);

	/* open the NULL ICE driver */
	ice_open(&dbg->ice, &ice_drv_null, &dbg_ice_ctrl_buf.ctrl);
	dbg->state = DBG_ST_UNDEF;
	DCC_LOG(LOG_TRACE, "[DBG_ST_UNDEF]");

	dbg->cfg.enable_ice_polling = true;

	dbg->dasm.base = 0;
	dbg->dasm.size = 20;
	dbg->dump.base = 0;
	dbg->dump.size = 256;
	dbg->stack.base = 0;
	dbg->stack.size = 64;

	dbg->mem = target_null_mem;
	dbg->target = &target_null;
	dbg->mem_mod_id = -1;
	dbg->tcp_port = 9;
	dbg->transf.size = 64 * 1024;

	dbg->ice_mutex = thinkos_mutex_alloc();

	/* FIXME: the dcc semaphore must be created/destroyed in the
	   TAP sctructure */
	dbg->poll_cond = thinkos_cond_alloc();
	dbg->halt_cond = thinkos_cond_alloc();
	dbg->poll_enabled = false;

	DCC_LOG3(LOG_TRACE, "ice_mutex=%d poll_cond=%d halt_cond=%d", 
			 dbg->ice_mutex, dbg->poll_cond, dbg->halt_cond);

	ice_comm_init(&dbg->comm);

	dbg->poll_thread = thinkos_thread_create_inf((void *)dbg_poll_task, 
						(void *)dbg, &dbg_poll_inf);

	DCC_LOG1(LOG_TRACE, "thinkos_thread_create()=%d", dbg->poll_thread);

	mod_ice_register(dbg);
}

int target_enable_ice_poll(bool flag)
{
	struct debugger * dbg = &debugger;

	DCC_LOG(LOG_TRACE, ".");

	thinkos_mutex_lock(dbg->target_mutex);

	poll_stop(dbg);

	dbg->cfg.enable_ice_polling = flag;

	poll_start(dbg);

	thinkos_mutex_unlock(dbg->target_mutex);

	return 0;
}

int target_enable_comm(bool flag)
{
	return 0;
}

#if 0
int target_int_enable(void)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	if ((ret = ice_int_enable(ice) )< 0) {
		DCC_LOG(LOG_WARNING, "drv->int_enable() fail!");
	}

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}

int target_int_disable(void)
{
	struct debugger * dbg = &debugger;
	ice_drv_t * ice = (ice_drv_t *)&dbg->ice;
	int ret;

	thinkos_mutex_lock(dbg->target_mutex);

	if (dbg->state < DBG_ST_CONNECTED) {
		DCC_LOG(LOG_WARNING, "invalid state"); 
		thinkos_mutex_unlock(dbg->target_mutex);
		return ERR_STATE;
	}

	if ((ret = ice_int_disable(ice) )< 0) {
		DCC_LOG(LOG_WARNING, "drv->int_disable() fail!");
	}

	thinkos_mutex_unlock(dbg->target_mutex);

	return ret;
}
#endif
