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
 * @file armice.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifdef CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "jtag_arm.h"
#include "armice.h"
#include "arm_pn.h"
#include "dbglog.h"

#ifndef ENABLE_ARMICE_ARM9
#define ENABLE_ARMICE_ARM9 1
#endif

static int core_on_break(armice_ctrl_t * ctrl, int dbg_status)
{
	int ret;

	/* ON HALT */
	if (dbg_status & ARMICE_ST_TBIT) {
		DCC_LOG(LOG_TRACE, "[HALT] THUMB mode");
		ctrl->flags |= ARMICE_THUMB;
	} else {
		DCC_LOG(LOG_TRACE, "[HALT] ARM mode");
		ctrl->flags &= ~ARMICE_THUMB;
	}

	DCC_LOG(LOG_TRACE, "disable interrupts, DBGACK force signal ON");
	/* disable interrupts and signal the periphery that we are in 
	   debug mode */
	ret = jtag_arm_dbg_ctrl_wr(ctrl->tap, ARMICE_CTRL_INTDIS | 
							   ARMICE_CTRL_DBGACK, JTAG_TAP_DRUPDATE);
	
	if (ctrl->flags & ARMICE_WP1_EN) {
		DCC_LOG(LOG_TRACE, "disabling WP1...");
		if ((ret = arm_wp_clr(ctrl->tap, 1)) != JTAG_OK) {
			return ret;
		}
		ctrl->flags &= ~ARMICE_WP1_EN;
	}

	if (!(ctrl->flags & ARMICE_BRK_EN)) {
		DCC_LOG(LOG_TRACE, "setting WP0 break condition ...");
		if ((ret = arm_wp_brk_req(ctrl->tap)) != JTAG_OK) {
			return ret;
		}
		ctrl->flags |= (ARMICE_BRK_EN | ARMICE_WP0_EN);
	}

	if (ctrl->flags & ARMICE_SSTEP_EN) {
		DCC_LOG(LOG_WARNING, "single step flag enabled !!!!");
		ctrl->flags &= ~ARMICE_SSTEP_EN;
	}

	/* Invalidade the code cache */
	ctrl->code_cache.addr = 0xffffffff;
	ctrl->code_cache.ptr = NULL;

	return ret;
}

static void core_status_update(armice_ctrl_t * ctrl, int dbg_status)
{
	if (dbg_status & ARMICE_ST_DBGACK) {
		if ((ctrl->dbg_status < 0) || !(ctrl->dbg_status & ARMICE_ST_DBGACK)) {
			DCC_LOG(LOG_TRACE, "---- HALT ---");
			core_on_break(ctrl, dbg_status);
		}
	} else {
		if ((ctrl->dbg_status < 0) || (ctrl->dbg_status & ARMICE_ST_DBGACK)) {
			DCC_LOG(LOG_TRACE, "++++ RUN ++++");
		}
	}

	/* cache the debug status */
	ctrl->dbg_status = dbg_status;
}

/* stop the core */
static int core_break_req(armice_ctrl_t * ctrl)
{
	int ret;
	int dbg_status;

	if (ctrl->flags & ARMICE_WP1_EN) {
		DCC_LOG(LOG_TRACE, "disabling WP1...");
		if ((ret = arm_wp_clr(ctrl->tap, 1)) != JTAG_OK) {
			return ret;
		}
		ctrl->flags &= ~ARMICE_WP1_EN;
	}

	if (ctrl->flags & ARMICE_WP0_EN) {
		DCC_LOG(LOG_TRACE, "disabling WP0...");
		if ((ret = arm_wp_clr(ctrl->tap, 0)) != JTAG_OK) {
			return ret;
		}
		ctrl->flags &= ~ARMICE_WP0_EN;
	}

	DCC_LOG(LOG_TRACE, "setting WP0 breakpoint...");

	if ((ret = arm_wp_brk_req(ctrl->tap)) != JTAG_OK) {
		return ret;
	}

	ctrl->flags |= (ARMICE_BRK_EN | ARMICE_WP0_EN);

	DCC_LOG(LOG_TRACE, "waiting for DBGACK...");

	if ((dbg_status = ctrl->jtag_arm_dbgack_wait(ctrl->tap, 100)) < 0) {
		return dbg_status;
	}

	DCC_LOG5(LOG_TRACE, " THUMB:%d nMREQ:%d IFEN:%d DBGRQ:%d DBGACK:%d", 
		(dbg_status & ARMICE_ST_TBIT) ? 1 : 0,
		(dbg_status & ARMICE_ST_NMREQ) ? 1 : 0,
		(dbg_status & ARMICE_ST_IFEN) ? 1 : 0,
		(dbg_status & ARMICE_ST_DBGRQ) ? 1 : 0,
		(dbg_status & ARMICE_ST_DBGACK) ? 1 : 0);

	core_status_update(ctrl, dbg_status);

	return dbg_status;
}


/* prepare to resume exexcution */
static int core_resume_prepare(armice_ctrl_t * ctrl)
{
	int ret;

	DCC_LOG(LOG_TRACE, "reenable interrupts, DBGACK force signal OFF");

	/* reenable interrupts and remove the DBGACK force signal */
	if ((ret = jtag_arm_dbg_ctrl_wr(ctrl->tap, 0, JTAG_TAP_DRUPDATE)) < 0) {
		DCC_LOG(LOG_ERROR, "jtag_arm_dbg_ctrl_wr()");
		return ret;
	}

#if 0
	/* clear DBGRQ flag */
	ctrl->flags &= ~ARMICE_DBGRQ;
#endif

	/* restore/disable watchpoint 1 */
	if (ctrl->flags & ARMICE_WP1_SET) {
		DCC_LOG(LOG_TRACE, "restoring WP1...");
		/* when the WP1 is set it will destroy the single step 
		   configuration, then we should mark the single step
		   as not initializad */
		ctrl->flags &= ~ARMICE_SSTEP_INIT;
		if ((ret = arm_wp_set(ctrl->tap, 1, &ctrl->wp[1])) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_arm_wp1_restore()");
			return ret;
		}
		ctrl->flags |= ARMICE_WP1_EN;
	} else {
		if (ctrl->flags & ARMICE_WP1_EN) {
			DCC_LOG(LOG_TRACE, "disabling WP1...");
			if ((ret = arm_wp_clr(ctrl->tap, 1)) != JTAG_OK) {
				DCC_LOG(LOG_ERROR, "jtag_arm_wp1_disable()");
				return ret;
			}
			ctrl->flags &= ~ARMICE_WP1_EN;
		}
	}

	/* restore/disable watchpoint 0 */
	if (ctrl->flags & ARMICE_WP0_SET) {
			DCC_LOG(LOG_TRACE, "restoring WP0...");
		if ((ret = arm_wp_set(ctrl->tap, 0, &ctrl->wp[0])) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_arm_wp1_restore()");
			return ret;
		}
		ctrl->flags |= ARMICE_WP0_EN;
	} else {
		if (ctrl->flags & ARMICE_WP0_EN) {
			DCC_LOG(LOG_TRACE, "disabling WP0...");
			if ((ret = arm_wp_clr(ctrl->tap, 0)) != JTAG_OK) {
				DCC_LOG(LOG_ERROR, "jtag_arm_wp0_disable()");
				return ret;
			}
			ctrl->flags &= ~ARMICE_WP0_EN;
		}
	}

	if (ctrl->flags & ARMICE_BRK_EN) {
		ctrl->flags &= ~ARMICE_BRK_EN;
	}

	return ret;
}

