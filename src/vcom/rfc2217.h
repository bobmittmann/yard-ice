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
 * @file rfc2217.h
 * @brief RFC 2217 - Telnet Com Port Control Option
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#ifndef __RFC2217_H__
#define __RFC2217_H__

#define COM_PORT_OPTION       44

#define SIGNATURE 0
/*   IAC SB COM-PORT-OPTION SIGNATURE <text> IAC SE
       This command may be sent by either the client or the access
       server to exchange signature information.  If the command is
       sent without <text> it is a request from the sender to receive
       the signature text of the receiver.  The text may be a
       combination of any characters.  There is no structure to the
       <text> field.  It may contain manufacturer information, version
       number information, or any other information desired.  If an
       IAC character appears in the text it must be translated to
       IAC-IAC to avoid conflict with the IAC which terminates
       the command. */

#define SET_BAUDRATE 1
/*   IAC SB COM-PORT-OPTION SET-BAUD <value(4)> IAC SE
       This command is sent by the client to the access server to set
       the baud rate of the com port. The value is four octets (4 bytes).
       The value is represented in network standard format.  The value
       is the baud rate being requested.  A special case is the value 0.
       If the value is zero the client is requesting the current baud
       rate of the com port on the access server. */

#define SET_DATASIZE 2
/* IAC SB COM-PORT-OPTION SET-DATASIZE <value> IAC SE
       This command is sent by the client to the access server to set
       the data bit size. The command can also be sent to query the
       current data bit size. The value is one octet (byte). The value
       is an index into the following value table:

           Value       Data Bit Size
             0           Request Current Data Bit Size
             1           Available for Future Use
             2           Available for Future Use
             3           Available for Future Use
             4           Available for Future Use
             5           5
             6           6
             7           7
             8           8
             9-127       Available for Future Use */

#define SET_PARITY 3
/*  IAC SB COM-PORT-OPTION SET-PARITY <value> IAC SE
       This command is sent by the client to the access server to set
       the parity.  The command can also be sent to query the current
       parity. The value is one octet (byte). The value is an index into
       the following value table:

            Value      Parity [1]
              0           Request Current Data Size
              1           NONE
              2           ODD
              3           EVEN
              4           MARK

              5           SPACE
              6-127       Available for Future Use */

#define SET_STOPSIZE 4
/* IAC SB COM-PORT-OPTION SET-STOPSIZE <value> IAC SE
       This command is sent by the client to the access server to set
       the number of stop bits. The command can also be sent to query
       the current stop bit size. The value is one octet (byte). The
       value is an index into the following value table:

           Value      Stop Bit Size
             0           Request Current Data Size
             1           1
             2           2
             3           1.5
             4-127       Available for Future Use */

#define SET_CONTROL 5
/*     IAC SB COM-PORT-OPTION SET-CONTROL <value> IAC SE
       This command is sent by the client to the access server to set
       special com port options. The command can also be sent to query
       the current option value. The value is one octet (byte). The
       value is an index into the following value table:

           Value      Control Commands
             0           Request Com Port Flow Control Setting
                           (outbound/both)
             1           Use No Flow Control (outbound/both)
             2           Use XON/XOFF Flow Control (outbound/both)
             3           Use HARDWARE Flow Control (outbound/both)

             4           Request BREAK State
             5           Set BREAK State ON
             6           Set BREAK State OFF
             7           Request DTR Signal State
             8           Set DTR Signal State ON
             9           Set DTR Signal State OFF
            10           Request RTS Signal State
            11           Set RTS Signal State ON
            12           Set RTS Signal State OFF
            13           Request Com Port Flow Control Setting (inbound)
            14           Use No Flow Control (inbound)
            15           Use XON/XOFF Flow Control (inbound)
            16           Use HARDWARE Flow Control (inbound)
            17           Use DCD Flow Control (outbound/both)
            18           Use DTR Flow Control (inbound)
            19           Use DSR Flow Control (outbound/both)
            20-127       Available for Future Use */

#define NOTIFY_LINESTATE 6
/*     IAC SB COM-PORT-OPTION NOTIFY-LINESTATE <value> IAC SE
       The value is one octet (byte). The value is a bit level
       composition made up from the value table below. Multiple bit
       values may be set in a single transmission. The values are based
       on the most popular UART (com port control chip) in use [1].

           Bit Position     Value     Meaning
              7              128         Time-out Error
              6               64         Transfer Shift Register Empty
              5               32         Transfer Holding Register Empty
              4               16         Break-detect Error
              3                8         Framing Error
              2                4         Parity Error
              1                2         Overrun Error
              0                1         Data Ready */

#define NOTIFY_MODEMSTATE 7
/*     IAC SB COM-PORT-OPTION NOTIFY-MODEMSTATE <value> IAC SE
       The value is one octet (byte). The value is a bit level
       composition made up from the value table below. Multiple bit
       values may be set in a single transmission. The values are based
       on the most popular UART (com port control chip) in use [1].

           Bit Position     Value     Meaning
              7              128        Receive Line Signal Detect
                                        (also known as Carrier Detect)
              6               64        Ring Indicator
              5               32        Data-Set-Ready Signal State
              4               16        Clear-To-Send Signal State
              3                8        Delta Receive Line Signal Detect
              2                4        Trailing-edge Ring Detector
              1                2        Delta Data-Set-Ready
              0                1        Delta Clear-To-Send */

#define FLOWCONTROL_SUSPEND 8
/*     IAC SB COM-PORT-OPTION FLOWCONTROL-SUSPEND IAC SE
       The sender of this command is requesting that the receiver
       suspend transmission of both data and commands until the
       FLOWCONTROL-RESUME is transmitted by the sender. */

