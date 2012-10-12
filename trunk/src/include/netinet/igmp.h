#ifndef _NETINET_IGMP_H_
#define	_NETINET_IGMP_H_

#include <sys/types.h>

struct igmp_hdr {
  u_int8_t igmp_type;             /* IGMP type */
  u_int8_t igmp_code;             /* routing code */
  u_int16_t igmp_cksum;           /* checksum */
  struct in_addr igmp_group;      /* group address */
};

/*
 * Message types, including version number.
 */
#define IGMP_MEMBERSHIP_QUERY   	0x11	/* membership query         */
#define IGMP_V1_MEMBERSHIP_REPORT	0x12	/* Ver. 1 membership report */
#define IGMP_V2_MEMBERSHIP_REPORT	0x16	/* Ver. 2 membership report */
#define IGMP_V2_LEAVE_GROUP		0x17	/* Leave-group message	    */

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


#endif	/* _NETINET_IGMP_H_ */