/* restart the core */
static int core_resume(armice_ctrl_t * ctrl)
{
	int ret;
	DCC_LOG(LOG_TRACE, "exec and restart ... ... ...");

	ctrl->arm_exec(ctrl->tap, ctrl->flags & ARMICE_THUMB);

	if ((ret = jtag_arm_restart(ctrl->tap)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_arm_restart()");
		return ret;
	}

	DCC_LOG(LOG_TRACE, "[CORE RUNNING]...");

	return ret;
}

static void core_state_save(armice_ctrl_t * ctrl, armice_context_t * ct)
{
	if (!(ctrl->flags & ARMICE_CTX_SAVED)) {
		if (ctrl->flags & ARMICE_THUMB) {
			DCC_LOG(LOG_TRACE, "saving THUMB context <<< <<< ");
			ctrl->thumb_context_save(ctrl->tap, ctrl->flags, ct);
			DCC_LOG1(LOG_INFO, "thumb @ 0x%08x", ct->pc - 4);
		} else {
			DCC_LOG(LOG_TRACE, "saving ARM context <<< <<<");
			ctrl->arm_context_save(ctrl->tap, ctrl->flags, ct);
			DCC_LOG1(LOG_INFO, "arm @ 0x%08x", ct->pc - 8);
		}

		ctrl->flags |= ARMICE_CTX_SAVED;
	}
}

static void core_state_restore(armice_ctrl_t * ctrl, armice_context_t * ct)
{
	if (ctrl->flags & ARMICE_CTX_SAVED) {
		if (ctrl->flags & ARMICE_THUMB) {
			DCC_LOG(LOG_TRACE, "restoring THUMB context >>> >>>");
			ctrl->thumb_context_restore(ctrl->tap, ct);
		} else {
			DCC_LOG(LOG_TRACE, "restoring ARM context >>> >>>");
			ctrl->arm_context_restore(ctrl->tap, ct);
		}
		ctrl->flags &= ~ARMICE_CTX_SAVED;
	}
}

/***********************************************************************
 ICE Driver Methods
 ***********************************************************************/

int armice_poll(armice_ctrl_t * ctrl, ice_comm_t * comm)
{
	int dbg_status;

	/* invalidate the status */
	ctrl->poll.status = -1;

	DCC_LOG(LOG_TRACE, "ARM poll start >>>>");
	dbg_status = jtag_arm_poll(ctrl->tap, &ctrl->poll, comm);
	DCC_LOG(LOG_TRACE, "<<<< ARM poll done.");

	ctrl->polling = false;

	if (dbg_status < 0)
		return ICE_ST_FAULT;

	return (dbg_status & ARMICE_ST_DBGACK) ? ICE_ST_HALT : 0;
}

void armice_signal(armice_ctrl_t * ctrl, ice_sig_t sig)
{
	switch (sig) {
	case ICE_SIG_POLL_STOP:
		ctrl->poll.enabled = false;
		break;
	case ICE_SIG_POLL_START:
		ctrl->poll.enabled = true;
		ctrl->polling = true;
		break;
	case ICE_SIG_TARGET_RESET:
		break;
	} 
}

int armice_status(armice_ctrl_t * ctrl)
{
	int dbg_status;

	if (ctrl->polling) {
		DCC_LOG(LOG_MSG, "poll status!!!!");
		if ((dbg_status = ctrl->poll.status) < 0) {
			DCC_LOG1(LOG_MSG, "ctrl->poll.status: %d", dbg_status);
			return dbg_status;
		}
	} else {
		/* get the status of the ICE driver */
		if ((dbg_status = ctrl->jtag_arm_dbg_status(ctrl->tap)) < 0) {
			DCC_LOG1(LOG_ERROR, "jtag_arm_dbg_status(): %d", dbg_status);
			return dbg_status;
		}
	}

	DCC_LOG5(LOG_TRACE, " THUMB:%d nMREQ:%d IFEN:%d DBGRQ:%d DBGACK:%d", 
		(dbg_status & ARMICE_ST_TBIT) ? 1 : 0,
		(dbg_status & ARMICE_ST_NMREQ) ? 1 : 0,
		(dbg_status & ARMICE_ST_IFEN) ? 1 : 0,
		(dbg_status & ARMICE_ST_DBGRQ) ? 1 : 0,
		(dbg_status & ARMICE_ST_DBGACK) ? 1 : 0);

	core_status_update(ctrl, dbg_status);

	return (dbg_status & ARMICE_ST_DBGACK) ? ICE_ST_HALT : 0;
}

int armice_connect(armice_ctrl_t * ctrl, uint32_t idmask, 
					uint32_t idcomp, int force)
{
//	int dbg_status;
	int ret;

	DCC_LOG3(LOG_TRACE, "idmask=%08x idcomp=%08x force=%d", 
			 idmask, idcomp, force);

	/* reset the JTAG tap */
/*
	FIXME: configure whether to reset or not the TAP before 
	connecting.
	jtag_tap_reset();
	*/

	jtag_arm_cache_clr();

	/* We cann't be sure about the
	   state of the EmbeddedICE. So let's do housekeeping... */
	/* clear the debug control register:
	   - enable interrupts
	   - clear the DBGRQ flag
	   - clear the DBGACK */
	/* ARM9: disable the Monitor mode bit (bit 4) in the 
	   debug control register */
	DCC_LOG(LOG_TRACE, "debug control register clear ...");
	if ((ret = jtag_arm_dbg_ctrl_wr(ctrl->tap, 0, 
									JTAG_TAP_RUN_TEST)) != JTAG_OK) {
		return ret;
	}

	/* restore/disable watchpoint 0 */
	if (ctrl->flags & ARMICE_WP0_SET) {
			DCC_LOG(LOG_TRACE, "restoring WP0...");
		if ((ret = arm_wp_set(ctrl->tap, 0, &ctrl->wp[0])) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_arm_wp1_restore()");
			return ret;
		}
		ctrl->flags |= ARMICE_WP0_EN;
	} else {
		DCC_LOG(LOG_TRACE, "disabling WP0...");
		if ((ret = arm_wp_clr(ctrl->tap, 0)) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_arm_wp0_disable()");
			return ret;
		}
		ctrl->flags &= ~ARMICE_WP0_EN;
	}

	/* restore/disable watchpoint 1 */
	if (ctrl->flags & ARMICE_WP1_SET) {
		DCC_LOG(LOG_TRACE, "restoring WP1...");
		if ((ret = arm_wp_set(ctrl->tap, 1, &ctrl->wp[1])) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_arm_wp1_restore()");
			return ret;
		}
		ctrl->flags |= ARMICE_WP1_EN;
	} else {
		DCC_LOG(LOG_TRACE, "disabling WP1...");
		if ((ret = arm_wp_clr(ctrl->tap, 1)) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_arm_wp1_disable()");
			return ret;
		}
		ctrl->flags &= ~ARMICE_WP1_EN;
	}

	ctrl->flags &= ~(ARMICE_SSTEP_EN | ARMICE_CTX_SAVED);
	ctrl->flags |= ARMICE_CONNECTED;

	ctrl->poll.enabled = 0;
	ctrl->poll.status = 0;
	ctrl->poll.count = 0;
	/* invalidate the debug status */
	ctrl->dbg_status = -1;

#if 0
	/* get the intial status of the ICE driver */
	if ((dbg_status = ctrl->jtag_arm_dbg_status(ctrl->tap)) < 0) {
		DCC_LOG1(LOG_ERROR, "jtag_arm_dbg_status(): %d", dbg_status);
		/* TODO: check the appropriate state for this error */
		return dbg_status;
	}

	DCC_LOG5(LOG_TRACE, " THUMB:%d nMREQ:%d IFEN:%d DBGRQ:%d DBGACK:%d", 
		(dbg_status & ARMICE_ST_TBIT) ? 1 : 0,
		(dbg_status & ARMICE_ST_NMREQ) ? 1 : 0,
		(dbg_status & ARMICE_ST_IFEN) ? 1 : 0,
		(dbg_status & ARMICE_ST_DBGRQ) ? 1 : 0,
		(dbg_status & ARMICE_ST_DBGACK) ? 1 : 0);

	/* cache the debug status */
	ctrl->dbg_status = dbg_status;
#endif
	return ICE_OK;
}

int armice_release(armice_ctrl_t * ctrl)
{
	int ret;

	core_state_restore(ctrl, &ctrl->context);

	ctrl->flags &= ~(ARMICE_CONNECTED | ARMICE_SSTEP_EN |
					 ARMICE_WP0_EN | ARMICE_WP1_EN);

	/* disable watchpoint 1 */
	if (ctrl->flags & ARMICE_WP1_EN) {
		DCC_LOG(LOG_TRACE, "disabling WP1...");
		if ((ret = arm_wp_clr(ctrl->tap, 1)) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_arm_wp1_disable()");
			return ret;
		}
	}

	/* disable watchpoint 0 */
	if (ctrl->flags & ARMICE_WP0_EN) {
		DCC_LOG(LOG_TRACE, "disabling WP0...");
		if ((ret = arm_wp_clr(ctrl->tap, 0)) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "jtag_arm_wp0_disable()");
			return ret;
		}
	}

	jtag_arm_dbg_ctrl_wr(ctrl->tap, 0, JTAG_TAP_RUN_TEST);

	jtag_arm_cache_clr();

	return 0;
}

