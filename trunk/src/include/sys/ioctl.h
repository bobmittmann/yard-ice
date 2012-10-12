/* $Id: ioctl.h,v 2.1 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	sys/ioctl.h
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com)
 * Target:
 * Comment:
 * Copyright(c) 2008 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifndef __SYS_IOCTL_H__
#define __SYS_IOCTL_H__

#define FIONBIO 0x5421

#ifdef __cplusplus
extern "C" {
#endif

int ioctl(int __fd, unsigned long int __request, ...);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_IOCTL_H__ */

