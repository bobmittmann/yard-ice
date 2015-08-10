/* 
 * Copyright(C) 2015 Robinson Mittmann. All Rights Reserved.
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
 * @file mstp_lnk.h
 * @brief MST/TP link layer public header
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __MSTP_LNK_H__
#define __MSTP_LNK_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/serial.h>

/** @mainpage MS/TP Data Link Layer Library
 * 
 *
 * This is an implementation of the @b MS/TP @b Data @b Link @b Layer as 
 * defined in the @b ANSI/ASHRAE @b Standard @b 135-2004 clause 9.
 * 
 * "9 DATA LINK/PHYSICAL LAYERS: MASTER-SLAVE/TOKEN PASSING (MS/TP) LAN
 *
 *
 * This clause describes a Master-Slave/Token-Passing (MS/TP) data link 
 * protocol, which provides the same services to the network layer as ISO 
 * 8802-2 Logical Link Control. It uses services provided by the EIA-485 
 * physical layer. Relevant clauses of EIA-485 are deemed to be included 
 * in this standard by reference. The following hardware is assumed:
 *
 * (a) A UART (Universal Asynchronous Receiver/Transmitter) capable of 
 * transmitting and receiving eight data bits with one stop bit and no parity.
 *
 * (b) An EIA-485 transceiver whose driver may be disabled.
 *
 * (c) A timer with a resolution of five milliseconds or less."
 * 
 */

/** @struct mstp_frame_inf
 *  @brief MS/TP frame info.
 *
 * This strucutre is used by the @c mstp_lnk_send() and @c mstp_lnk_recv()
 * in addition to the payload.
 */
struct mstp_frame_inf {
	uint8_t type;  /**< frame type */
	uint8_t daddr; /**< destination address */
	uint8_t saddr; /**< source address */
};

struct mstp_lnk_stat {
	uint32_t rx_err;
	uint32_t rx_token;
	uint32_t rx_mgmt;
	uint32_t rx_unicast;
	uint32_t rx_bcast;
	uint32_t tx_token;
	uint32_t tx_mgmt;
	uint32_t tx_unicast;
	uint32_t tx_bcast;
	uint32_t token_lost;
};

/** @enum mstp_frame_type 
 * @brief MSTP Frame Types 
 *
 */
enum mstp_frame_type {
	FRM_TOKEN                  = 0x00,
	FRM_POLL_FOR_MASTER        = 0x01,
	FRM_REPLY_POLL_FOR_MASTER  = 0x02, 
	FRM_TEST_REQUEST           = 0x03,
	FRM_TEST_RESPONSE          = 0x04,
	FRM_BACNET_DATA_XPCT_REPLY = 0x05,
	FRM_BACNET_DATA_NO_REPLY   = 0x06,
	FRM_REPLY_POSTPONED        = 0x07,
	FRM_DATA_XPCT_REPLY        = 0x80,
	FRM_DATA_NO_REPLY          = 0xc0
};

/** @enum mstp_frame_type
 * @brief MSTP Frame Types
 *
 */
enum mstp_mgmt_event {
	MSTP_EV_SOLE_MASTER = 0,
	MSTP_EV_MULTIMASTER,
	MSTP_EV_TOKEN_LOST
};

/** @def MSTP_ADDR_BCAST
 * @brief MSTP broadcast address
 *
 */
#define MSTP_ADDR_BCAST 255
 
/** @struct mstp_lnk
 *  @brief MS/TP link control strcuture.
 *
 * An opaque structure representing a MS/TP link control device.
 */