int armice_halt(armice_ctrl_t * ctrl)
{
	int dbg_status;

	/* break the core */
	if ((dbg_status = core_break_req(ctrl)) < 0) {
		DCC_LOG(LOG_ERROR, "core_break_req()"); 
		return dbg_status;
	}

	DCC_LOG4(LOG_INFO, "DBGACK:%d THUMB:%d nMREQ:%d DBGRQ:%d", 
		(dbg_status & ARMICE_ST_DBGACK) ? 1 : 0,
		(dbg_status & ARMICE_ST_TBIT) ? 1 : 0,
		(dbg_status & ARMICE_ST_NMREQ) ? 1 : 0,
		(dbg_status & ARMICE_ST_DBGRQ) ? 1 : 0);

	return ICE_OK;
}

int armice_run(armice_ctrl_t * ctrl)
{
	int ret;

	DCC_LOG(LOG_INFO, "."); 

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	if (ctrl->mem_lock) {
		DCC_LOG(LOG_WARNING, "locked"); 
		return -1;
	}

	core_state_restore(ctrl, &ctrl->context);

	if ((ret = core_resume_prepare(ctrl)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "core_resume()");
		return ret;
	}

	if ((ret = core_resume(ctrl)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "core_resume()");
		return ret;
	}

	return ICE_OK;
}

int armice_goto(armice_ctrl_t * ctrl, unsigned int addr)
{
	struct armice_context * ct = &ctrl->context;
	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

#if 0
	XXX: check what is this thing here....
	if (ctrl->flags & ARMICE_SSTEP_EN) {
		if ((ret = arm_sstp_disable(ctrl->tap)) != JTAG_OK) {
			return ret;
		}
		ctrl->flags &= ~ARMICE_SSTEP_EN;
	}
#endif

	core_state_save(ctrl, ct);

	if ((addr & 1) || (ct->cpsr & ARM_STATE_THUMB)) {
		ct->pc = (addr & 0xfffffffe) + 4;
		DCC_LOG2(LOG_TRACE, "thumb: addr=%08x pc=%08x", addr, ct->pc);
		ct->cpsr |= ARM_STATE_THUMB;
	} else {
		ct->pc = (addr & 0xfffffffc) + 8;
		DCC_LOG2(LOG_TRACE, "arm: addr=%08x pc=%08x", addr, ct->pc);
	}

	return ICE_OK;
}

int armice_step(armice_ctrl_t * ctrl)
{
	struct armice_context * ct = &ctrl->context;
	int dbg_status;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	core_state_save(ctrl, ct);

	if (!(ctrl->flags & ARMICE_SSTEP_INIT)) {
		DCC_LOG(LOG_TRACE, "single step enable.....");
		arm_sstp_init(ctrl->tap);
		ctrl->flags |= ARMICE_SSTEP_INIT;
	}

	/* clear DBGACK flag */
	jtag_arm_dbg_ctrl_wr(ctrl->tap, ARMICE_CTRL_INTDIS, JTAG_TAP_DRUPDATE);

	if (ctrl->flags & ARMICE_THUMB) {
		if ((dbg_status = ctrl->thumb_insn_step(ctrl->tap, ct)) < 0) {
			DCC_LOG(LOG_ERROR, "thumb_insn_step()");
		}
	} else {
		if ((dbg_status = ctrl->arm_insn_step(ctrl->tap, ct)) < 0) {
			DCC_LOG(LOG_ERROR, "arm_insn_step()");
		}
	}

	/* invalidate context */
	ctrl->flags &= ~ARMICE_CTX_SAVED;

	if (dbg_status < 0) {
		/* XXX: step error, this may  happen due to a branch to 
		 the current address: 'b ." */
		ctrl->dbg_status = -1;
		if ((dbg_status = core_break_req(ctrl)) < 0) {
			DCC_LOG(LOG_ERROR, "core_break_req()"); 
			return dbg_status;
		}
	} else {
		/* adjust the THUMB flag */
		if (dbg_status & ARMICE_ST_TBIT) {
			DCC_LOG(LOG_TRACE, "[HALT] THUMB mode");
			ctrl->flags |= ARMICE_THUMB;
		} else {
			DCC_LOG(LOG_TRACE, "[HALT] ARM mode");
			ctrl->flags &= ~ARMICE_THUMB;
		}
	}
	/* set DBGACK flag */
	jtag_arm_dbg_ctrl_wr(ctrl->tap, ARMICE_CTRL_INTDIS | 
						 ARMICE_CTRL_DBGACK, JTAG_TAP_DRUPDATE);

	DCC_LOG5(LOG_TRACE, " THUMB:%d nMREQ:%d IFEN:%d DBGRQ:%d DBGACK:%d", 
		(dbg_status & ARMICE_ST_TBIT) ? 1 : 0,
		(dbg_status & ARMICE_ST_NMREQ) ? 1 : 0,
		(dbg_status & ARMICE_ST_IFEN) ? 1 : 0,
		(dbg_status & ARMICE_ST_DBGRQ) ? 1 : 0,
		(dbg_status & ARMICE_ST_DBGACK) ? 1 : 0);

	return ICE_OK;
}

int armice_core_reset(armice_ctrl_t * ctrl)
{
	struct armice_context * ct = &ctrl->context;
	int i;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	core_state_save(ctrl, ct);

	for (i = 0; i < 16; i++)
		ct->r[i] = 0;

	ct->cpsr = ARM_DISABLE_IRQ | ARM_DISABLE_FIQ | ARM_MODE_SUPERVISOR;

	return ICE_OK;
}

int armice_mem_lock(armice_ctrl_t * ctrl)
{
	struct armice_context * ct = &ctrl->context;
	int dbg_status;
	int ret;

	if (ctrl->mem_lock) {
		ctrl->mem_lock++;
		DCC_LOG1(LOG_TRACE, "# LOCK(%d) #", ctrl->mem_lock);
		return ICE_ERR_LOCKED;
	}

	if (ctrl->polling) {
		DCC_LOG(LOG_ERROR, "poll task is running!");
		return -1;
	}

	if ((ctrl->dbg_status & ARMICE_ST_DBGACK) == 0) {
		DCC_LOG(LOG_TRACE, "core RUNNING..."); 
		/* break the core */
		if ((dbg_status = core_break_req(ctrl)) < 0) {
			DCC_LOG(LOG_ERROR, "core_break_req() failed!");
			return dbg_status;
		}

		if ((dbg_status & ARMICE_ST_DBGACK) == 0) {
			return ICE_ERR_HALT_TMO;
		}

		/* indicate that we have to restart the core 
		   on unlock */
		ctrl->flags |= ARMICE_RESUME;
	} else {
		DCC_LOG(LOG_TRACE, "core HALTED..."); 
		/* clearing the single step execution */
		if (ctrl->flags & ARMICE_SSTEP_EN) {
			DCC_LOG(LOG_TRACE, "setting WP0 break condition ...");
			if ((ret = arm_wp_brk_req(ctrl->tap)) != JTAG_OK) {
				return ret;
			}
			ctrl->flags |= (ARMICE_BRK_EN | ARMICE_WP0_EN);
			ctrl->flags &= ~ARMICE_SSTEP_EN;
		}
	}

	/* saves the context */
	core_state_save(ctrl, ct);

	ctrl->mem_lock++;
	DCC_LOG1(LOG_TRACE, "# LOCK(%d) #", ctrl->mem_lock);

	return 0;
}

