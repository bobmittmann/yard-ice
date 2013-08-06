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
 * @file errno.h
 * @brief YARD-ICE libc
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __ERRNO_H__
#define __ERRNO_H__

#define	EPERM 1
#define	ENOENT 2
#define	ESRCH 3
#define	EINTR 4
#define	EIO 5
#define	EBADF 6
#define	ENOMEM 7
#define	EACCES 8
#define	EFAULT 9
#define	EBUSY 10
#define	EEXIST 11
#define	ENODEV 12
#define	ENOTDIR 13
#define	EISDIR 14
#define	EINVAL 15
#define	EDEADLK 16
#define	ENOLCK 17
#define	EAGAIN 18
#define ENOSYS 19
#define ETIMEDOUT 20
#define EADDRINUSE 21
#define ENOTCONN 22
#define ECONNREFUSED 23
#define EDESTADDRREQ 24
#define EADDRNOTAVAIL 25
#define ENOTSOCK 26
#define EHOSTUNREACH 27

#ifndef __ASSEMBLER__

extern int errno;

#endif /* __ASSEMBLER__ */

#endif /* __ERRNO_H__ */