#define FLOWCONTROL_RESUME 9
/*  IAC SB COM-PORT-OPTION FLOWCONTROL-RESUME IAC SE
       The sender of this command is requesting that the receiver resume
       transmission of both data and commands. */

#define SET_LINESTATE_MASK 10
/*   IAC SB COM-PORT-OPTION SET-LINESTATE-MASK <value> IAC SE
       This command is sent by the client to the access server to set a
       bit mask for the sending of the NOTIFY-LINESTATE option (see
       section 4).  When the LINESTATE changes on the access server, the
       access server will "AND" the new LINESTATE with the LINESTATE-
       MASK.  If the result is not zero, the access server will send the
       result of the "AND" as the value in a NOTIFY-LINESTATE com port
       option. If more than one bit satisfies the LINESTATE-MASK, only
       one NOTIFY-LINESTATE, with all the satisfying bits, will be sent
       to the client.  The SET-LINESTATE-MASK may be any combination of
       bits as listed below.  These are the same bit values used in the
       NOTIFY-LINESTATE option.  The SET-LINESTATE-MASK values are based
       on the most popular UART (com port control chip) in use [1].

           Bit Position     Value     Meaning
              7              128         Time-out Error
              6               64         Transfer Shift Register Empty
              5               32         Transfer Holding Register Empty
              4               16         Break-detect Error
              3                8         Framing Error
              2                4         Parity Error
              1                2         Overrun Error
              0                1         Data Ready */

#define SET_MODEMSTATE_MASK 11
/*     IAC SB COM-PORT-OPTION SET-MODEMSTATE-MASK <value> IAC SE
       This command is sent by the client to the access server to set a
       bit mask for the sending of the NOTIFY-MODEMSTATE option (see
       section 4).  When the MODEMSTATE changes on the access server,
       the access server will "AND" the new MODEMSTATE with the
       MODEMSTATE-MASK.  If the result is not zero, the access server
       will send the result of the "AND" as the value in a NOTIFY-
       MODEMSTATE com port option. If more than one bit satisfies the
       MODEMSTATE-MASK, only one NOTIFY-MODEMSTATE, with all the
       satisfying bits, will be sent to the client.  The SET-
       MODEMSTATE-MASK may be any combination of bits as listed below.
       These are the same bit values used in the NOTIFY-MODEMSTATE
       option.  The SET-MODEMSTATE-MASK values are based on the most
       popular UART (com port control chip) in use [1].

           Bit Position     Value     Meaning
              7              128        Receive Line Signal Detect
                                        (also known as Carrier Detect)
              6               64        Ring Indicator
              5               32        Data-Set-Ready Signal State
              4               16        Clear-To-Send Signal State
              3                8        Delta Receive Line Signal Detect
              2                4        Trailing-edge Ring Detector
              1                2        Delta Data-Set-Ready
              0                1        Delta Clear-To-Send */

#define PURGE_DATA 12
/*	IAC SB COM-PORT-OPTION PURGE-DATA <value> IAC SE
       This command is sent by the client to the access server to
       instruct the access server to immediately clear all data from the
       buffer or buffers referenced by the value.  The value is one
       octet (byte).  The value is an index into the following value
       table:

           Value      Purge Data Buffer
             0           Available for Future Use
             1           Purge access server receive data buffer
             2           Purge access server transmit data buffer
             3           Purge both the access server receive data
                         buffer and the access server transmit data
                         buffer
             4-127       Available for Future Use */

/* CPC Client to Access Server constants */
#define CAS_SIGNATURE SIGNATURE
#define CAS_SET_BAUDRATE SET_BAUDRATE
#define CAS_SET_DATASIZE SET_DATASIZE
#define CAS_SET_PARITY SET_PARITY
#define CAS_SET_STOPSIZE SET_STOPSIZE
#define CAS_SET_CONTROL SET_CONTROL
#define CAS_NOTIFY_LINESTATE NOTIFY_LINESTATE
#define CAS_NOTIFY_MODEMSTATE NOTIFY_MODEMSTATE
#define CAS_FLOWCONTROL_SUSPEND FLOWCONTROL_SUSPEND
#define CAS_FLOWCONTROL_RESUME FLOWCONTROL_RESUME
#define CAS_SET_LINESTATE_MASK SET_LINESTATE_MASK
#define CAS_SET_MODEMSTATE_MASK SET_MODEMSTATE_MASK
#define CAS_PURGE_DATA PURGE_DATA

/* CPC Access Server to Client constants */
#define ASC_SIGNATURE (100 + SIGNATURE)
#define ASC_SET_BAUDRATE (100 + SET_BAUDRATE)
#define ASC_SET_DATASIZE (100 + SET_DATASIZE)
#define ASC_SET_PARITY (100 + SET_PARITY)
#define ASC_SET_STOPSIZE (100 + SET_STOPSIZE)
#define ASC_SET_CONTROL (100 + SET_CONTROL)
#define ASC_NOTIFY_LINESTATE (100 + NOTIFY_LINESTATE)
#define ASC_NOTIFY_MODEMSTATE (100 + NOTIFY_MODEMSTATE)
#define ASC_FLOWCONTROL_SUSPEND (100 + FLOWCONTROL_SUSPEND)
#define ASC_FLOWCONTROL_RESUME (100 + FLOWCONTROL_RESUME)
#define ASC_SET_LINESTATE_MASK (100 + SET_LINESTATE_MASK)
#define ASC_SET_MODEMSTATE_MASK (100 + SET_MODEMSTATE_MASK)
#define ASC_PURGE_DATA (100 + PURGE_DATA)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RFC2217_H__ */