int armice_mem_unlock(armice_ctrl_t * ctrl)
{
	armice_context_t * ct = &ctrl->context;
	int dbg_status;
	int ret;

	if (ctrl->mem_lock == 0) {
		DCC_LOG(LOG_WARNING, "not locked"); 
		return -1;
	}

	DCC_LOG1(LOG_TRACE, "# UNLOCK(%d) #", ctrl->mem_lock);
	ctrl->mem_lock--;

	if (ctrl->mem_lock > 0) {
		return ICE_OK;
	}

	if (ctrl->polling) {
		DCC_LOG(LOG_ERROR, "poll task is running!");
		return -1;
	}

	if (!(ctrl->dbg_status & ARMICE_ST_DBGACK)) {
		/* core running,
		 in normal situations this shuldn't happen. */

		if (!(ctrl->flags & ARMICE_RESUME) && 
			!(ctrl->flags & ARMICE_CTX_SAVED)) {
			/* the core was running before lock, and is running
			   now but we have no context stored, 
			   this situation can occur if the
			   exec() function was called between lock()/unlock() */
			DCC_LOG(LOG_TRACE, "# UNLOCKED # exec()!!!"); 
			return ICE_OK;
		}

		if (ctrl->flags & ARMICE_CTX_SAVED) {
			/* clearing context flag */
			DCC_LOG(LOG_TRACE, "clearing CONTEXT"); 
			ctrl->flags &= ~ARMICE_CTX_SAVED;
		}

		/* The core was running before lock, and is running
		   now, but the context is probably wrong. Keep it running... */
		if (ctrl->flags & ARMICE_RESUME) {
			DCC_LOG(LOG_TRACE, "# UNLOCKED # running"); 
			ctrl->flags &= ~ARMICE_RESUME;
		}

		return ICE_OK;
	} 

	if (!(ctrl->flags & ARMICE_RESUME)) {
		DCC_LOG(LOG_TRACE, "halted, # UNLOCKED #"); 
		return ICE_OK;
	}
	ctrl->flags &= ~ARMICE_RESUME;

	core_state_restore(ctrl, ct);

	if ((ret = core_resume_prepare(ctrl)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "core_resume()");
		return ret;
	}

	if ((ret = core_resume(ctrl)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "core_resume()");
		return ret;
	}

	/* get the status of the ICE driver */
	if ((dbg_status = ctrl->jtag_arm_dbg_status(ctrl->tap)) < 0) {
		DCC_LOG1(LOG_ERROR, "jtag_arm_dbg_status(): %d", dbg_status);
		return dbg_status;
	}

	core_status_update(ctrl, dbg_status);

	DCC_LOG(LOG_TRACE, "running, # UNLOCKED #"); 

	return ICE_OK;
}



/***********************************************************************
 Initialization, finalization and configuration
 ***********************************************************************/

/* configure the ice driver */
int armice_configure(armice_ctrl_t * ctrl, jtag_tap_t * tap, 
					 ice_opt_t * opt, armice_cfg_t * cfg)
{
	jtag_idcode_t idcode;

	if (!(ctrl->flags & ARMICE_OPENED)) {
		DCC_LOG(LOG_WARNING, "not open!");
		return ICE_ERROR;
	}

	if (cfg == NULL) {
		DCC_LOG(LOG_WARNING, "cfg == NULL"); 
		return -1;
	}

	if (cfg->endianness) {
		DCC_LOG(LOG_ERROR, "BIG endian unsupported.");
		return ICE_ERR_BIG_ENDIAN;
	}
	
	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	ctrl->work_addr = cfg->work_addr;
	ctrl->work_size = cfg->work_size;
	DCC_LOG2(LOG_TRACE, "Work memory addr: %08x, size: %d.", 
			 ctrl->work_addr, ctrl->work_size);

	if (jtag_tap_idcode((jtag_tap_t *)tap, &idcode) != JTAG_OK) {
		DCC_LOG(LOG_WARNING, "jtag_tap_idcode() fail!"); 
		return -1;
	}

	DCC_LOG1(LOG_TRACE, "idcode:%08x", idcode); 

	arm_pn_decode(idcode, &ctrl->pn);

	if (ctrl->pn.device == 0) {
		DCC_LOG2(LOG_TRACE, "ARM%d%s", ctrl->pn.family, 
				 arm_capability_str(&ctrl->pn));
	} else {
		DCC_LOG3(LOG_TRACE, "ARM%d%02x%s", ctrl->pn.family, 
				 ctrl->pn.device, arm_capability_str(&ctrl->pn));
	}

	if (ctrl->pn.family == 7) {
		/* configure the driver */
		ctrl->jtag_arm_dbg_status = jtag_arm7_dbg_status;
		ctrl->jtag_arm_dbgack_wait = jtag_arm7_dbgack_wait;
		ctrl->arm_exec = arm7_exec;
		ctrl->thumb_context_save = arm7_thumb_context_save;
		ctrl->arm_context_save = arm7_context_save;
		ctrl->thumb_insn_step = arm7_thumb_insn_step;
		ctrl->arm_insn_step = arm7_insn_step;
		ctrl->thumb_context_restore = arm7_thumb_context_restore;
		ctrl->arm_context_restore = arm7_context_restore;
		ctrl->arm_wr8 = arm7_wr8;
		ctrl->arm_rd8 = arm7_rd8;
		ctrl->arm_wr16 = arm7_wr16;
		ctrl->arm_rd16 = arm7_rd16;
		ctrl->arm_wr32 = arm7_wr32;
		ctrl->arm_rd32 = arm7_rd32;
		ctrl->mem_set = arm7_mem_set;
		ctrl->mem_write = arm7_mem_write;
		ctrl->mem_read = arm7_mem_read;
		ctrl->code_exec = arm7_code_exec;
		ctrl->insn_fetch = arm7_insn_fetch;
	} else {
#if ENABLE_ARMICE_ARM9
		if (ctrl->pn.family == 9) {
			/* configure the driver */
			ctrl->jtag_arm_dbg_status = jtag_arm9_dbg_status;
			ctrl->jtag_arm_dbgack_wait = jtag_arm9_dbgack_wait;
			ctrl->arm_exec = arm9_exec;
			if (ctrl->pn.device == 0x26) {
				ctrl->thumb_context_save = arm926_thumb_context_save;
				ctrl->arm_context_save = arm926_context_save;
				ctrl->thumb_context_restore = arm926_thumb_context_restore;
				ctrl->arm_context_restore = arm926_context_restore;
			} else {
				ctrl->thumb_context_save = arm9_thumb_context_save;
				ctrl->arm_context_save = arm9_context_save;
				ctrl->thumb_context_restore = arm9_thumb_context_restore;
				ctrl->arm_context_restore = arm9_context_restore;
			}
			ctrl->thumb_insn_step = arm9_thumb_insn_step;
			ctrl->arm_insn_step = arm9_insn_step;
			ctrl->arm_wr8 = arm9_wr8;
			ctrl->arm_rd8 = arm9_rd8;
			ctrl->arm_wr16 = arm9_wr16;
			ctrl->arm_rd16 = arm9_rd16;
			ctrl->arm_wr32 = arm9_wr32;
			ctrl->arm_rd32 = arm9_rd32;
			ctrl->mem_set = arm9_mem_set;
			ctrl->mem_write = arm9_mem_write;
			ctrl->mem_read = arm9_mem_read;
			ctrl->code_exec = arm9_code_exec;
			ctrl->insn_fetch = arm9_insn_fetch;
		} else 
#endif
		{
			DCC_LOG(LOG_ERROR, "ARM family not unsupported.");
			return ICE_ERR_BIG_ENDIAN;
		}
	};

	jtag_tap_select(tap);
	jtag_arm_tap_setup(tap, ctrl->pn.family);

	ctrl->tap = tap;
	opt->bp_max = 2;
	opt->bp_defsz = 4;

	/* clear watchpoints */
	memset(&ctrl->wp[0], 0, sizeof(jtag_arm_wp_t));
	memset(&ctrl->wp[1], 0, sizeof(jtag_arm_wp_t));
	arm_wp_set(ctrl->tap, 0, &ctrl->wp[0]);
	arm_wp_set(ctrl->tap, 1, &ctrl->wp[1]);

#if 0
	if ((ctrl->pn.family == 9) && (ctrl->pn.device == 0x26)) {
		uint32_t data;
		/* CP15 */
		jtag_arm926_cp15_rd(tap, CP15_C0_IDCODE, &data);
		DCC_LOG1(LOG_TRACE, "CP15 IDCODE: %08x", data);

		jtag_arm926_cp15_rd(tap, CP15_C0_CACHE_TYPE, &data);

		DCC_LOG2(LOG_TRACE, "ICahe: %d, DCache: %d", 
				 CACHE_SIZE(C0_ISIZE(data)), CACHE_SIZE(C0_DSIZE(data)));

		jtag_arm926_cp15_rd(tap, CP15_C0_TCM_TYPE, &data);
		DCC_LOG2(LOG_TRACE, "CP15 DTCM:%d ITCM:%d", 
				 C0_TCM_DTCM(data), C0_TCM_ITCM(data));

		jtag_arm926_cp15_rd(tap, CP15_C1_CONTROL, &data);

		DCC_LOG3(LOG_TRACE, "CP15 C1 ICache:%s DCache:%s MMU:%s", 
				 (data & C1_ICACHE) ? "Enabled" : "Disabled",
				 (data & C1_DCACHE) ? "Enabled" : "Disabled",
				 (data & C1_MMU) ? "Enabled" : "Disabled");

		if (data & (C1_ICACHE | C1_DCACHE | C1_MMU)) {
			data &= ~(C1_ICACHE | C1_DCACHE | C1_MMU);
			jtag_arm926_cp15_wr(tap, CP15_C1_CONTROL, data);
		}

		jtag_arm926_cp15_rd(tap, CP15_ADDR(7, 0, 15, 0), &data);
		data |= 0x7;
		jtag_arm926_cp15_wr(tap, CP15_ADDR(7, 0, 15, 0), data);
	}
#endif

	DCC_LOG(LOG_TRACE, "[UNCONNECTED]");
	return ICE_OK;
}

