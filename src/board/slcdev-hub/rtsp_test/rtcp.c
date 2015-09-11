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
 * but WITHOUT ANY WARRANTY; without even the implied warranty of * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You can receive a copy of the GNU Lesser General Public License from 
 * http://www.gnu.org/
 */

/** 
 * @file rtcp.c
 * @brief YARD-ICE
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include "rtp.h"
#include <string.h>
#include <stdbool.h>

enum {
    CNAME = 1,
    NAME = 2,
    EMAIL = 3,
    PHONE = 4,
    LOC = 5,
    TOOL = 6,
    NOTE = 7,
    PRIV = 8
};

enum {
    RTCP_SR = 200,
    RTCP_RR = 201,
    RTCP_SDES = 202,
    RTCP_BYE = 203,
    RTCP_APP = 204
};

#define RTCP_PAYLOAD_LEN_MAX 256

struct rtcp_header {
#if (BYTE_ORDER == LITTLE_ENDIAN)
    uint8_t count:5;    //< SC or RC
    uint8_t padding:1;
    uint8_t version:2;
#elif (BYTE_ORDER == BIG_ENDIAN)
    uint8_t version:2;
    uint8_t padding:1;
    uint8_t count:5;    //< SC or RC
#else
#error "Adjust your <bits/endian.h> defines"
#endif
    uint8_t pt;
    uint16_t length;
} __attribute__((__packed__));

struct rtcp_header_sr {
    uint32_t ssrc;
    uint32_t ntp_timestampH;
    uint32_t ntp_timestampL;
    uint32_t rtp_timestamp;
    uint32_t pkt_count;
    uint32_t octet_count;
};

struct rtcp_header_rr {
    uint32_t ssrc;
} __attribute__((__packed__));

struct rtcp_rcv_report_block {
    uint32_t ssrc;
#if (BYTE_ORDER == LITTLE_ENDIAN)
    uint32_t fract_lost: 8;
    uint32_t pkt_lost: 24;
#elif (BYTE_ORDER == BIG_ENDIAN)
    uint32_t pkt_lost: 24;
    uint32_t fract_lost: 8;
#else
#error "Adjust your <bits/endian.h> defines"
#endif
    uint32_t h_seq_no;
    uint32_t jitter;
    uint32_t last_sr;
    uint32_t delay_last_sr;
} __attribute__((__packed__));

struct rtcp_header_sdes {
    uint32_t ssrc;
    uint8_t attr_name;
    uint8_t len;
    char name[];
} __attribute__((__packed__));

struct rtcp_header_bye {
    uint32_t ssrc;
    uint8_t length;
    char reason[];
} __attribute__((__packed__));


/**
 * @brief Basic polymorphic structure of a Server Report RTCP compound
 *
 * Please note that since both @ref RTCP_header_SDES and @ref
 * RTCP_header_BYE are variable sized, there can't be anything after
 * the payload.
 *
 * This structure is designed to be directly applied over the memory
 * area that will be sent as a packet. Its definition is described in
 * RFC 3550 Section 6.4.1.
 */
struct rtcp_sr_compound {
    /** The header for the SR preamble */
    struct rtcp_header sr_hdr;
    /** The actual SR preamble */
    struct rtcp_header_sr sr_pkt;
    /** The header for the payload packet */
    struct rtcp_header payload_hdr;
    /** Polymorphic payload */
    union {
        /** Payload for the source description packet */
        struct rtcp_header_sdes sdes;
        /** Payload for the goodbye packet */
        struct rtcp_header_bye bye;
    } payload;
};

struct rtcp_rr {
    struct rtcp_header rr_hdr;
    struct rtcp_header_rr rr_pkt;
    struct rtcp_rcv_report_block rr_blk[];
} __attribute__((__packed__));


/**
 * @brief Sets the SR preamble for the given compound
 *
 * @param rtp_s The rtp_s to send the RTCP data of
 * @param pkt The compound packet to set data in
 */
static void rtcp_set_sr(struct rtp_session *rtp_s,
                        struct rtcp_sr_compound * pkt,
                        uint32_t rtptime, struct ntp_time * ntp)
{
    size_t sr_size = sizeof(struct rtcp_header) +
        sizeof(struct rtcp_header_sr);

//    rtp_s->last_sr_time = *ntp;
//    rtptime += rtp_s->start_rtptime;


    pkt->sr_hdr.version = 2;
    pkt->sr_hdr.padding = 0;
    pkt->sr_hdr.count = 0;
    pkt->sr_hdr.pt = RTCP_SR;
    pkt->sr_hdr.length = htons((sr_size >> 2) - 1);

    pkt->sr_pkt.ssrc = htonl(rtp_s->ssrc);

    pkt->sr_pkt.ntp_timestampH = htonl(ntp->sec);
    pkt->sr_pkt.ntp_timestampL = htonl(ntp->frac);

    pkt->sr_pkt.rtp_timestamp = htonl(rtptime);

    pkt->sr_pkt.pkt_count = htonl(rtp_s->pkt_count);
    pkt->sr_pkt.octet_count = htonl(rtp_s->octet_count);
}

/**
 * @brief Create a new compound server report for SDES packets
 *
 * @param rtp_s The rtp_s to create the report for
 */


size_t rtcp_pkt_sr_sdes(void * buf, struct rtp_session * rtp_s,
                        uint32_t rtptime, struct ntp_time * ntp)
{
    struct rtcp_sr_compound * pkt;
    char * name = "::";

    int name_len = strlen(name);

    int sdes_size = sizeof(struct rtcp_header) +
        sizeof(struct rtcp_header_sdes) + name_len;

    int pkt_size = sizeof(struct rtcp_header) +
        sizeof(struct rtcp_header_sr) + sdes_size;

    pkt = (struct rtcp_sr_compound *)buf;

    /* Pad to 32-bit */
    if (pkt_size % 4 != 0 ) {
        const size_t padding = 4-(pkt_size%4);
        sdes_size += padding;
        pkt_size += padding;
    }

    pkt = (struct rtcp_sr_compound *)buf;
    memset(pkt, 0, pkt_size);

    rtcp_set_sr(rtp_s, pkt, rtptime, ntp);

    pkt->payload_hdr.version = 2;
    pkt->payload_hdr.padding = 0; // Avoid padding
    pkt->payload_hdr.count = 1;
    pkt->payload_hdr.pt = RTCP_SDES;

    pkt->payload_hdr.length = htons((sdes_size>>2) -1);

    pkt->payload.sdes.ssrc = htonl(rtp_s->ssrc);
    pkt->payload.sdes.attr_name = CNAME;
    pkt->payload.sdes.len = name_len;

    memcpy(&pkt->payload.sdes.name, name, name_len);

    return pkt_size;
}

/**
 * @brief Send a compound RTCP server report
 *
 * @param rtp_s The RTP rtp_s to send the command for
 * @param type The type of packet to send after the SR preamble
 *
 * Warning! This function will actually send a compound response of
 * _two_ packets, the first is the SR packet, and the second is the
 * one requested with the @p type parameter.
 *
 * Since the two packets are sent with a single message, only one call
 * is needed.
 */
int rtcp_send_sr_sdes(struct rtp_session * __rtp, uint32_t __rtptime,
		struct ntp_time * __ntp, struct sockaddr_in * __sin)
{
    uint8_t buf[sizeof(struct rtcp_sr_compound) + RTCP_PAYLOAD_LEN_MAX];
    size_t size;

    size = rtcp_pkt_sr_sdes(buf, __rtp, __rtptime, __ntp);

    return udp_sendto(__rtp->udp[1], (char *)buf, size, __sin);
}






