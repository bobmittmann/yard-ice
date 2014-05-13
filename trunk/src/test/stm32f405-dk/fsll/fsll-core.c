#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <fsll.h>

#define __FSLL_I__
#include "fsll-i.h"

/***************************************************************************** 
   Link Layer MAC 
 *****************************************************************************/

/* Link Layer receiving state machine states */
enum {
	WAIT_SOF = 0,
	WAIT_HDR1,
	WAIT_HDR2,
	WAIT_HDR3,
	RECV_DATA
};

void fsll_mac_send(struct fsll * ll, const void * buf, unsigned int len)
{
//	pthread_mutex_lock(&ll->tx.mutex);
	ll->tx.pending = true;
//	serial_write(ll->dev, buf, len);
	ll->tx.pending = false;
//	pthread_mutex_unlock(&ll->tx.mutex);
}

void fsll_mac_rx_task(struct fsll * ll)
{
	struct serial_dev * dev = ll->dev;
	int32_t wait_tmo = ll->rx.timeout;
	unsigned int state;
	unsigned int crc;
	unsigned int cnt = 0;
	unsigned int len = 0;
	struct fsll_frame * frm;
	int c;

	state = WAIT_SOF;

	frm = __fsll_frm_alloc(ll);
#if 0
	fprintf(stderr, "%s:() frm=%p\n", __func__, frm);
	fflush(stderr);
#endif

	while (ll->rx.enabled) {

//		c = serial_getchar(dev, wait_tmo);
		c = 0;

		if (c == SERIAL_ERR) {
//			TRACEF(LINK|ERR, "serial_getchar() failed!");
			fprintf(stderr, "[ERR]");
			fflush(stderr);
			break;
		}

		if (c == SERIAL_TMO) {
			if (ll->tx.pending) {
#if 0
				fprintf(stderr, "[BUSY]");
				fflush(stderr);
#endif
		//		pthread_mutex_lock(&ll->tx.mutex);
		//		pthread_mutex_unlock(&ll->tx.mutex);
				continue;
			}
//			TRACEF(LINK_STAT|DBG, "[TMO]");
#if 0
			fprintf(stderr, "[TMO]\n");
			fflush(stderr);
#endif
			/* Placeholder for timeout */
			ll->rx.frm = NULL;
			sem_post(&ll->rx.sem);
			state = WAIT_SOF;
			continue;
		}

		switch (state) {

		case WAIT_SOF:
			if (c == FSLL_SOF) {
				if (ll->rx.head != ll->rx.tail) {
					ll->rx.stat.err_ovr++;
#if 1
					fprintf(stderr, "[%c%02x: OVR] head=%d tail=%d\n", 
							ll->master ? 'M' : 'S',
							ll->mac.addr, ll->rx.head, ll->rx.tail);
					fflush(stderr);
#endif
				} else {
					state = WAIT_HDR1;
#if 0
					fprintf(stderr, "[SOF]");
					fflush(stderr);
#endif
				}
//				TRACEF(LINK_STAT|DBG, "[SOF]");
			}
			break;;

		case WAIT_HDR1:
			frm->raw[1] = c;
			state = WAIT_HDR2;
			break;

		case WAIT_HDR2:
			frm->raw[2] = c;
			state = WAIT_HDR3;
			continue;

		case WAIT_HDR3:
			crc = c >> 3;
			frm->raw[3] = c & 0x07;

			/* FIXME: inline this CRC check to improve performance */
			if (crc != crc5(&frm->raw[1], 3)) {
				fprintf(stderr, "[CRC]\n");
				fflush(stderr);

				/* CRC failed, this means that either we got a corrupted 
				   package or it was not a valid frame header.
				   Try to find a SOF in the remeaining bytes of the 
				   header and restart the state machine. */
				if (frm->raw[1] == FSLL_SOF) {
					frm->raw[1] = frm->raw[2];
					frm->raw[2] = c;
				} else if (frm->raw[2] == FSLL_SOF) {
					frm->raw[1] = c;
					state = WAIT_HDR2;
				} else if (c == FSLL_SOF) {
					state = WAIT_HDR1;
				} else {
					/* No SOF in the header, restart from the beginning. */
					state = WAIT_SOF;
				}
				break;
			}

#if 0
			fprintf(stderr, "[HDR: %02x.%d, %d]", frm->hdr.addr, 
					frm->hdr.svc, frm->hdr.len);
			fflush(stderr);
#endif

			if ((frm->hdr.len == 0) || ((frm->hdr.svc == 0) && 
										(frm->hdr.len > 255))) {
				/* Packets from service access point 0 are
				   treated different from other services. */
				/* The bit 8 of header length identify link layer
				   control frames with no extra payload. If this bit 
				   is clear the frames are treated as any other regular 
				   service. */
				goto frm_rcvd;
			} else {
				/* The frame has a data payload. */
				len = frm->hdr.len;
				cnt = 0;
				state = RECV_DATA;
			}
			break;

		case RECV_DATA:
			frm->data[cnt++] = c;
			if (cnt == len) {
frm_rcvd:
				/* A frame was received. Check whether discard or deliver it. */
				/* Broadcast frames are allways deliverd.
				   Unicast frames must match the MAC address.
				   Master node should set the MAC address to the
				   node whose reply is expected from. */
				if ((frm->hdr.addr == FSLL_ADDR_BCAST) || 
					(frm->hdr.addr == ll->mac.addr)) {
					struct fsll_frame * new_frm;
					/* Only deliver the frame if we can allocate a new one. 
					   This is to avoid locking the MAC layer. */
					if ((new_frm = __fsll_frm_alloc(ll)) != NULL) {
						ll->rx.head++;
						/* set the rx frame pointer */
						ll->rx.frm = frm;
						/* signal the decoding thread */	
						sem_post(&ll->rx.sem);
						/* use the newly allocated frame */
						frm = new_frm;
					} else {
						fprintf(stderr, "[MEM!]\n");
						fflush(stderr);
					}
				} else {
#if 0
					fprintf(stderr, "[msg=%02x mac=%02x discarded]\n", 
							frm->hdr.addr, ll->mac.addr);
					fflush(stderr);
#endif
				}
				state = WAIT_SOF;
			}
			break;
		}
	}
}


