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

#define EPERM           1    /* Operation not permitted */
#define ENOENT          2    /* No such file or directory */
#define ESRCH           3    /* No such process */
#define EINTR           4    /* Interrupted system call */
#define EIO             5    /* I/O error */
#define ENXIO           6    /* No such device or address */
#define E2BIG           7    /* Argument list too long */
#define ENOEXEC         8    /* Exec format error */
#define EBADF           9    /* Bad file number */
#define ECHILD         10    /* No child processes */
#define EAGAIN         11    /* Try again */
#define ENOMEM         12    /* Out of memory */
#define EACCES         13    /* Permission denied */
#define EFAULT         14    /* Bad address */
#define ENOTBLK        15    /* Block device required */
#define EBUSY          16    /* Device or resource busy */
#define EEXIST         17    /* File exists */
#define EXDEV          18    /* Cross-device link */
#define ENODEV         19    /* No such device */
#define ENOTDIR        20    /* Not a directory */
#define EISDIR         21    /* Is a directory */
#define EINVAL         22    /* Invalid argument */
#define ENFILE         23    /* File table overflow */
#define EMFILE         24    /* Too many open files */
#define ENOTTY         25    /* Not a typewriter */
#define ETXTBSY        26    /* Text file busy */
#define EFBIG          27    /* File too large */
#define ENOSPC         28    /* No space left on device */
#define ESPIPE         29    /* Illegal seek */
#define EROFS          30    /* Read-only file system */
#define EMLINK         31    /* Too many links */
#define EPIPE          32    /* Broken pipe */
#define EDOM           33    /* Math argument out of domain of func */
#define ERANGE         34    /* Math result not representable */

#define EDEADLK        35    /* Resource deadlock would occur */
#define ENAMETOOLONG   36    /* File name too long */
#define ENOLCK         37    /* No record locks available */
#define ENOSYS         38    /* Function not implemented */
#define ENOTEMPTY      39    /* Directory not empty */

#define EBADMSG        74    /* Not a data message */
#define EILSEQ         84    /* Illegal byte sequence */
#define EMSGSIZE       90    /* Message too long */
#define ENOTSOCK       88    /* Socket operation on non-socket */
#define EDESTADDRREQ   89    /* Destination address required */
#define EADDRINUSE     98    /* Address already in use */
#define EADDRNOTAVAIL  99    /* Cannot assign requested address */
#define ENOTCONN      107    /* Transport endpoint is not connected */
#define ETIMEDOUT     110    /* Connection timed out */
#define ECONNREFUSED  111    /* Connection refused */
#define EHOSTUNREACH  113    /* No route to host */
#define EINPROGRESS   115    /* Operation now in progress */
#define ECANCELED     125    /* Operation Canceled */

#ifndef __ASSEMBLER__

extern int errno;

#endif /* __ASSEMBLER__ */

#endif /* __ERRNO_H__ */

