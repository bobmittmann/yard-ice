/* FSSL internal (private) header file */

#ifndef __FSLL_I_H__
#define __FSLL_I_H__

#ifndef __FSLL_I__
#error "Never use <fssl-i.h> directly; include <fssl.h> instead."
#endif 

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define    MIN(a,b)    (((a)<(b))?(a):(b))
#define    MAX(a,b)    (((a)>(b))?(a):(b))
#else
#include <sys/param.h>
#endif

#include <fsll.h>
#include "mempool.h"
#include "fifo.h"
#include "crc.h"

#define FSLL_FRAME_DATA_MAX 511 
#define FSLL_HEADER_LEN     4
#define FSLL_FRAME_LEN_MAX  (FSLL_HEADER_LEN + FSLL_FRAME_DATA_MAX)
#define FSLL_SOF            0x55
#define FSLL_S_FLAG         (1 << 7)
#define FSLL_CTRL_FLAG      (1 << 8)
/* Broadcast address */
#define FSLL_ADDR_BCAST     0
#define FSLL_ADDR_MAX       64

#define FSLL_CTRL_POLL      0
#define FSLL_CTRL_ENABLE    1
#define FSLL_CTRL_DESC_REQ  2
#define FSLL_CTRL_DISCOVER  3
#define FSLL_CTRL_DISABLE   4

#define FSLL_MAC_ERR_MAX    5

/* FSLL Frame Header */
struct fsll_hdr {
	uint32_t sof: 8;
	uint32_t len: 9;
	uint32_t addr: 7;
	uint32_t svc: 3;
	uint32_t crc: 5;
};

/* FSLL Link Control Frame Header */
struct fsll_lc {
	uint32_t sof: 8;
	uint32_t len: 8;
	uint32_t opt: 1;
	uint32_t addr: 7;
	uint32_t svc: 3;
	uint32_t crc: 5;
};

struct fsll_frame {
	union {
		struct {
			struct fsll_hdr hdr;
			uint8_t data[FSLL_FRAME_DATA_MAX];
		};
		uint8_t raw[FSLL_FRAME_LEN_MAX];
	};
};

/*****************************************************************************
 * MAC layer framming
 ****************************************************************************/

static inline void fsll_mk_m_hdr(void * hdr, uint16_t ctrl, 
								 uint8_t addr, uint8_t svc)
{
	uint8_t * buf = (uint8_t *)hdr;
	unsigned int crc;

	buf[0] = FSLL_SOF;
	buf[1] = ctrl;
	buf[2] = ((ctrl >> 8) & 1) | ((addr << 1) & 0x7e);
	buf[3] = svc & 0x07;
	crc = crc5(&buf[1], 3);
	buf[3] |= crc << 3;
}

static inline void fsll_mk_s_hdr(void * hdr, uint16_t ctrl, 
								 uint8_t addr, uint8_t svc)
{
	uint8_t * buf = (uint8_t *)hdr;
	unsigned int crc;

	buf[0] = FSLL_SOF;
	buf[1] = ctrl;
	buf[2] = ((ctrl >> 8) & 1) | ((addr << 1) & 0x7e) | FSLL_S_FLAG;
	buf[3] = svc & 0x07;
	crc = crc5(&buf[1], 3);
	buf[3] |= crc << 3;
}

/*****************************************************************************
 * LLC (service) layer 
 ****************************************************************************/

struct fsll_svc {
	struct fsll * ll;
	volatile bool active;
	uint8_t num;
	struct {
		struct fifo * q;
//		sem_t sem;
	} rcv;
	struct {
		uint64_t pdu_rcv;
		uint64_t pdu_xmt;
	} stat;
};	

struct fsll;

enum {
	FSLL_OPER_NODE_DISABLE = 0,
	FSLL_OPER_NODE_ENABLE = 1
};

/* FSSL operations */
struct fsll_oper {
	int (* open)(struct fsll *);
	int (* close)(struct fsll *);
	bool (* frm_send)(struct fsll *, struct fsll_frame *);
	struct fsll_svc * (* svc_open)(struct fsll *, unsigned int, unsigned int);
	int (* ctrl)(struct fsll *, unsigned int, 
						unsigned int, void *);
};

/* FSSL base (abstract) type (class) */
struct fsll {
	struct serial_dev * dev;
	struct fsll_oper oper;
	volatile bool master;
	struct {
		volatile uint8_t addr;
		uint16_t mtu;
	} mac;

	struct {
//		sem_t sem;
		bool enabled;
		uint32_t timeout;
		volatile uint32_t head;
		volatile uint32_t tail;
		/* Warning: this is a volatile pointer. I.E. the pointer
		   can not be chached in a thread local variable */
		struct fsll_frame * volatile frm;
		struct {
			unsigned int err_ovr;
		} stat;
	} rx;

