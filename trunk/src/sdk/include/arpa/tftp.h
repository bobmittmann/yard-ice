/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file arpa/tftp.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __ARPA_TFTP_H__
#define	__ARPA_TFTP_H__

#include <stdint.h>

/*
 * Trivial File Transfer Protocol (IEN-133)
 */

/* data segment size */
#define	TFTP_SEGSIZE 512		

/*
 * Packet types.
 */
#define TFTP_RRQ   1 /* read request */
#define TFTP_WRQ   2 /* write request */
#define TFTP_DATA  3 /* data packet */
#define TFTP_ACK   4 /* acknowledgement */
#define TFTP_ERROR 5 /* error code */

#define TFTP_OACK  6 /* RFC 2347 - TFTP Option */

struct tftphdr {
	uint16_t th_opcode; /* packet type */
	uint16_t th_code;   /* error code / block number */
	uint8_t th_data[0]; /* data or error string */
};

/* block # */
#define	th_block    th_code
/* request packet stuff */
#define	th_stuff    th_code
#define	th_msg      th_data

/*
 * Error codes.
 */
#define TFTP_EUNDEF    0 /* not defined */
#define TFTP_ENOTFOUND 1 /* file not found */
#define TFTP_EACCESS   2 /* access violation */
#define TFTP_ENOSPACE  3 /* disk full or allocation exceeded */
#define TFTP_EBADOP    4 /* illegal TFTP operation */
#define TFTP_EBADID    5 /* unknown transfer ID */
#define TFTP_EEXISTS   6 /* file already exists */
#define TFTP_ENOUSER   7 /* no such user */

#define TFTP_ERROPTION 8 /* RFC 2347 - TFTP Option */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ARPA_TFTP_H__ */
