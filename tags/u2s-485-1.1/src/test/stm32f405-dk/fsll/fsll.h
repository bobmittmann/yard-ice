#ifndef __FSLL_H__
#define __FSLL_H__

#include <stdbool.h>
#include <time.h>

/* Abstract Link layer control object */
struct fsll;

/* Link layer service access point */
struct fsll_svc;

/* Node descriptor */

#define FSLL_NODE_TYPE_LEN_MAX 8
#define FSLL_NODE_MODEL_LEN_MAX 12
#define FSLL_NODE_ESN_LEN_MAX 12
#define FSLL_NODE_FWVER_LEN_MAX 8

enum {
	FSLL_TMO = -2,
	FSLL_FAIL = -1,
	FSLL_OK = 0
};

struct fsll_node_desc {
	char type[FSLL_NODE_TYPE_LEN_MAX];
	char model[FSLL_NODE_MODEL_LEN_MAX];
	char esn[FSLL_NODE_ESN_LEN_MAX];
	char fwver[FSLL_NODE_FWVER_LEN_MAX];
};

#ifdef __cplusplus
extern "C" {
#endif

/* Create a new instance of a FSLL slave */
struct fsll * fsll_slave_create(unsigned int addr, 
								const struct fsll_node_desc * desc);

/* Create a new instance of a FSLL master */
struct fsll * fsll_master_create(unsigned int slave_max);

/* Try to allocate a frame */
struct fsll_frame * fsll_frm_try_alloc(struct fsll * ll, int svcnum);

/* Release an allocated a frame */
void fsll_frm_free(struct fsll * ll, struct fsll_frame * frm);

/* Open a link-layer service access point */
struct fsll_svc * fsll_svc_open(struct fsll * ll, unsigned int num, 
								unsigned int addr);

/* Close a link-layer serivce access point */
void fsll_svc_close(struct fsll_svc * svc);

/* Return the service number of a service object */
/* FIXME: inconsistent naming 'service number' <--> 'service port'. */
int fsll_svc_port_get(struct fsll_svc * svc);

/* Request the node description */
void fsll_node_info_get(struct fsll * ll, unsigned int addr, 
						struct fsll_node_desc * desc);

/*
   PDU (Protocol Data Unit) 
   From the point of view of a service access point, the PDU is a upper layer
   entity and it should be agnostic about its content. 
   The lenght of the PDU is the only thing that matters.
   PDUs are allowed to have zero length.
 */

/* Allocate a PDU */
void * fsll_pdu_alloc(struct fsll_svc * svc);

/* Increment the PDU reference counter */
void fsll_pdu_incref(void * pdu);

/* Decrement the PDU reference counter */
void fsll_pdu_decref(void * pdu);

/* Return the PDU lenght */
size_t fsll_pdu_len(void * pdu);

/* Release a PDU */
void fsll_pdu_free(struct fsll_svc * svc, void * pdu);

/* Dequeue a PDU from the service access point */
int fsll_pdu_recv(struct fsll_svc * svc, uint8_t * addr, void ** pdu);

/* Dequeue a PDU from the service access point. 
   Blocks up to a specified absoluete timeout time. */
int fsll_pdu_timedrecv(struct fsll_svc * svc, uint8_t * addr, void ** pdu, 
					   struct timespec * tm);

/* Enqueue a PDU in the service access point */
bool fsll_pdu_send(struct fsll_svc * svc,  uint8_t addr, 
				   const void * pdu, size_t len);


#ifdef __cplusplus
}
#endif	

#endif /* __FSLL_H__ */