int armice_context_show(armice_ctrl_t * ctrl, FILE * f)
{
	int thumb;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	armice_context_t * ct = &ctrl->context;

	DCC_LOG(LOG_INFO, ".");

	core_state_save(ctrl, ct);

	thumb = (ct->cpsr & ARM_STATE_THUMB);

	if (thumb)
		fprintf(f, "THUMB state\n");
	else
		fprintf(f, "ARM state\n");

	arm_show_cpsr(f, ct->cpsr);

	arm_show_regs(f, ct->r);

	return ICE_OK;
}

int armice_wr8(armice_ctrl_t * ctrl, uint32_t addr, uint8_t data)
{
	armice_context_t * ct = &ctrl->context;
	int ret;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	core_state_save(ctrl, ct);

	if ((ret = ctrl->arm_wr8(ctrl->tap, addr, data)) < 0) {
		DCC_LOG(LOG_ERROR, "arm_wr8()");
		return ret;
	}

	return ICE_OK;
}

int armice_rd8(armice_ctrl_t * ctrl, uint32_t addr, uint8_t * data)
{
	armice_context_t * ct = &ctrl->context;
	int ret;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	core_state_save(ctrl, ct);

	if ((ret = ctrl->arm_rd8(ctrl->tap, addr, data)) < 0) {
		DCC_LOG(LOG_ERROR, "arm_rd8()");
		return ret;
	}

	return ICE_OK;
}

int armice_wr16(armice_ctrl_t * ctrl, uint32_t addr, uint16_t data)
{
	armice_context_t * ct = &ctrl->context;
	int ret;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	core_state_save(ctrl, ct);

	if ((ret = ctrl->arm_wr16(ctrl->tap, addr, data)) < 0) {
		DCC_LOG(LOG_ERROR, "arm_wr16()");
		return ret;
	}

	return ICE_OK;
}

int armice_rd16(armice_ctrl_t * ctrl, uint32_t addr, uint16_t * data)
{
	armice_context_t * ct = &ctrl->context;
	int ret;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	core_state_save(ctrl, ct);

	if ((ret = ctrl->arm_rd16(ctrl->tap, addr, data)) < 0) {
		DCC_LOG(LOG_ERROR, "arm_rd16()");
		return ret;
	}

	return ICE_OK;
}

int armice_wr32(armice_ctrl_t * ctrl, uint32_t addr, uint32_t data)
{
	armice_context_t * ct = &ctrl->context;
	int ret;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	core_state_save(ctrl, ct);

	if ((ret = ctrl->arm_wr32(ctrl->tap, addr, data)) < 0) {
		DCC_LOG(LOG_ERROR, "arm_wr32()");
		return ret;
	}

	return ICE_OK;
}

int armice_rd32(armice_ctrl_t * ctrl, uint32_t addr, uint32_t * data)
{
	armice_context_t * ct = &ctrl->context;
	int ret;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	core_state_save(ctrl, ct);

	if ((ret = ctrl->arm_rd32(ctrl->tap, addr, data)) < 0) {
		DCC_LOG(LOG_ERROR, "arm_rd32()");
		return ret;
	}

	return ICE_OK;
}

extern struct armice_codelet codelet_fifo_write;
extern struct armice_codelet codelet_fifo_read;

int armice_fifo_rd8(armice_ctrl_t * ctrl, uint32_t addr, void * buf, int len)
{
	uint32_t exec_addr = ctrl->work_addr;
	armice_context_t * ct = &ctrl->context;
	uint32_t data;
	uint8_t * cp;
	uint32_t reg[2];
	int ret;
	int rem;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	core_state_save(ctrl, ct);

	DCC_LOG(LOG_INFO, "code load...");

	if ((ret = armice_code_load(ctrl, exec_addr, 
								codelet_fifo_read.code, 
								codelet_fifo_read.size)) < 0) { 
		DCC_LOG(LOG_ERROR, "armice_code_load()");
		return ret;
	}

	DCC_LOG2(LOG_INFO, "addr: 0x%08x len: %d...", addr, len);

	reg[0] = addr;
	reg[1] = len;
	if ((ret = armice_code_exec(ctrl, exec_addr, reg, 2)) < 0) { 
		DCC_LOG(LOG_ERROR, "arm7ice_code_load()");
		return ret;
	}

	cp = (uint8_t *)buf;
	ret = ICE_OK;
	rem = len;

	while (rem) {
		/* TODO: adjust the timeout parameter according to the
		   processor, memory & jtag speeds... */
		if ((ret = jtag_arm_dcc_in(ctrl->tap, &data, 100)) != JTAG_OK) {
			DCC_LOG(LOG_WARNING, "jtag_arm_dcc_in() fail!");
			return -1;
		}

		DCC_LOG1(LOG_INFO, "%08x", data);

		*cp++ = data;
		rem--;

		if (rem) {
			*cp++ = data >> 8;
			rem--;
		}

		if (rem) {
			*cp++ = data >> 16;
			rem--;
		}

		if (rem) {
			*cp++ = data >> 24;
			rem--;
		}
	}

	armice_code_stop(ctrl);

	return ret;
}


