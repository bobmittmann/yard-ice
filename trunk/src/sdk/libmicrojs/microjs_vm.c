/* $Id: dcclog.c,v 1.10 2006/09/28 19:31:45 bob Exp $ 
 *
 * File:	microjs_vm.c
 * Module:
 * Project:	Micro JavasScript virtual machine
 * Author:	Robinson Mittmann (bob@boreste.com, bob@methafora.com.br)
 * Target:
 * Comment:
 * Copyright(C) 2005 Robinson Mittmann. All Rights Reserved.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __MICROJS_I__
#include "microjs-i.h"

#include <sys/dcclog.h>

static int base(uint16_t s[], int b, int l)
{
	int b1;

	b1 = b; /* find base l levels down */
	while (l > 0) {
		b1 = s[b1];
		l = l - 1;
	}
	return b1;
}

int microjs_run(struct microjs_vm * vm, uint16_t code[], unsigned int len)
{
	uint16_t s[64];
	uint16_t a;
	int i;
	int t;
	int b;
	int p;
	int l;

	printf("\n");

	vm->ip = 0;

	t = 0; 
	b = 1; 
	p = 0;
	s[1] = 0; 
	s[2] = 0; 
	s[3] = 0;
	do {
		i = code[p];
		a = i & 0x07ff;
		l = (i >> 11) & 0x3;
		i >>= 13;
		p = p + 1;
		switch (i) {
		case (LIT >> 13):
			t = t + 1; 
			s[t] = a;
			break;
		case (LOD >> 13):
			t = t + 1; 
			s[t] = s[base(s, b, l) + a];
			break;
		case (STO >> 13):
			s[base(s, b, l)+a] = s[t]; 
			t = t - 1;
			break;
		case (CAL >> 13):
			s[t + 1] = base(s, b, l); 
			s[t + 2] = b; 
			s[t + 3] = p;
			b = t + 1; 
			p = a;
			break;
		case (INT >> 13):
			t = t + a;
			break;
		case (JMP >> 13):
			p = a;
			break;
		case (JPC >> 13):
			if (s[t] == 0)
				p = a; 
			else
				t = t - 1;
			break;
		case (OPR >> 13):
			switch (a) {
			case 0:
				t = b - 1;
				p = s[t + 3]; 
				b = s[t + 2];
				break;
			case (NEG & 0x1fff):
				s[t] = -s[t];
				break;
			case (ADD & 0x1ffff):
				t = t - 1; 
				s[t] = s[t] + s[t + 1];
				break;
			case (SUB & 0x1ffff):
				t = t - 1; 
				s[t] = s[t] - s[t + 1];
				break;
			case (MUL & 0x1ffff):
				t = t - 1; 
				s[t] = s[t] * s[t + 1];
				break;
			case (DIV & 0x1ffff):
				t = t - 1; 
				s[t] = s[t] / s[t + 1];
				break;
			case (BIT & 0x1ffff):
				s[t] = s[t] & 1;
				break;
			case (EQ & 0x1ffff):
				t = t - 1; 
				s[t] = (s[t] == s[t + 1]) ? 1 : 0;
				break;
			case (NEQ & 0x1ffff):
				t = t - 1; 
				s[t] = (s[t] != s[t + 1]) ? 1 : 0;
				break;
			case (LT & 0x1ffff):
				t = t - 1; 
				s[t] = (s[t] < s[t + 1]) ? 1 : 0;
				break;
			case (GTE & 0x1ffff):
				t = t - 1; 
				s[t] = (s[t] >= s[t + 1]) ? 1 : 0;
				break;
			case (GT & 0x1ffff):
				t = t - 1; 
				s[t] = (s[t] > s[t + 1]) ? 1 : 0;
				break;
			case (LTE & 0x1ffff):
				t = t - 1; 
				s[t] = (s[t] <= s[t + 1]) ? 1 : 0;
				break;
			}
		}
	} while (p != 1);

	return 0;
}

/*
const
	levmax=3;
	amax=2047; 
type 
	fct=(lit,opr,lod,sto,cal,int,jmp,jpc);
	instruction=packed record 
		f:fct;
		l:0..levmax;
		a:0..amax;
	end;

procedure interpret;

const stacksize = 500;

var
p, b, t: integer; {program-, base-, topstack-registers}
i: instruction; {instruction register}
s: array [1..stacksize] of integer; {datastore}

function base(l: integer): integer;
var b1: integer;
begin
	b1 := b; {find base l levels down}
	while l > 0 do begin
		b1 := s[b1];
		l := l - 1
	end;
	base := b1
end {base};

begin
	writeln(' start pl/0');
	t := 0; b := 1; p := 0;
	s[1] := 0; s[2] := 0; s[3] := 0;
	repeat
		i := code[p]; p := p + 1;
		with i do
			case f of
			lit: begin t := t + 1; s[t] := a end;
			opr: 
				case a of {operator}
					0: 
					begin {return}
						t := b - 1; p := s[t + 3]; b := s[t + 2];
					end;
					1: s[t] := -s[t];
					2: begin t := t - 1; s[t] := s[t] + s[t + 1] end;
					3: begin t := t - 1; s[t] := s[t] - s[t + 1] end;
					4: begin t := t - 1; s[t] := s[t] * s[t + 1] end;
					5: begin t := t - 1; s[t] := s[t] div s[t + 1] end;
					6: s[t] := ord(odd(s[t]));
					8: begin t := t - 1; s[t] := ord(s[t] = s[t + 1]) end;
					9: begin t := t - 1; s[t] := ord(s[t] <> s[t + 1]) end;
					10: begin t := t - 1; s[t] := ord(s[t] < s[t + 1]) end;
					11: begin t := t - 1; s[t] := ord(s[t] >= s[t + 1]) end;
					12: begin t := t - 1; s[t] := ord(s[t] > s[t + 1]) end;
					13: begin t := t - 1; s[t] := ord(s[t] <= s[t + 1]) end;
				end;
			lod: begin t := t + 1; s[t] := s[base(l) + a] end;
			sto: begin s[base(l)+a] := s[t]; writeln(s[t]); t := t - 1 end;
			cal: 
				begin {generate new block mark}
					s[t + 1] := base(l); s[t + 2] := b; s[t + 3] := p;
					b := t + 1; p := a
				end;
			int: t := t + a;
			jmp: p := a;
			jpc: begin if s[t] = 0 then p := a; t := t - 1 end
		end {with, case}
	until p = 1;
	writeln(' end pl/0');
end {interpret};
*/