int fsll_attach(struct fsll * ll, struct serial_dev * dev)
{
	int ret;
	pthread_t thread;

	assert(ll != NULL);
	assert(dev != NULL);

	ll->dev = dev;

	ll->rx.enabled = true;

	if ((ret = pthread_create(&thread, NULL, 
							  (void * (*)(void *))fsll_mac_rx_task, 
							  (void *)ll)) < 0) {
		fprintf(stderr, "%s: pthread_create() failed: %s", 
				__func__, strerror(ret));
	}

	return ll->oper.open(ll);
}

int fsll_close(struct fsll * ll)
{
	assert(ll != NULL);
	assert(ll->dev != NULL);

	return ll->oper.close(ll);
}

struct fsll_frame * fsll_frm_try_alloc(struct fsll * ll, int svcnum)
{
	struct fsll_frame * frm;

	assert(ll != NULL);

	if ((frm = __fsll_frm_alloc(ll)) != NULL) {
		/* clear the frame header */
		memset(&frm->hdr, 0, sizeof(struct fsll_hdr));

		/* partially initialize the frame header */
		frm->hdr.sof = FSLL_SOF;
		frm->hdr.svc = svcnum;
		/* Set the S bit for slaves and clear for master */
		frm->hdr.addr = ll->master ? 0 : (1 << 6);
	}

	return frm;
}

void fsll_frm_free(struct fsll * ll, struct fsll_frame * frm)
{
	assert(ll != NULL);
	assert(frm != NULL);

//	pthread_mutex_lock(&ll->frm_mutex);

	if (__fsll_frm_free(ll, frm)) {
//		pthread_cond_signal(&ll->frm_cond);
	}

//	pthread_mutex_unlock(&ll->frm_mutex);
}

int fsll_pdu_recv(struct fsll_svc * svc, uint8_t * addr, void ** pdu)
{
	struct fsll_frame * frm;

	assert(svc != NULL);
	assert(pdu != NULL);

#if 0
	fprintf(stderr, "%s: sem_wait()...\n", __func__);
	fflush(stderr);
#endif

	sem_wait(&svc->rcv.sem);

	frm = __fsll_frm_dequeue(svc->rcv.q);

#if 0
	fprintf(stderr, "%s: __fsll_frm_dequeue(): frm=%p\n", __func__, frm);
	fflush(stderr);
#endif

	assert(frm != NULL);

	*pdu = frm->data;

	if (addr != NULL)
		*addr = frm->hdr.addr & 0x3f;

	return frm->hdr.len;
}

int fsll_pdu_timedrecv(struct fsll_svc * svc, uint8_t * addr, void ** pdu, 
					   struct timespec * tm)
{
	struct fsll_frame * frm;

	assert(svc != NULL);
	assert(pdu != NULL);

#if 0
	fprintf(stderr, "%s: sem_wait()...\n", __func__);
	fflush(stderr);
#endif