int armice_fifo_wr8(armice_ctrl_t * ctrl, uint32_t addr, 
					const void * buf, int len)
{
	uint32_t exec_addr = ctrl->work_addr;
	armice_context_t * ct = &ctrl->context;
	uint32_t data;
	uint8_t * cp;
	uint32_t reg[2];
	int ret;
	int rem;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	core_state_save(ctrl, ct);

	DCC_LOG(LOG_INFO, "code load...");

	if ((ret = armice_code_load(ctrl, exec_addr, 
								codelet_fifo_write.code, 
								codelet_fifo_write.size)) < 0) { 
		DCC_LOG(LOG_ERROR, "armice_code_load()");
		return ret;
	}

	DCC_LOG2(LOG_INFO, "addr: 0x%08x len: %d...", addr, len);

	reg[0] = addr;
	reg[1] = len;
	if ((ret = armice_code_exec(ctrl, exec_addr, reg, 2)) < 0) { 
		DCC_LOG(LOG_ERROR, "arm7ice_code_load()");
		return ret;
	}

	cp = (uint8_t *)buf;
	ret = ICE_OK;
	rem = len;

	while (rem) {

		data = *cp++;
		rem--;

		if (rem) {
			data |= *cp++ << 8;
			rem--;
		}

		if (rem) {
			data |= *cp++ << 16;
			rem--;
		}

		if (rem) {
			data |= *cp++ << 24;
			rem--;
		}
		
		DCC_LOG1(LOG_INFO, "%08x", data);
		/* TODO: adjust the timeout parameter according to the
		   processor, memory & jtag speeds... */
		if ((ret = jtag_arm_dcc_out(ctrl->tap, data, 100)) != JTAG_OK) {
			DCC_LOG(LOG_WARNING, "jtag_arm_dcc_out() fail!");
			break;
		}
	}

	armice_code_stop(ctrl);

	return rem;
}

int armice_reg_get(armice_ctrl_t * ctrl, int reg, uint32_t * val)
{
	struct armice_context * ct = &ctrl->context;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	if (reg > ARM_SPSR)
		return -1;

	core_state_save(ctrl, ct);

	if (reg == ARM_PC) {
		if (ct->cpsr & ARM_STATE_THUMB) {
			*val = ct->pc - 4;
		} else {
			*val = ct->pc - 8;
		}
		return 0;
	}

	if (reg == ARM_SPSR) {
		DCC_LOG(LOG_WARNING, "unsupported SPSR"); 
/*
FIXME:
		arm_spsr_get(ctrl, val);
*/
		return 0;
	}

	*val = ct->r[reg];

	return 0;
}

int armice_reg_set(armice_ctrl_t * ctrl, int reg, uint32_t val)
{
	struct armice_context * ct = &ctrl->context;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	if (reg > ARM_SPSR)
		return -1;

	core_state_save(ctrl, ct);

	if (reg == ARM_PC) {
		if (ct->cpsr & ARM_STATE_THUMB) {
			val &= 0xfffffffe;
		} else {
			val &= 0xfffffffc;
		}
	} else {
		if (reg == ARM_CPSR) {
			/* FIXME: mode change ?? */
		}
		if (reg == ARM_SPSR) {
/*
FIXME:
   			arm_spsr_set(ctrl, val);
*/

			return 0;
		}
	}

	ct->r[reg] = val;

	return 0;
}

/* Instruction fetch address get */
int armice_ifa_get(armice_ctrl_t * ctrl, uint32_t * val)
{
	struct armice_context * ct = &ctrl->context;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	core_state_save(ctrl, ct);

	if (ct->cpsr & ARM_STATE_THUMB) {
		*val = ct->pc - 4;
	} else {
		*val = ct->pc - 8;
	}

	return 0;
}

/* Instruction fetch address set */
int armice_ifa_set(armice_ctrl_t * ctrl, uint32_t val)
{
	struct armice_context * ct = &ctrl->context;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	core_state_save(ctrl, ct);

	if (ct->cpsr & ARM_STATE_THUMB) {
		ct->pc = val + 4;
	} else {
		ct->pc = val + 8;
	}

	return 0;
}

int armice_thumb_mode(armice_ctrl_t * ctrl, int thumb) 
{
	struct armice_context * ct = &ctrl->context;

	core_state_save(ctrl, ct);

	if (thumb) {
		if (!(ct->cpsr & ARM_STATE_THUMB)) {
			ct->cpsr |= ARM_STATE_THUMB;
			ct->pc -= 4;
			ctrl->flags |= ARMICE_THUMB;
		}
	} else {
		if (ct->cpsr & ARM_STATE_THUMB) {
			ct->cpsr &= ~ARM_STATE_THUMB;
			ct->pc += 4;
			ctrl->flags &= ~ARMICE_THUMB;
		}
	}

	return 0;
}


/*****************************************************************************
 * Breakpoints and Watchpoins
 *****************************************************************************/

int armice_bp_set(armice_ctrl_t * ctrl, uint32_t addr, 
				  uint32_t size, uint32_t * id_ptr)
{
	jtag_arm_wp_t * wp;
	uint32_t mask;
	int ret;
	int id;

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	DCC_LOG2(LOG_TRACE, "addr=%08x size=%d", addr, size);

	size >>= 1;
	for (mask = 0; size > mask; mask = (mask << 1) + 1);

	if (!(ctrl->flags & ARMICE_WP0_SET)) {
		id = 0;
		DCC_LOG(LOG_TRACE, "setting WP0...");
	} else {
		if (!(ctrl->flags & ARMICE_WP1_SET)) {
			id = 1;
			DCC_LOG(LOG_TRACE, "setting WP1...");
		} else {
			DCC_LOG(LOG_WARNING, "no hardware BP available !!!");
			return -1;
		}
	}

	wp = &ctrl->wp[id];

	wp->addr_value = addr;
	wp->addr_mask = mask;
	wp->data_value = 0x00000000;
	wp->data_mask = 0xffffffff;
	wp->ctrl_value = ARMICE_WP_ENABLE;
	wp->ctrl_mask = ~ARMICE_WP_NOPC & 0xff;

	if ((ctrl->dbg_status & ARMICE_ST_DBGACK) == 0) {
		if (id == 1) {
			/* when the WP1 is set it will destroy the single step 
			   configuration, then we should mark the single step
			   as not initializad */
			ctrl->flags &= ~ARMICE_SSTEP_INIT;
		}
		if ((ret = arm_wp_set(ctrl->tap, id, wp)) != JTAG_OK) {
			DCC_LOG(LOG_ERROR, "arm_wp_set()");
			return ret;
		}
	}

	if (id == 0)
		ctrl->flags |= ARMICE_WP0_SET;
	else
		ctrl->flags |= ARMICE_WP1_SET;

	if (id_ptr != NULL)
		*id_ptr = id;

	return 0;
}

int armice_bp_clr(armice_ctrl_t * ctrl, uint32_t id)
{
	DCC_LOG1(LOG_INFO, "id=%d", id);

	if (ctrl->polling) {
		DCC_LOG(LOG_WARNING, "poll task is running!");
		return -1;
	}

	if (id == 0) {
		if (!(ctrl->flags & ARMICE_WP0_SET)) {
			DCC_LOG(LOG_WARNING, "WP0 is not set...");
			return -1;
		}
		DCC_LOG(LOG_TRACE, "clearing WP0...");
		ctrl->flags &= ~ARMICE_WP0_SET;
		return 0;
	}

	if (id == 1) {
		if (!(ctrl->flags & ARMICE_WP1_SET)) {
			DCC_LOG(LOG_WARNING, "WP1 is not set...");
			return -1;
		}
		DCC_LOG(LOG_TRACE, "clearing WP1...");
		ctrl->flags &= ~ARMICE_WP1_SET;
		return 0;
	}

	DCC_LOG1(LOG_WARNING, "invalid hardware BP: %d !", id);

	return -1;
}

