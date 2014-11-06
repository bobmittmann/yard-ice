/* 
 * File:	 fft.h
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2013 Bob Mittmann. All Rights Reserved.
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
 */

#ifndef __VT100_H__
#define __VT100_H__

#define ESC "\033"

#define VT100_GOTO ESC "[%d;%df"

#define VT100_CLRSCR ESC "[2J"

#define VT100_CLREOL ESC "[K"

#define VT100_CURSOR_SHOW ESC "[?25h"

#define VT100_CURSOR_HIDE ESC "[?25l"

#define VT100_CURSOR_SAVE ESC "[s"

#define VT100_CURSOR_UNSAVE ESC "[u"

#define VT100_ATTR_SAVE ESC "[7"

#define VT100_ATTR_RESTORE ESC "[8"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif	

#endif /* __VT100_H__ */

