/* $Id: pcb.h,v 2.3 2008/06/04 00:03:14 bob Exp $ 
 *
 * File:	tcpip/pcb.h
 * Module:	
 * Project:
 * Author:	Robinson Mittmann (bob@cnxtech.com)
 * Target:	arm7tdmi
 * Comment:	NOPS Internet Protocol Control Blocks
 * Copyright(c) 2003,2004 CNX Technologies. All Rights Reserved.
 *
 */

#ifndef __SYS_PCB_H__
#define __SYS_PCB_H__

#include <stdint.h>

#include <tcpip/in.h> /* in_addr_t */
#include <tcpip/route.h> /* struct ip_route */

#include <sys/mbuf.h>

/* Protocol Control Block */
struct pcb {
	/* foreign address */
	in_addr_t inp_faddr;
	/* local address */
	in_addr_t inp_laddr;
	union {
		struct {
			/* foreign port */
			uint16_t inp_fport;
			/* local port */
			uint16_t inp_lport;
		};
		uint32_t inp_ports;
	};
};

struct pcb_link {
	struct pcb_link * next;
	struct pcb pcb;
};

struct pcb_list {
	struct pcb_link * first;
	struct pcb_link * last;
};

#define INPF_STREAM       0x10
#define INPF_NRD          0x20
#define INPF_NWR          0x40
#define INPF_NBIO         0x80

#ifdef __cplusplus
extern "C" {
#endif

void pcb_list_init(struct pcb_list * __list);

struct pcb * pcb_alloc(void);

int pcb_free(struct pcb * __inp);

int pcb_bind(struct pcb * __inp, in_addr_t __addr, uint16_t __port);

int pcb_connect(struct pcb * __inp, in_addr_t __addr, uint16_t __port);

int pcb_enum(int (* __callback)(struct pcb *, void *),
			   void * __parm, struct pcb_list * __list);

void pcb_insert(struct pcb * __inp, struct pcb_list * __list);
int pcb_remove(struct pcb * __inp, struct pcb_list * __list);
int pcb_release(struct pcb * __inp, struct pcb_list * __list);
int pcb_move(struct pcb * __inp, struct pcb_list * __from, struct pcb_list * __to);

int pcb_find(struct pcb * __inp, struct pcb_list * __list);

struct pcb * pcb_wildlookup(in_addr_t __faddr, uint16_t __fport, 
	in_addr_t __laddr, uint16_t __lport, struct pcb_list * __list);

struct pcb * pcb_lookup(in_addr_t __faddr, uint16_t __fport, 
	in_addr_t __laddr, uint16_t __lport, struct pcb_list * __list);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_PCB_H__ */