int armice_wp_set(armice_ctrl_t * ctrl, int n, ice_addr_t addr, 
				  ice_addr_mask_t mask)
{
	DCC_LOG3(LOG_TRACE, "n=%d, addr=%08x mask=%08x", n, addr, mask);
	return -1;
}

int armice_wp_clr(armice_ctrl_t * ctrl, int n)
{
	DCC_LOG1(LOG_TRACE, "n=%d", n);
	return -1;
}

/*****************************************************************************
 * Codelet
 * Load, executes, stop and resume codelets on target.
 *****************************************************************************/

/*
 * load a codelet on target
 */
int armice_code_load(armice_ctrl_t * ctrl, uint32_t addr, 
					 uint32_t * ptr, int size)
{
	int ret;

	if ((ctrl->code_cache.addr == addr) && 
		(ctrl->code_cache.ptr == ptr)) {
		/* reuse the code... */
		DCC_LOG(LOG_INFO, "cache hit...");
		return 0;
	}

	DCC_LOG2(LOG_TRACE, "addr=%08x ptr=%08x", addr, ptr);

	if ((ret = ctrl->mem_write(ctrl->tap, addr, 0, ptr, size)) <= 0)
		return ret;

	ctrl->code_cache.addr = addr;
	ctrl->code_cache.ptr = ptr;
	return ret;
}

/*
 * execute a codelet on target
 * inner code
 */