struct mstp_lnk;

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup linkmgmt MS/TP link management
 *
 * This group of functions are used to allocate, initialize,
 * and manage SM/TP links.
 *
 * @{
 */

/** @brief Alloc a MS/TP link control structure
 *
 * The link control structure is allocated from a pool of resources.
 * The link control structure should be initialized by calling the
 * @c mstp_lnk_init() fucntion;
 * Subsequent MS/TP link operations will be performed by referencing 
 * the returning pointer.
 *
 * @return On success, a pointer to a MS/TP link control structure is
 * returned. On error @b NULL is returned.
 */
struct mstp_lnk * mstp_lnk_alloc(void);

/** @brief Initialize a MS/TP link control structure
 *
 * This function binds a MS/TP link control structure with a name,
 * an address and a serial device.
 *
 * @param lnk The MS/TP link control structure.
 * @param name String naming this link.
 * @param addr The MAC address for this link.
 * @param dev A serial device driver.
 * @return On success, 0 is returned. On error #-1 is returned.
 */
int mstp_lnk_init(struct mstp_lnk * lnk, const char * name, 
				  unsigned int addr, struct serial_dev * dev);

/** @brief Start sending and receiving data on MS/TP link.
 *
 * @param lnk The MS/TP link control structure.
 * @return On success, 0 is returned. On error #-1 is returned.
 */
int mstp_lnk_resume(struct mstp_lnk * lnk);

/** @brief Start sending/receiving data on MS/TP link.
 *
 * @param lnk The MS/TP link control structure.
 * @return On success, 0 is returned. On error #-1 is returned.
 */
int mstp_lnk_stop(struct mstp_lnk * lnk);

/** @brief  MS/TP link processing loop.
 *
 * This function receives frames from the serial interface, processes 
 * them as appropriate and sends back responses, and tokens.
 * It mantains the link.
 * A thread should be reserved esclusivelly to call this function as it
 * will never returns.
 *
 * @param lnk The MS/TP link control structure.
 */
void mstp_lnk_loop(struct mstp_lnk * lnk);

int mstp_lnk_getaddr(struct mstp_lnk * lnk);

int mstp_lnk_getbcast(struct mstp_lnk * lnk);

/** @brief  MS/TP link management callback registering.
 *
 * This function assigns a function to be called whenever an management event
 *
 * The callback function should be fast to avoid interference with the link
 * control.
 *
 * @param lnk The MS/TP link control structure.
 * @param cbk Callback function.
 */
int mstp_lnk_mgmt(struct mstp_lnk * lnk,
		void (*cbk)(struct mstp_lnk *, unsigned int));

int mstp_lnk_getstat(struct mstp_lnk * lnk,
		struct mstp_lnk_stat * stat, bool reset);

int mstp_lnk_getnetmap(struct mstp_lnk * lnk, uint8_t map[], unsigned int max);

/**@}*/




/** @defgroup linkio MS/TP link data sending/receiving 
 * This group of functions are used to send and receive
 * data trough the SM/TP links.
 *
 * The link should be previouslly initialized by calling the
 * appropriated link management calls. Also the link state machine
 * must be running by calling @c mstp_lnk_loop().
 * @{
 */

/** @brief MS/TP link frame send.
 *
 * This function sends a frames on a SM/TP link.
 *
 * @param lnk The MS/TP link control structure.
 * @param buf Pointer to the frame payload.
 * @param count Number of bytes to send.
 * @param inf Pointer to a frame info strucutre containing the destination
 * address and the frame type.
 * @return On success, the number of bytes sent is returned.
 * On error @b -1 is returned.
 */

int mstp_lnk_send(struct mstp_lnk * lnk, const void * buf,
				  unsigned int count, const struct mstp_frame_inf * inf);

/** @brief MS/TP link frame receive.
 *
 * This function receives a frames on a SM/TP link.
 *
 * @param lnk The MS/TP link control structure.
 * @param buf Pointer to the frame payload receiving bufffer.
 * @param count Maximum number of bytes to be recevied.
 * @param inf Pointer to a frame info strucutre containing the source 
 * address and the frame type.
 * @return On success, the number of bytes received is returned.
 * On error @b -1 is returned.
 */

int mstp_lnk_recv(struct mstp_lnk * lnk, void * buf, 
				  unsigned int count, struct mstp_frame_inf * inf);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* __MSTP_LNK_H__ */

