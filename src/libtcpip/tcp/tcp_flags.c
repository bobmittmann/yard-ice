/* 
 * Copyright(c) 2004-2012 BORESTE (www.boreste.com). All Rights Reserved.
 *
 * This file is part of the libtcpip.
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
 * @file tcp_flags.c
 * @brief 
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#define __USE_SYS_TCP__
#include <sys/tcp.h>

const char tcp_all_flags[64][8] = {
	"......",
	".....F",
	"....S.",
	"....SF",
	"...R..",
	"...R.F",
	"...RS.",
	"...RSF",
	"..P...",
	"..P..F",
	"..P.S.",
	"..P.SF",
	"..PR..",
	"..PR.F",
	"..PRS.",
	"..PRSF",
	".A....",
	".A...F",
	".A..S.",
	".A..SF",
	".A.R..",
	".A.R.F",
	".A.RS.",
	".A.RSF",
	".AP...",
	".AP..F",
	".AP.S.",
	".AP.SF",
	".APR..",
	".APR.F",
	".APRS.",
	".APRSF",
	"U.....",
	"U....F",
	"U...S.",
	"U...SF",
	"U..R..",
	"U..R.F",
	"U..RS.",
	"U..RSF",
	"U.P...",
	"U.P..F",
	"U.P.S.",
	"U.P.SF",
	"U.PR..",
	"U.PR.F",
	"U.PRS.",
	"U.PRSF",
	"UA....",
	"UA...F",
	"UA..S.",
	"UA..SF",
	"UA.R..",
	"UA.R.F",
	"UA.RS.",
	"UA.RSF",
	"UAP...",
	"UAP..F",
	"UAP.S.",
	"UAP.SF",
	"UAPR..",
	"UAPR.F",
	"UAPRS.",
	"UAPRSF"
};

const char tcp_basic_flags[16][5] = { 
	".",
	"F",
	"S",
	"SF",
	"R",
	"FR",
	"SR",
	"SFR",
	"P",
	"FP",
	"SP",
	"SFP",
	"RP",
	"FRP",
	"SRP",
	"SFRP" };


#if 0
/*
#define TF_ACKNOW       0x01
#define TF_DELACK       0x02
#define TF_SENTFIN      0x04
#define TF_IDLE         0x08
#define TF_RXMT         0x10
#define TF_NODELAY      0x20
*/
const char tcp_pcb_flags[64][8] = {
	"      ",
	"     A",
	"    D ",
	"    DA",
	"   F  ",
	"   F A",
	"   FD ",
	"   FDA",
	"  I   ",
	"  I  A",
	"  I D ",
	"  I DA",
	"  IF  ",
	"  IF A",
	"  IFD ",
	"  IFDA",
	" R    ",
	" R   A",
	" R  D ",
	" R  DA",
	" R F  ",
	" R F A",
	" R FD ",
	" R FDA",
	" RI   ",
	" RI  A",
	" RI D ",
	" RI DA",
	" RIF  ",
	" RIF A",
	" RIFD ",
	" RIFDA",
	"N     ",
	"N    A",
	"N   D ",
	"N   DA",
	"N  F  ",
	"N  F A",
	"N  FD ",
	"N  FDA",
	"N I   ",
	"N I  A",
	"N I D ",
	"N I DA",
	"N IF  ",
	"N IF A",
	"N IFD ",
	"N IFDA",
	"NR    ",
	"NR   A",
	"NR  D ",
	"NR  DA",
	"NR F  ",
	"NR F A",
	"NR FD ",
	"NR FDA",
	"NRI   ",
	"NRI  A",
	"NRI D ",
	"NRI DA",
	"NRIF  ",
	"NRIF A",
	"NRIFD ",
	"NRIFDA"
};
#endif