int armice_code_exec(armice_ctrl_t * ctrl, uint32_t addr, 
					 uint32_t * regv, int regc)
{
	int ret;

	ctrl->code_exec(ctrl->tap, addr, regv, regc);

	/* disable interrupts and remove the DBGACK force signal */
	jtag_arm_dbg_ctrl_wr(ctrl->tap, ARMICE_CTRL_INTDIS, JTAG_TAP_DRUPDATE);

	/* disable WP0 used to single step execution */
	if ((ret = arm_wp_clr(ctrl->tap, 0)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_arm_wp0_disable()");
		return ret;
	}

	/* */
	if ((ret = jtag_arm_restart(ctrl->tap)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_arm_restart()");
	}

	return ret;
}

/* API wrapper to execute a codelet on target */
int armice_exec(armice_ctrl_t * ctrl, uint32_t addr, uint32_t * regv, int regc)
{
	int dbg_status; 
	int ret;

	if (ctrl->flags & ARMICE_CTX_SAVED) {
		/* clearing context flag */
		DCC_LOG(LOG_INFO, "clearing CTX_SAVED flag"); 
		ctrl->flags &= ~ARMICE_CTX_SAVED;
	}

	if (ctrl->flags & ARMICE_THUMB) {
		DCC_LOG(LOG_INFO, "clearing THUMB flag"); 
		/* only arm code */
		ctrl->flags &= ~ARMICE_THUMB;
	}
#if 0
	if (ctrl->flags & ARMICE_RESUME) {
		/* clearing resume flag, */
		DCC_LOG(LOG_INFO, "clearing RESUME flag"); 
		ctrl->flags &= ~ARMICE_RESUME;
	}
#endif
	if ((ret = core_resume_prepare(ctrl)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "core_resume()");
		return ret;
	}

	ctrl->code_exec(ctrl->tap, addr, regv, regc);

	/* */
	if ((ret = jtag_arm_restart(ctrl->tap)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_arm_restart()");
	}

	/* get the status of the ICE driver */
	if ((dbg_status = ctrl->jtag_arm_dbg_status(ctrl->tap)) < 0) {
		DCC_LOG1(LOG_ERROR, "jtag_arm_dbg_status(): %d", dbg_status);
		return dbg_status;
	}

	core_status_update(ctrl, dbg_status);

	return ret;
}

/*
 * stop execution
 */
int armice_code_stop(armice_ctrl_t * ctrl)
{
	int dbg_status;
	int ret;

	if ((ret = arm_wp_brk_req(ctrl->tap)) != JTAG_OK) {
		return ret;
	}

	/* disable interrupts and signal the periphery that we are in 
	   debug mode */
	jtag_arm_dbg_ctrl_wr(ctrl->tap, ARMICE_CTRL_INTDIS | ARMICE_CTRL_DBGACK, 
						 JTAG_TAP_DRUPDATE);

	dbg_status = ctrl->jtag_arm_dbgack_wait(ctrl->tap, 100);

	DCC_LOG4(LOG_INFO, "%s %s %s nMREQ:%d", 
			 (dbg_status & ARMICE_ST_DBGRQ) ? "DBGRQ" : "BREAK", 
			 (dbg_status & ARMICE_ST_DBGACK) ? "HALTED" : "RUNNING", 
			 (dbg_status & ARMICE_ST_TBIT) ? "THUMB" : "ARM",
			 (dbg_status & ARMICE_ST_NMREQ) ? 1 : 0);

	return dbg_status;
}

/*
 * resume execution
 */
int armice_code_resume(armice_ctrl_t * ctrl)
{
	int ret;

	ctrl->arm_exec(ctrl->tap, 0);

	if ((ret = arm_wp_clr(ctrl->tap, 0)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_arm_wp0_disable()");
		return ret;
	}

	/* */
	if ((ret = jtag_arm_restart(ctrl->tap)) != JTAG_OK) {
		DCC_LOG(LOG_ERROR, "jtag_arm_restart()");
	}

	return ret;
}


int armice_mem_read(armice_ctrl_t * ctrl, ice_mem_ref_t * addr, 
					void * p, ice_size_t len)
{
	return ctrl->mem_read(ctrl->tap, addr->base, addr->offs, p, len);
}

int armice_mem_write(armice_ctrl_t * ctrl, ice_mem_ref_t * addr, 
					 const void * p, ice_size_t len)
{
	return ctrl->mem_write(ctrl->tap, addr->base, addr->offs, p, len);
}

int armice_mem_zero(armice_ctrl_t * ctrl, ice_mem_ref_t * addr, ice_size_t len)
{
	return ctrl->mem_set(ctrl->tap, addr->base, addr->offs, 0, len);
}

int armice_print_insn(armice_ctrl_t * ctrl, uint32_t addr, 
				   struct disassemble_info * info)
{
	/* FIXME: select mach endian flags and options */
	info->endian_code = BFD_ENDIAN_LITTLE;
	info->mach = bfd_mach_arm_5TE;
	
	if (ctrl->context.cpsr & ARM_STATE_THUMB)
		info->disassembler_options = "force-thumb";
	else
		info->disassembler_options = "no-force-thumb";

	return print_insn_little_arm(addr, info);
};

/* initialize the ice driver */
int armice_open(armice_ctrl_t * ctrl)
{
	if (ctrl->flags & ARMICE_OPENED) {
		DCC_LOG(LOG_WARNING, "already open!");
		return ICE_ERROR;
	}

	DCC_LOG1(LOG_TRACE, "ctrl=0x%p [OPEN]", ctrl);

	/* initialize the control structure */
	ctrl->tap = NULL;
	ctrl->flags = ARMICE_OPENED;

	/* clear the context */
	memset(&ctrl->context, 0, sizeof(struct armice_context));

	ctrl->poll.enabled = 0;
	ctrl->poll.count = 0;
	ctrl->poll.status = -1;
	ctrl->polling = false;
	ctrl->mem_lock = 0;

#if 0
	jtag_arm_dcc_init(&ctrl->arm_dcc);

	/* FIXME: the dcc semaphore must be created/destroyed in the
	   TAP sctructure */
	ctrl->poll_mutex = uthread_mutex_alloc();
	ctrl->poll_cond = uthread_cond_alloc();
	ctrl->halt_cond = uthread_cond_alloc();

	ctrl->poll_thread = uthread_create(ctrl->poll_stack, 
									   sizeof(ctrl->poll_stack), 
								 (uthread_task_t)poll_task, 
								 (void *)ctrl, 10, NULL);
	DCC_LOG1(LOG_TRACE, "uthread_create()=%d", ctrl->poll_thread);
#endif


	ctrl->mod_id = module_register(&armice_module, ctrl);

	return ICE_OK;
}

int armice_close(armice_ctrl_t * ctrl)
{
	if (!(ctrl->flags & ARMICE_OPENED)) {
		DCC_LOG(LOG_WARNING, "not open!");
		return ICE_ERROR;
	}

	if (ctrl->polling) {
		DCC_LOG(LOG_ERROR, "poll task is running!");
		ctrl->poll.enabled = 0;
	}

	DCC_LOG(LOG_TRACE, "[CLOSE]");
	module_unregister(ctrl->mod_id); 
	ctrl->mod_id = -1;
	ctrl->flags = 0;

	return ICE_OK;
}

int armice_test(armice_ctrl_t * ctrl, FILE * f, uint32_t val);

int armice_info(armice_ctrl_t * ctrl, FILE * f, uint32_t which);

const struct ice_oper armice_oper = {

	.open = (ice_open_t)armice_open,
	.close = (ice_close_t)armice_close,

	.configure = (ice_configure_t)armice_configure,
	.status = (ice_status_t)armice_status,
	.poll = (ice_poll_t)armice_poll,
	.signal = (ice_signal_t)armice_signal,
 
	.connect = (ice_connect_t)armice_connect,	
	.release = (ice_release_t)armice_release,	

	.halt = (ice_halt_t)armice_halt,
	.halt_wait = (ice_halt_wait_t)NULL,
	.on_break = (ice_step_t)NULL,

	.go_to = (ice_go_to_t)armice_goto,
	.run = (ice_run_t)armice_run,
	.step = (ice_step_t)armice_step,
	.exec = (ice_exec_t)armice_exec,

	.bp_set = (ice_bp_set_t)armice_bp_set,
	.bp_clr = (ice_bp_clr_t)armice_bp_clr,

	.wp_set = (ice_wp_set_t)armice_wp_set,
	.wp_clr = (ice_wp_clr_t)armice_wp_clr,

	.reg_get = (ice_reg_get_t)armice_reg_get,
	.reg_set = (ice_reg_set_t)armice_reg_set,

	.ifa_get = (ice_ifa_get_t)armice_ifa_get,
	.ifa_set = (ice_ifa_set_t)armice_ifa_set,

	.mem_lock = (ice_mem_lock_t)armice_mem_lock,
	.mem_unlock = (ice_mem_unlock_t)armice_mem_unlock,

	.rd8 = (ice_rd8_t)armice_rd8,
	.wr8 = (ice_wr8_t)armice_wr8,

	.rd16 = (ice_rd16_t)armice_rd16,
	.wr16 = (ice_wr16_t)armice_wr16,

	.rd32 = (ice_rd32_t)armice_rd32,
	.wr32 = (ice_wr32_t)armice_wr32,

	.fifo_rd8 = (ice_fifo_rd8_t)armice_fifo_rd8,
	.fifo_wr8 = (ice_fifo_wr8_t)armice_fifo_wr8,

	.test = (ice_test_t)armice_test,
	.info = (ice_info_t)armice_info,

	.core_reset = (ice_core_reset_t)armice_core_reset,
	.system_reset = (ice_system_reset_t)NULL,

	.context_show = (ice_context_show_t)armice_context_show,
	.print_insn = (ice_print_insn_t)armice_print_insn
};

const struct ice_drv_info armice_drv = {
	.name = "armice",
	.version = "0.3",
	.vendor = "BORESTE",
	.oper = &armice_oper
};


//at91_gpio_t pio_p31 = { .pio = PIOA, .pin = 31 };


#if 0
static int armice_reconnect(jtag_tap_t * tap)
{
	uint32_t idcode;

	DCC_LOG(LOG_TRACE, ".........");

	uthread_sleep(500);

	drv.dr_chain = -1;

	jtag_tap_reset();

	insn_drscan(DESC_IDCODE, JTAG_TAP_RUN_TEST);

	/* read the resulting vector */
	idcode = vec_rd32(0);

	if (idcode != tap->idcode) {
		DCC_LOG1(LOG_WARNING, "invalid IDCODE: %08x", idcode);
		return -1;
	}

	DCC_LOG1(LOG_TRACE, "match IDCODE:%08x", idcode);

	return 0;
}
#endif

#if 0
int poll_task(armice_ctrl_t * ctrl, uthread_id_t id)
{
	int status;

	DCC_LOG1(LOG_TRACE, "thread start: <%d>...", id);
	uthread_mutex_lock(ctrl->poll_mutex);

	for (;;) {
		while (!ctrl->poll.enabled) {
			DCC_LOG(LOG_INFO, "wait");
			uthread_cond_wait(ctrl->poll_cond, ctrl->poll_mutex);
			DCC_LOG(LOG_INFO, "signal");
#if 0
			/* do not enable poll to allow debugging .. */
			ctrl->poll.enabled = 0;
#endif
		}

		DCC_LOG(LOG_TRACE, "poll start");

		status = jtag_arm_poll(ctrl->tap, &ctrl->poll, &ctrl->arm_dcc);
		if (status < 0) {
			DCC_LOG1(LOG_ERROR, "jtag_arm_poll(): %d", status);
			/* TODO: check the appropriate action for this error */
			ctrl->poll.enabled = 0;
			continue;
		}

		if (status == 0) {
			DCC_LOG(LOG_INFO, "poll stop");
			continue;
		}

		DCC_LOG(LOG_INFO, "break!!!!");
		ctrl->poll.enabled = 0;
		uthread_cond_signal(ctrl->halt_cond);
	}

	uthread_mutex_unlock(ctrl->poll_mutex);
	return 0;
}
#endif

#if 0
static inline int poll_stop(armice_ctrl_t * ctrl)
{
	if (ctrl->poll.enabled) {
		DCC_LOG(LOG_TRACE, "stop polling...");
		/* stop polling */
		ctrl->poll.enabled = 0;
		/* sync */
		if (uthread_mutex_timedlock(ctrl->poll_mutex, 100) < 0) {
			DCC_LOG(LOG_WARNING, "mutex lock timeout!");
			return JTAG_ERR_ARM_POLL_STOP;
		}
		/* release mutex */
		uthread_mutex_unlock(ctrl->poll_mutex);
	}

	return 0;
}

static inline int poll_start(armice_ctrl_t * ctrl)
{
	if (!ctrl->poll.enabled) {
		DCC_LOG(LOG_TRACE, "start polling...");
		/* sync */
		if (uthread_mutex_timedlock(ctrl->poll_mutex, 10) < 0) {
			DCC_LOG(LOG_WARNING, "mutex lock timeout!");
			return JTAG_ERR_ARM_POLL_STOP;
		}
		ctrl->poll.enabled  = 1;
		uthread_cond_signal(ctrl->poll_cond);
		/* release mutex */
		uthread_mutex_unlock(ctrl->poll_mutex);
	}
	return 0;
}
#endif


#if 0
int armice_insn_show(armice_ctrl_t * ctrl, uint32_t addr,
					 struct armice_insn * insn,  FILE * f)
{
	if (insn->thumb)
		thumb_show_asm(f, addr, insn->data);
	else
		arm_show_asm(f, addr, insn->data);

	fprintf(f, "\n");

	return ICE_OK;
}
#endif