	struct {
//		pthread_mutex_t mutex;
		volatile bool pending;
	} tx;

//	pthread_cond_t frm_cond;
//	pthread_mutex_t frm_mutex;
	struct mempool * frm_pool;

	struct fsll_svc svc[8];
};

/*****************************************************************************
 * Management PDU strcucures 
 ****************************************************************************/

#define FSLL_MGM_HDR_SIZE 4
#define FSLL_MGM_OPC_TIME 1
#define FSLL_MGM_OPC_DESC 2

struct fsll_mgm_hdr {
	uint8_t crc;
	uint8_t opc;
	uint16_t seq;
};

struct fsll_mgm_pdu {
	struct fsll_mgm_hdr hdr;
	uint32_t data[];
};

struct fsll_mgm_node_desc_pdu {
	struct fsll_mgm_hdr hdr;
	struct fsll_node_desc desc;
};

struct fsll_mgm_time_pdu {
	struct fsll_mgm_hdr hdr;
	uint32_t sec;
	uint32_t frac;
};


/*****************************************************************************
 * MAC layer interface helpers
 ****************************************************************************/

/* Wait from a frame from the MAC layer.
   Returns a pointer to the frame. */
static inline struct fsll_frame * __fsll_mac_frm_wait(struct fsll * ll)
{
	unsigned int tail = ll->rx.tail;
	struct fsll_frame * frm;

	if (!ll->rx.enabled)
		return NULL;

//	sem_wait(&ll->rx.sem);

	if (ll->rx.head == tail) {
		/* This should never happen!! */
		return NULL;
	}

	frm = ll->rx.frm;
	/* Increment the tail counter */
	ll->rx.tail = tail + 1;

	return frm;
}

/*****************************************************************************
 * LLC layer interface helpers
 ****************************************************************************/

static inline struct fsll_frame * __fsll_frm_alloc(struct fsll * ll)
{
	return memblk_alloc(ll->frm_pool);
}

static inline bool __fsll_frm_free(struct fsll * ll, struct fsll_frame * frm)
{
	return memblk_free(ll->frm_pool, frm);
}

static inline void __fsll_frm_incref(struct fsll_frame * frm)
{
	memblk_incref(frm);
}

static inline void __fsll_frm_decref(struct fsll_frame * frm)
{
	memblk_decref(frm);
}

static inline bool __fsll_frm_enqueue(struct fifo * q, struct fsll_frame * frm)
{
	return fifo_enqueue(q, (void *)&frm);
}

static inline struct fsll_frame * __fsll_frm_dequeue(struct fifo * q) 
{
	struct fsll_frame * frm = NULL;

	fifo_dequeue(q, &frm);

	return frm;
}

#ifdef __cplusplus
extern "C" {
#endif

void fsll_frm_pool_init(void);

int __fsll_init(struct fsll * ll, unsigned int tmo_ms, unsigned int mtu);

#ifdef __cplusplus
}
#endif	


/*****************************************************************************
 * Node 
 ****************************************************************************/

enum {
	FSLL_NODE_INACTIVE = 0,
	FSLL_NODE_ACTIVE = 1,
	FSLL_NODE_ENABLED = 2
};

#define FSLL_STATUS_ENABLED (1 << 0)

struct fsll_node {
	uint8_t addr;
	volatile uint8_t state;
	uint8_t error;
	uint8_t status;
	struct fsll_node_desc desc;
	struct {
		volatile uint32_t head;
		volatile uint32_t tail;
		struct fsll_frame * frm[8];
//		pthread_cond_t cond;
//		pthread_mutex_t mutex;
	} xmt;
	struct {
		uint64_t frm_rcvd;
		uint64_t frm_xmtd;
	} stat;
};

struct node_list {
	unsigned int cnt;
	struct fsll_node * node[FSLL_ADDR_MAX];
};

#ifdef __cplusplus
extern "C" {
#endif

int node_next(struct node_list * lst, unsigned int addr);

int node_search(struct node_list * lst, unsigned int addr);

void node_delete(struct node_list * lst, unsigned int idx);

void node_insert(struct node_list * lst, struct fsll_node * node);

struct fsll_node * node_remove(struct node_list * lst, unsigned int addr);

void node_list_dump(FILE * f,  struct node_list * lst);

void node_flush_queues(struct fsll * ll, struct fsll_node * node);

void node_init(struct fsll_node * node, unsigned int addr);

bool node_xmt_enqueue(struct fsll_node * node, struct fsll_frame * frm);

struct fsll_frame * node_xmt_dequeue(struct fsll_node * node);

bool node_frm_enqueue(struct fsll_node * node, struct fsll_frame * frm, 
					  size_t len);

void fsll_mac_send(struct fsll * ll, const void * buf, unsigned int len);

#ifdef __cplusplus
}
#endif	

#endif /* __FSLL_I_H__ */