	if (sem_timedwait(&svc->rcv.sem, tm) < 0)
		return FSLL_TMO;

	frm = __fsll_frm_dequeue(svc->rcv.q);

#if 0
	fprintf(stderr, "%s: __fsll_frm_dequeue(): frm=%p\n", __func__, frm);
	fflush(stderr);
#endif

	assert(frm != NULL);

	*pdu = frm->data;

	if (addr != NULL)
		*addr = frm->hdr.addr & 0x3f;

	return frm->hdr.len;
}


bool fsll_pdu_send(struct fsll_svc * svc,  uint8_t addr, 
				   const void * pdu, size_t len)
{
	struct fsll_hdr * ptr;
	struct fsll_frame * frm;

	assert(svc != NULL);
	assert(pdu != NULL);
	assert(len < 511);
	assert(addr < 63);

	ptr =(struct fsll_hdr *)pdu;
	/* get the frame from PDU */
	frm = (struct fsll_frame *)(ptr - 1);

	/* adjust header */
	frm->hdr.crc = 0;
	frm->hdr.addr |= addr;
	frm->hdr.len = len;
	/* calculate the CRC */
	frm->hdr.crc = crc5(&frm->raw[1], 3);

	return svc->ll->oper.frm_send(svc->ll, frm);
}

void fsll_pdu_pack(struct fsll_svc * svc,  uint8_t addr, 
				   const void * pdu, size_t len)
{
	struct fsll_hdr * ptr;
	struct fsll_frame * frm;

	assert(svc != NULL);
	assert(pdu != NULL);
	assert(len < 511);
	assert(addr < 63);

	ptr =(struct fsll_hdr *)pdu;
	/* get the frame from PDU */
	frm = (struct fsll_frame *)(ptr - 1);

	/* adjust header */
	frm->hdr.crc = 0;
	frm->hdr.addr |= addr;
	frm->hdr.len = len;
	/* calculate the CRC */
	frm->hdr.crc = crc5(&frm->raw[1], 3);
}

bool fsll_pdu_enqueue(struct fsll_svc * svc,  const void * pdu)
{
	struct fsll_hdr * ptr;
	struct fsll_frame * frm;

	assert(svc != NULL);
	assert(pdu != NULL);

	ptr =(struct fsll_hdr *)pdu;
	/* get the frame from PDU */
	frm = (struct fsll_frame *)(ptr - 1);

	return svc->ll->oper.frm_send(svc->ll, frm);
}

void * fsll_pdu_alloc(struct fsll_svc * svc)
{
	struct fsll_frame * frm;
	struct fsll * ll;

	assert(svc != NULL);

	ll = svc->ll;

//	pthread_mutex_lock(&ll->frm_mutex);

	while ((frm = __fsll_frm_alloc(ll)) == NULL) {
		pthread_cond_wait(&ll->frm_cond, &ll->frm_mutex);
	}

	pthread_mutex_unlock(&ll->frm_mutex);

	/* clear the frame header */
	memset(&frm->hdr, 0, sizeof(struct fsll_hdr));

	/* partially initialize the frame header */
	frm->hdr.sof = FSLL_SOF;
	frm->hdr.svc = svc->num;
	/* Set the S bit for slaves and clear for master */
	frm->hdr.addr = ll->master ? 0 : (1 << 6);

	/* return a pointer to the upper layer PDU section */
	return frm->data;
}

void fsll_pdu_incref(void * pdu)
{
	struct fsll_frame * frm;

	assert(pdu != NULL);

	frm = (struct fsll_frame *)((struct fsll_hdr *)pdu - 1);

	__fsll_frm_incref(frm);
}

void fsll_pdu_decref(void * pdu)
{
	struct fsll_frame * frm;

	assert(pdu != NULL);

	frm = (struct fsll_frame *)((struct fsll_hdr *)pdu - 1);

	__fsll_frm_decref(frm);
}

size_t fsll_pdu_len(void * pdu)
{
	struct fsll_frame * frm;

	assert(pdu != NULL);

	frm = (struct fsll_frame *)((struct fsll_hdr *)pdu - 1);

	return frm->hdr.len;
}


void fsll_pdu_free(struct fsll_svc * svc, void * pdu)
{
	struct fsll_hdr * ptr;
	struct fsll_frame * frm;
	struct fsll * ll;

	assert(svc != NULL);
	ll = svc->ll;

	assert(pdu != NULL);

	ptr =(struct fsll_hdr *)pdu;
	frm = (struct fsll_frame *)(ptr - 1);

#if 0
	fprintf(stderr, "%s(): frm=%p.\n", __func__, frm);
	fflush(stderr);
#endif

//	pthread_mutex_lock(&ll->frm_mutex);

	if (__fsll_frm_free(ll, frm)) {
//		pthread_cond_signal(&ll->frm_cond);
	}

//	pthread_mutex_unlock(&ll->frm_mutex);
}

