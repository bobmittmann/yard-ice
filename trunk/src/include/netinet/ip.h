#ifndef _NETINET_IP_H_
#define _NETINET_IP_H_

#include <stdint.h>

struct iphdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint8_t hlen:4;
    uint8_t ver:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
    uint8_t ver:4;
    uint8_t hlen:4;
#else
# error "Please fix <bits/endian.h>"
#endif
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t proto;
    uint16_t chksum;
    uint32_t saddr;
    uint32_t daddr;
    /*The options start here. */
	uint8_t opt[];
};

#define ip_off frag_off
#define ip_len tot_len

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define IP_RF 0x0080        /* reserved fragment flag */
#define IP_DF 0x0040        /* dont fragment flag */
#define IP_MF 0x0020        /* more fragments flag */
#define IP_OFFMASK 0xff1f   /* mask for fragmenting bits */

#elif __BYTE_ORDER == __BIG_ENDIAN

#define IP_RF 0x8000        /* reserved fragment flag */
#define IP_DF 0x4000        /* dont fragment flag */
#define IP_MF 0x2000        /* more fragments flag */
#define IP_OFFMASK 0x1fff   /* mask for fragmenting bits */

#else
# error "Please fix <bits/endian.h>"
#endif
 
struct iptrans_hdr {
	struct iphdr ip;
	uint16_t transport[16];
};

#define IP_HEADER_LEN 20

#define IP_MAXOPTLEN 44

#define IP_HDR_LEN_MASK 0x000f
#define IP_VERSION_MASK 0x00f0
#define IP_VERSION(V) (V << 4)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* _NETINET_IP_H_ */