/* 
FIXME Naming consistency: endpoint, access point (this sounds like 
wireless stuf), service point...

   Open a link layer service access point. This will activate the access point.
   The address (addr) field specifies a filter for frames based on the 
   node adress. A value of zero mean that frames from any address will be 
   queued in this access point.
XXX: This call could be more approprieted named, like endpoint_open()...

   = Addr != 0 =
   == Master ==	
   The master will filter the receving frames based on the 'addr' field'.
   Only this ispecific address is allowed.
   == Slave ==	
   The only valid address is its own MAC address. Other address are ignored.
   
   = Addr == 0 =
   == Master ==	
   The master will receive frames from any slave.
   == Slave ==	
   The slave will receive unicast as well as broadcast frames.

 */

struct fsll_svc * __fsll_svc_open(struct fsll * ll, unsigned int num,
								  unsigned int addr)
{
	struct fsll_svc * svc;

	assert(ll != NULL);
	assert(num < 8);

	svc = &ll->svc[num];

	assert(!svc->active);

	svc->active = true;

	return svc;

}

struct fsll_svc * fsll_svc_open(struct fsll * ll, unsigned int num,
								  unsigned int addr)
{
	struct fsll_svc * svc;

	assert(ll != NULL);
	assert(num < 8);
	assert(addr < 64);

	svc = ll->oper.svc_open(ll, num, addr);

	assert(!svc->active);

	svc->active = true;

	return svc;
}

void fsll_svc_close(struct fsll_svc * svc)
{
	struct fsll_frame * frm;

	assert(svc != NULL);
	assert(svc->active);

	svc->active = false;

	/* flush receiving queue */
	while (sem_trywait(&svc->rcv.sem) == 0) {
		frm = __fsll_frm_dequeue(svc->rcv.q);
		__fsll_frm_free(svc->ll, frm);
	}
}

/* XXX: service port or service number ??? */
int fsll_svc_port_get(struct fsll_svc * svc)
{
	assert(svc != NULL);
	return svc->num;
}

int fsll_node_enable(struct fsll * ll, unsigned int addr)
{
	assert(ll != NULL);
	return ll->oper.ctrl(ll, addr, FSLL_OPER_NODE_ENABLE, NULL);
}

int fsll_node_disable(struct fsll * ll, unsigned int addr)
{
	assert(ll != NULL);
	return ll->oper.ctrl(ll, addr, FSLL_OPER_NODE_DISABLE, NULL);
}

/* Common initializer for the link layer control object */
int __fsll_init(struct fsll * ll, unsigned int tmo_ms, unsigned int mtu)
{
	int i;

	assert(ll != NULL);

	/* Alloc a pool of frames */
	ll->frm_pool = mempool_alloc(32, FSLL_FRAME_LEN_MAX);
	pthread_mutex_init(&ll->frm_mutex, NULL);
	pthread_cond_init(&ll->frm_cond, NULL);

	/* initialize RX structure */
	sem_init(&ll->rx.sem, 0, 0);
	ll->rx.timeout = tmo_ms;
	ll->rx.enabled = false;
	ll->rx.head = 0;
	ll->rx.tail = 0;
	ll->rx.stat.err_ovr = 0;

	pthread_mutex_init(&ll->tx.mutex, NULL);

	/* initialize MAC structure */
	ll->mac.mtu = MIN(mtu, 511);

	fprintf(stderr, "%s: RX timeout= %d ms.\n", __func__, ll->rx.timeout);
	fprintf(stderr, "%s: MAC MTU= %d octets.\n", __func__, ll->mac.mtu);

	/* Initialize services */
	for (i = 0; i < 8; ++i) {
		struct fsll_svc * svc = &ll->svc[i];
		/* allocate a FIFo queue for each service */
		/* FIXME: optionally this could be initialized (allocated) when
		   a sevice is enabled */
		svc->rcv.q = fifo_queue_alloc(4, sizeof(void *));
		/* initialize the associated semaphore */
		sem_init(&svc->rcv.sem, 0, 0);
		/* set the service number */
		svc->num = i;
		/* set the link layer reference */
		svc->ll = ll;
		/* start disabled */
		svc->active = false;
	}

	return 0;
}

