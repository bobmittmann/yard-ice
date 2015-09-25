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
 * @file yard-ice.c
 * @brief YARD-ICE application main
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 */ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <tcpip/httpd.h>


/*---------------------------------------------------------------------------
  Icon (thinkos16x16.png)
  ---------------------------------------------------------------------------*/
const uint8_t thinkos16x16_png[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
	0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
	0x08, 0x06, 0x00, 0x00, 0x00, 0x1f, 0xf3, 0xff, 0x61, 0x00, 0x00, 0x00,
	0x04, 0x73, 0x42, 0x49, 0x54, 0x08, 0x08, 0x08, 0x08, 0x7c, 0x08, 0x64,
	0x88, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x02,
	0x42, 0x00, 0x00, 0x02, 0x42, 0x01, 0xfe, 0xa1, 0x55, 0x03, 0x00, 0x00,
	0x00, 0x19, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61,
	0x72, 0x65, 0x00, 0x77, 0x77, 0x77, 0x2e, 0x69, 0x6e, 0x6b, 0x73, 0x63,
	0x61, 0x70, 0x65, 0x2e, 0x6f, 0x72, 0x67, 0x9b, 0xee, 0x3c, 0x1a, 0x00,
	0x00, 0x01, 0x73, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0x95, 0x8f, 0xbf,
	0x2f, 0x43, 0x71, 0x14, 0xc5, 0x3f, 0xf7, 0xbd, 0xaa, 0xc4, 0x20, 0x2f,
	0x1d, 0x84, 0x0d, 0x61, 0xd1, 0x41, 0x78, 0x7d, 0x92, 0x9a, 0x0c, 0x06,
	0x93, 0x45, 0x22, 0xfe, 0x02, 0x61, 0xb6, 0x92, 0xf7, 0x63, 0x32, 0x98,
	0x98, 0xba, 0x88, 0x91, 0x45, 0xc4, 0x50, 0x4d, 0x8c, 0x92, 0x26, 0x0d,
	0x8d, 0x55, 0x44, 0xc2, 0x20, 0x7e, 0x0c, 0x6a, 0x7f, 0xfa, 0x5c, 0x83,
	0x96, 0xd7, 0xa6, 0x5a, 0x3d, 0xc9, 0x4d, 0xce, 0xf7, 0x7c, 0xef, 0x39,
	0xb9, 0x07, 0x55, 0x25, 0x3e, 0x64, 0xbd, 0xd4, 0x0f, 0xb7, 0xbd, 0x03,
	0x32, 0xde, 0x61, 0xab, 0xbf, 0xfa, 0x18, 0xc4, 0x20, 0x4e, 0xb0, 0x44,
	0xc8, 0x8d, 0x4c, 0xf9, 0x16, 0x4d, 0x90, 0x29, 0xdf, 0x22, 0xe4, 0x46,
	0x9c, 0x60, 0x29, 0xae, 0x37, 0x04, 0x10, 0xf6, 0x15, 0x80, 0x2a, 0xa6,
	0xac, 0xd7, 0x94, 0x5d, 0x22, 0x76, 0x00, 0x6a, 0x5a, 0xb5, 0xb6, 0xf3,
	0x0b, 0x55, 0x05, 0xc7, 0x9b, 0xc7, 0xce, 0xf5, 0x7c, 0xf3, 0x20, 0x5d,
	0xe7, 0x0d, 0xd5, 0xec, 0x5c, 0x0f, 0x4e, 0x90, 0xfe, 0xe5, 0xde, 0xbc,
	0xaa, 0x22, 0x64, 0xbd, 0x14, 0x21, 0xf7, 0xc0, 0x33, 0x11, 0x0b, 0x7a,
	0xed, 0x3e, 0x34, 0x9f, 0xdf, 0x54, 0x65, 0x18, 0x93, 0x02, 0x30, 0x44,
	0x92, 0x11, 0x43, 0x8b, 0x6e, 0x85, 0x04, 0x69, 0x94, 0x3c, 0x16, 0x8f,
	0xed, 0xcc, 0x00, 0x58, 0x3c, 0xa2, 0xe4, 0x49, 0x90, 0xd6, 0xa2, 0x5b,
	0x11, 0x55, 0xed, 0xe8, 0x69, 0x7b, 0x11, 0xb6, 0xb7, 0x0d, 0x80, 0xc9,
	0x3e, 0x1f, 0xc9, 0x77, 0x8c, 0x70, 0xa3, 0xe5, 0xa6, 0xc9, 0xbe, 0x96,
	0xdc, 0x5b, 0x00, 0x99, 0xf1, 0x47, 0xf9, 0x64, 0x15, 0x20, 0x01, 0xcc,
	0x01, 0x50, 0xe5, 0x04, 0x23, 0x8a, 0x80, 0x95, 0x98, 0x6d, 0x00, 0x88,
	0x80, 0x37, 0xd4, 0x38, 0x03, 0x6e, 0x01, 0xf8, 0xc4, 0xaa, 0xfb, 0xda,
	0x56, 0x90, 0x8c, 0x7f, 0x82, 0xf2, 0xa2, 0x65, 0x77, 0xed, 0xaf, 0x1d,
	0x03, 0x40, 0xc4, 0x37, 0xc4, 0xf1, 0x17, 0x3b, 0xf5, 0x6d, 0x19, 0x20,
	0xb3, 0x7e, 0x0a, 0x9b, 0x2b, 0x94, 0x23, 0xb1, 0xfd, 0xb1, 0xae, 0x03,
	0xb4, 0xe8, 0x56, 0x10, 0x4e, 0x31, 0xcd, 0x09, 0x2d, 0xbb, 0x77, 0x32,
	0xbe, 0xd7, 0xdb, 0x55, 0x00, 0x80, 0x5e, 0xba, 0x9e, 0x96, 0x36, 0xef,
	0x45, 0x10, 0xfa, 0x2b, 0x17, 0x92, 0x09, 0xd6, 0x3b, 0x19, 0x1b, 0x02,
	0x7e, 0x30, 0x1d, 0x2c, 0x23, 0x4c, 0xa2, 0x7a, 0xfe, 0xdf, 0x80, 0x44,
	0xc3, 0x4b, 0x06, 0x8f, 0xd1, 0xd7, 0x27, 0x2d, 0x6f, 0xdd, 0xfd, 0x37,
	0xe0, 0x0b, 0xca, 0x0a, 0xaf, 0x48, 0x18, 0x12, 0x78, 0x63, 0x00, 0x00,
	0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82,
};


/*---------------------------------------------------------------------------
  Icon (thinkos47x20.png)
  ---------------------------------------------------------------------------*/
const uint8_t thinkos47x20_png[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
	0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x00, 0x14,
	0x08, 0x06, 0x00, 0x00, 0x00, 0x1d, 0x9a, 0x64, 0xc2, 0x00, 0x00, 0x04,
	0x8a, 0x49, 0x44, 0x41, 0x54, 0x48, 0xc7, 0xbd, 0xd6, 0x59, 0x88, 0x56,
	0x65, 0x18, 0x07, 0xf0, 0xdf, 0x37, 0xf3, 0x8d, 0x8e, 0x7b, 0x8b, 0x46,
	0xb4, 0x53, 0x44, 0xb4, 0x27, 0x47, 0xa2, 0x28, 0x2b, 0x0b, 0x83, 0x0a,
	0xac, 0x84, 0x12, 0x5a, 0x10, 0xda, 0x10, 0x21, 0xba, 0x28, 0x73, 0x28,
	0xa2, 0x97, 0xea, 0xa2, 0xcd, 0x0c, 0xea, 0x2a, 0x35, 0x25, 0x42, 0xeb,
	0x22, 0x12, 0xbd, 0xb0, 0x50, 0x42, 0x5b, 0xc5, 0x3a, 0x85, 0x65, 0x05,
	0x59, 0xb4, 0x40, 0x5a, 0xb6, 0x39, 0xa6, 0x39, 0xea, 0xcc, 0x9c, 0x2e,
	0xe6, 0xf9, 0xbe, 0x79, 0xfb, 0x5c, 0xb0, 0x0b, 0x3d, 0x70, 0x38, 0xef,
	0x79, 0xd7, 0xe7, 0xf9, 0x3f, 0xff, 0xe7, 0xff, 0xbc, 0x35, 0x50, 0x24,
	0xca, 0xa4, 0xd9, 0xa6, 0x86, 0x4a, 0x99, 0x28, 0xd2, 0x48, 0xcc, 0xc3,
	0xc5, 0x38, 0x5f, 0x99, 0xba, 0x15, 0xa9, 0x0d, 0x15, 0x28, 0x53, 0x15,
	0x6b, 0x06, 0xf7, 0x38, 0x4c, 0x4f, 0x5b, 0xf3, 0xe0, 0xc1, 0xe7, 0x0a,
	0xac, 0xc5, 0xe9, 0xcd, 0x39, 0x03, 0xce, 0x34, 0x5e, 0x38, 0x19, 0xaf,
	0xe2, 0xc1, 0x6c, 0xce, 0xe0, 0x53, 0xa4, 0xc3, 0x64, 0x7c, 0x99, 0x5a,
	0x11, 0xdb, 0x8d, 0x0b, 0xf1, 0x54, 0x66, 0xd8, 0x56, 0xfc, 0x8e, 0xfe,
	0xe8, 0xbb, 0x12, 0xb7, 0x60, 0x78, 0xfc, 0xb7, 0x47, 0x94, 0x0e, 0x6b,
	0x04, 0xea, 0xff, 0xa1, 0x4c, 0x1c, 0x8d, 0x97, 0x71, 0x91, 0x22, 0x1d,
	0x81, 0x6e, 0x65, 0x9a, 0x91, 0xa1, 0x3a, 0x02, 0x13, 0xf1, 0x29, 0x5e,
	0x88, 0xde, 0xde, 0xa6, 0xd1, 0x7b, 0xef, 0x77, 0xc8, 0x9e, 0x5a, 0xa0,
	0x75, 0x09, 0x7a, 0xb0, 0x3e, 0xb8, 0xdc, 0x89, 0x7e, 0x65, 0xda, 0xa9,
	0x48, 0x39, 0x65, 0xfa, 0x83, 0xe3, 0x43, 0x30, 0x4a, 0x99, 0xfe, 0x08,
	0x87, 0xce, 0xc7, 0x58, 0xac, 0x51, 0xa6, 0x5e, 0x45, 0xaa, 0x29, 0x53,
	0x75, 0xe8, 0x69, 0x33, 0x60, 0xd4, 0xf3, 0xf8, 0x04, 0x73, 0x31, 0x04,
	0xbb, 0xb0, 0x3b, 0x50, 0xac, 0x82, 0x2e, 0x7d, 0x59, 0x72, 0xf6, 0xe2,
	0x0f, 0x45, 0x6a, 0x57, 0xa4, 0x3b, 0x23, 0x5a, 0x2f, 0x61, 0xfc, 0x3e,
	0x73, 0xe0, 0x10, 0x23, 0x7f, 0x0e, 0x9e, 0xc3, 0x49, 0x38, 0xaf, 0x69,
	0x6c, 0x43, 0x51, 0xf6, 0xb5, 0x6e, 0x30, 0x02, 0x73, 0x71, 0x03, 0xee,
	0x52, 0xa6, 0x15, 0x11, 0x29, 0x4d, 0xb5, 0xca, 0xd5, 0x6c, 0x7f, 0x89,
	0xdc, 0x9a, 0x2f, 0xf9, 0xbc, 0xff, 0xaa, 0xe0, 0xde, 0xfd, 0x19, 0x9f,
	0x8f, 0xf9, 0xdf, 0xee, 0x17, 0x69, 0x98, 0x22, 0x0d, 0xdb, 0xcf, 0xd8,
	0xc1, 0x29, 0x4f, 0x3e, 0xaf, 0x75, 0x7e, 0x63, 0x6c, 0x1f, 0xfd, 0x35,
	0x45, 0xea, 0x8c, 0x30, 0xb7, 0x61, 0x47, 0x0c, 0x75, 0xb6, 0xa0, 0x5e,
	0xcb, 0xda, 0x7d, 0x11, 0x99, 0x0e, 0xec, 0x89, 0xff, 0xa1, 0xa8, 0x67,
	0xff, 0xf5, 0xa0, 0xd9, 0x9e, 0x03, 0x20, 0xdf, 0x19, 0xfb, 0xec, 0xde,
	0x2b, 0x42, 0x03, 0xed, 0xa1, 0xca, 0xb4, 0x6b, 0xa0, 0x7d, 0x37, 0xe5,
	0x3c, 0x51, 0x5f, 0xda, 0x1b, 0xfb, 0xd6, 0x23, 0xec, 0x35, 0x6c, 0x55,
	0xa6, 0x2e, 0x45, 0x1a, 0x83, 0x17, 0xd1, 0x1d, 0xe3, 0xc2, 0xa8, 0x7e,
	0x8c, 0xc1, 0x06, 0x7c, 0x8e, 0x1b, 0xb1, 0x50, 0x99, 0xd6, 0x29, 0xd2,
	0xf5, 0xf1, 0xff, 0x23, 0x3e, 0xc0, 0xb9, 0xf8, 0x16, 0xaf, 0x67, 0xf4,
	0xac, 0xb2, 0xc4, 0xaf, 0x30, 0x1b, 0x9b, 0xf1, 0x0a, 0x7a, 0x9a, 0x74,
	0x2b, 0x52, 0x07, 0xa6, 0x84, 0xda, 0x7d, 0x82, 0xa5, 0xca, 0xd4, 0xa3,
	0x48, 0x27, 0xc6, 0x19, 0xc7, 0x2a, 0xd2, 0x67, 0x58, 0x59, 0xc7, 0x4d,
	0x81, 0xfa, 0x66, 0x74, 0x85, 0x76, 0xdf, 0x16, 0xba, 0xde, 0x11, 0x63,
	0xa3, 0x9a, 0x3c, 0x66, 0x45, 0xf4, 0xcf, 0xc0, 0xc7, 0x58, 0x87, 0xb3,
	0x30, 0x2d, 0xd4, 0xea, 0x6f, 0xdc, 0x83, 0x55, 0x4d, 0xe3, 0x07, 0x95,
	0xa7, 0x42, 0x15, 0x08, 0xce, 0xc0, 0x17, 0x78, 0x0d, 0x3d, 0xcd, 0x39,
	0x45, 0x9a, 0x8d, 0xc7, 0x02, 0xb0, 0x0e, 0x3c, 0xa1, 0x48, 0x4f, 0x63,
	0x35, 0x4e, 0x0d, 0x76, 0x8c, 0xc0, 0x82, 0x7a, 0xa0, 0x54, 0x6b, 0x6a,
	0x35, 0xbf, 0xe1, 0x82, 0x08, 0x7f, 0x2f, 0x8e, 0xc4, 0x93, 0x38, 0x21,
	0x0c, 0xfc, 0x1e, 0x93, 0x9b, 0x11, 0x19, 0xa0, 0xc2, 0xa2, 0x50, 0x9b,
	0x3e, 0x9c, 0x12, 0x45, 0xee, 0xf3, 0x66, 0xf8, 0x07, 0x68, 0xf5, 0x4f,
	0xec, 0xd5, 0x83, 0xed, 0xf1, 0xee, 0x8c, 0x35, 0x14, 0x69, 0x6c, 0xb4,
	0x67, 0x45, 0xd4, 0xba, 0xf0, 0x68, 0x54, 0xf1, 0x9f, 0xc2, 0xf0, 0x47,
	0xb0, 0x14, 0x93, 0x30, 0xa2, 0xae, 0x4c, 0x9b, 0xb3, 0x24, 0x68, 0xf0,
	0x6c, 0x7d, 0xd6, 0x37, 0x2a, 0xbc, 0xed, 0xc3, 0x7a, 0x65, 0xda, 0xa3,
	0x08, 0x2e, 0x73, 0x59, 0x54, 0xda, 0xee, 0x38, 0xf4, 0xaa, 0x70, 0xf0,
	0x67, 0x7c, 0x17, 0x73, 0xae, 0xc7, 0xe5, 0xd8, 0x84, 0xab, 0xf1, 0x0d,
	0x1e, 0x8a, 0x28, 0x0c, 0x09, 0x00, 0xee, 0x88, 0x39, 0x5f, 0x45, 0x2e,
	0xcc, 0x57, 0xa6, 0x37, 0x14, 0xe9, 0xf8, 0xa0, 0xf5, 0xcd, 0x01, 0xe4,
	0x07, 0xca, 0xb4, 0x21, 0xa8, 0xab, 0x1e, 0x21, 0x1c, 0x1b, 0x68, 0x5d,
	0x6a, 0x42, 0xea, 0x52, 0xe9, 0x88, 0x82, 0xd4, 0x97, 0x51, 0xa7, 0xd6,
	0x92, 0xa4, 0x8d, 0x6b, 0x82, 0x40, 0xe5, 0xfb, 0x30, 0x78, 0x12, 0x86,
	0xc5, 0x9a, 0xe5, 0x98, 0x80, 0x99, 0xe1, 0x50, 0x4f, 0x38, 0xbc, 0x32,
	0xd6, 0xed, 0xc4, 0x75, 0x98, 0x8f, 0x65, 0xf8, 0x0b, 0xed, 0xf8, 0x51,
	0x91, 0xea, 0x91, 0x43, 0x6d, 0x41, 0xc5, 0x3a, 0x16, 0x29, 0x52, 0x17,
	0x3e, 0x52, 0xa6, 0x1f, 0x1a, 0xb7, 0xc3, 0xc7, 0xe3, 0xa0, 0xa9, 0x2a,
	0x47, 0xa3, 0x37, 0x0c, 0x17, 0x89, 0x5a, 0x65, 0x4a, 0x23, 0xbb, 0xe3,
	0xcc, 0x09, 0xda, 0x6d, 0xc7, 0x29, 0xca, 0x34, 0x3f, 0xf8, 0xbf, 0x29,
	0xa3, 0xe1, 0xb6, 0xf8, 0x4e, 0x8b, 0x44, 0xec, 0x89, 0xba, 0x32, 0x3c,
	0x40, 0x5b, 0x88, 0x0f, 0xa3, 0x56, 0xd4, 0x9a, 0xaa, 0x57, 0xa6, 0xde,
	0x4c, 0xfd, 0xd6, 0x46, 0x62, 0x1f, 0x85, 0xc5, 0x58, 0xa2, 0x48, 0x67,
	0xb5, 0x65, 0x07, 0x2c, 0xc0, 0xe4, 0xac, 0xe4, 0x1f, 0xa8, 0x4a, 0x76,
	0x64, 0xf9, 0x31, 0x3c, 0xbe, 0x9d, 0xd1, 0x37, 0x2e, 0xda, 0x8d, 0xf5,
	0x43, 0x33, 0x87, 0x77, 0x84, 0x53, 0x43, 0xa3, 0x3d, 0x31, 0x14, 0x6c,
	0x79, 0x48, 0x64, 0x7f, 0xf3, 0xa2, 0x37, 0xa0, 0x3e, 0xed, 0xf1, 0xdf,
	0xa3, 0x4c, 0xd3, 0x43, 0x08, 0xde, 0xc6, 0x45, 0x8d, 0x84, 0xad, 0x94,
	0x69, 0x56, 0x8b, 0x71, 0x53, 0x71, 0x1a, 0x9e, 0x3e, 0x88, 0x52, 0xdf,
	0x17, 0x87, 0x54, 0x19, 0x15, 0xfa, 0x33, 0x43, 0xaa, 0x96, 0x68, 0xd5,
	0x32, 0xbe, 0xff, 0x82, 0x1f, 0xf0, 0xb0, 0x22, 0x2d, 0xc6, 0xaf, 0x31,
	0x36, 0x2e, 0x2a, 0xf8, 0xd8, 0x58, 0xf3, 0x67, 0xa8, 0xd6, 0x92, 0x40,
	0xfd, 0x23, 0x8c, 0xaf, 0x67, 0x28, 0xb7, 0x47, 0x32, 0x9e, 0x11, 0x21,
	0xda, 0xa6, 0x48, 0xcf, 0x65, 0x87, 0xf5, 0x67, 0x06, 0x57, 0x2d, 0xdf,
	0xbe, 0x6c, 0x6c, 0x74, 0x66, 0x9c, 0x0c, 0xbd, 0x2a, 0x73, 0xaa, 0x37,
	0xa2, 0xb7, 0x26, 0xae, 0x25, 0xab, 0x71, 0x7f, 0xd0, 0xb0, 0x17, 0xd7,
	0x2a, 0xd2, 0xdb, 0xb8, 0x26, 0x8b, 0xc4, 0xa5, 0xf8, 0x1a, 0x23, 0x23,
	0x37, 0x86, 0xb4, 0x65, 0xf7, 0x90, 0x86, 0x82, 0x9c, 0x19, 0x77, 0xfa,
	0x7b, 0x83, 0x77, 0x6d, 0xa1, 0xab, 0xa3, 0xb3, 0x4a, 0xdb, 0x91, 0x7d,
	0x1b, 0xe3, 0xe3, 0x14, 0x69, 0x26, 0x9e, 0x0d, 0xea, 0x4c, 0x51, 0xa4,
	0x87, 0x71, 0x76, 0xcb, 0xdc, 0x51, 0x91, 0xd0, 0x23, 0x70, 0x9c, 0x32,
	0xad, 0xc3, 0x9b, 0xb8, 0x2f, 0xc6, 0xde, 0x09, 0xfe, 0x2f, 0xc4, 0xad,
	0x78, 0x3f, 0x0c, 0x7e, 0x2f, 0x1c, 0x9d, 0x13, 0x4e, 0xbd, 0x55, 0x8f,
	0xb2, 0x9d, 0x5f, 0x05, 0x56, 0x61, 0xba, 0x32, 0x2d, 0xcb, 0x68, 0xf0,
	0x29, 0xb6, 0x64, 0xe8, 0x6f, 0xc1, 0x97, 0x11, 0xe6, 0x3d, 0x78, 0x37,
	0x8a, 0xda, 0x19, 0x38, 0x31, 0x12, 0xf0, 0xc8, 0x50, 0x9a, 0xcd, 0x61,
	0x50, 0x77, 0x24, 0xeb, 0x6a, 0x6c, 0x8c, 0x35, 0x1b, 0x15, 0x69, 0x38,
	0x9e, 0x89, 0xfa, 0x30, 0x09, 0x0f, 0x44, 0xd4, 0xae, 0x8c, 0x7d, 0x66,
	0x86, 0xe3, 0xab, 0x70, 0x7b, 0x00, 0xb8, 0x1c, 0x8f, 0xfc, 0x0b, 0x7e,
	0xa7, 0xb5, 0xdd, 0x5e, 0xab, 0x7b, 0x82, 0x00, 0x00, 0x00, 0x00, 0x49,
	0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82,
};

/*---------------------------------------------------------------------------
  Icon (thinkos57x24.png)
  ---------------------------------------------------------------------------*/
const uint8_t thinkos57x24_png[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
	0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x39, 0x00, 0x00, 0x00, 0x18,
	0x08, 0x06, 0x00, 0x00, 0x00, 0x40, 0xe8, 0x55, 0x16, 0x00, 0x00, 0x00,
	0x04, 0x73, 0x42, 0x49, 0x54, 0x08, 0x08, 0x08, 0x08, 0x7c, 0x08, 0x64,
	0x88, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x03,
	0x69, 0x00, 0x00, 0x03, 0x69, 0x01, 0x42, 0x0f, 0x3c, 0x1c, 0x00, 0x00,
	0x00, 0x19, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61,
	0x72, 0x65, 0x00, 0x77, 0x77, 0x77, 0x2e, 0x69, 0x6e, 0x6b, 0x73, 0x63,
	0x61, 0x70, 0x65, 0x2e, 0x6f, 0x72, 0x67, 0x9b, 0xee, 0x3c, 0x1a, 0x00,
	0x00, 0x05, 0xca, 0x49, 0x44, 0x41, 0x54, 0x58, 0x85, 0xdd, 0x98, 0x79,
	0x6c, 0x14, 0x75, 0x14, 0xc7, 0x3f, 0x6f, 0x76, 0xb7, 0x2d, 0x5a, 0x10,
	0x1b, 0x03, 0x22, 0xa9, 0x01, 0xa2, 0x46, 0x6b, 0x45, 0xda, 0x9d, 0xa2,
	0x20, 0x8a, 0x15, 0xaf, 0x98, 0x26, 0x46, 0xbc, 0xa2, 0x28, 0x36, 0x26,
	0x1e, 0x18, 0x88, 0x80, 0x10, 0xe4, 0x68, 0xa7, 0xb3, 0x5c, 0x22, 0xa1,
	0x72, 0x79, 0x04, 0x89, 0x22, 0xa2, 0x21, 0x8a, 0x86, 0x60, 0x8c, 0x60,
	0x42, 0xb8, 0x3c, 0xd8, 0x74, 0xb7, 0x0a, 0x9a, 0x2a, 0xa8, 0x85, 0x80,
	0x52, 0x54, 0x2c, 0x48, 0x4b, 0x4b, 0xdb, 0xdd, 0x7d, 0xfe, 0xb1, 0xbf,
	0xae, 0xdb, 0xb2, 0xd8, 0x16, 0xaa, 0x26, 0x7c, 0x93, 0xc9, 0xcc, 0xef,
	0x1d, 0xdf, 0xdf, 0x7b, 0x33, 0xef, 0x77, 0xcc, 0x4f, 0x54, 0x95, 0xce,
	0x42, 0x6c, 0x77, 0x23, 0x70, 0x07, 0x42, 0xa1, 0x56, 0x38, 0x5b, 0x3b,
	0xed, 0xf8, 0x3f, 0xc3, 0x9b, 0x4a, 0x28, 0x83, 0x5f, 0xb8, 0x90, 0xf4,
	0xa6, 0x52, 0xa2, 0xbc, 0xa3, 0x95, 0x4e, 0xa8, 0x23, 0x12, 0xf1, 0xbb,
	0x73, 0x10, 0x42, 0x1a, 0x72, 0xd6, 0x77, 0x7f, 0x88, 0x67, 0x0f, 0x2b,
	0xa5, 0x34, 0xbd, 0xf9, 0x19, 0x94, 0x89, 0x58, 0x2c, 0xee, 0x88, 0x40,
	0x0a, 0xdc, 0xa1, 0x08, 0x33, 0x80, 0x35, 0x72, 0xad, 0xdb, 0xa7, 0xbb,
	0x03, 0xec, 0x0e, 0xa4, 0x4e, 0xd2, 0xd7, 0xb3, 0x1c, 0x38, 0x00, 0xdc,
	0x20, 0xfe, 0xc0, 0x83, 0x09, 0xb9, 0xf2, 0x06, 0x30, 0x1d, 0xa5, 0x3a,
	0xc9, 0x7a, 0x31, 0x20, 0x28, 0xf3, 0x74, 0x97, 0xf3, 0xdb, 0xbf, 0x18,
	0xeb, 0x19, 0x23, 0x65, 0xb9, 0xea, 0x17, 0x93, 0x1a, 0xc5, 0x0e, 0x4c,
	0x01, 0x75, 0xb1, 0xf4, 0xd7, 0x84, 0x3c, 0xec, 0xbc, 0x97, 0x6c, 0x27,
	0xb9, 0x6e, 0x1a, 0x19, 0x6c, 0x07, 0x7a, 0x73, 0x3c, 0x6b, 0xd1, 0xbf,
	0x1c, 0xeb, 0x99, 0x43, 0x55, 0x61, 0x58, 0x59, 0x16, 0xfe, 0xb2, 0x59,
	0x0c, 0x5e, 0x78, 0xbe, 0xaa, 0xd2, 0x7a, 0x71, 0x73, 0x99, 0x37, 0xb9,
	0x7d, 0xba, 0xab, 0xbd, 0x1d, 0x79, 0x65, 0x36, 0xf9, 0xee, 0x13, 0x9d,
	0xf1, 0xfd, 0x2f, 0x2e, 0x51, 0x55, 0xc4, 0x76, 0x97, 0x02, 0x13, 0x80,
	0x43, 0x20, 0x33, 0x08, 0x97, 0xae, 0x56, 0xa5, 0xf3, 0xd3, 0xae, 0x81,
	0xd8, 0xf3, 0xfa, 0x41, 0xcb, 0x7c, 0x60, 0x2c, 0xd0, 0x82, 0x87, 0x6b,
	0x34, 0xe8, 0xec, 0xed, 0xee, 0x0f, 0xd3, 0x55, 0x98, 0x31, 0x69, 0xbd,
	0x8e, 0xb0, 0x19, 0xb8, 0x04, 0x74, 0x1a, 0xfe, 0x15, 0x29, 0xcb, 0xb8,
	0x43, 0x68, 0x8b, 0x1f, 0x78, 0x0c, 0x68, 0x00, 0x66, 0xe3, 0xe9, 0x75,
	0xb0, 0x9b, 0xe2, 0x3c, 0x2b, 0x48, 0xf2, 0x3a, 0x29, 0x05, 0x81, 0xbb,
	0x89, 0x49, 0xad, 0x86, 0x4b, 0x76, 0x9c, 0x31, 0xa1, 0xed, 0x4e, 0x26,
	0xc6, 0x5a, 0xad, 0x74, 0x0e, 0x75, 0x4b, 0x84, 0xdd, 0x80, 0x36, 0x49,
	0x9e, 0xab, 0x48, 0xbd, 0x84, 0x9c, 0x63, 0x10, 0xf2, 0xcb, 0xb6, 0x26,
	0xb5, 0x56, 0x68, 0xc8, 0x79, 0x57, 0x86, 0xbb, 0x59, 0x34, 0xf1, 0x61,
	0x27, 0xbc, 0xab, 0x10, 0x36, 0x12, 0x63, 0x32, 0x16, 0xeb, 0xb4, 0xc2,
	0x59, 0x0e, 0x20, 0xb6, 0xfb, 0x21, 0x4a, 0x16, 0x00, 0x16, 0xeb, 0x00,
	0x88, 0x71, 0x1f, 0x16, 0xe5, 0x5a, 0xe1, 0x6c, 0xf8, 0x47, 0x4a, 0xbf,
	0x5b, 0x0c, 0x14, 0x03, 0xab, 0x34, 0xec, 0xac, 0x3a, 0x45, 0x2f, 0x08,
	0xf9, 0x81, 0x62, 0x2c, 0xbd, 0x05, 0x65, 0x1f, 0x1e, 0xcf, 0x9b, 0x1a,
	0x9c, 0xb5, 0x0f, 0x40, 0x72, 0xdd, 0x4c, 0xd2, 0x65, 0x1c, 0x96, 0x0e,
	0x26, 0xc6, 0x31, 0x60, 0x0b, 0xbd, 0xd8, 0xe0, 0x45, 0x18, 0x99, 0xc4,
	0xb1, 0x11, 0x80, 0x08, 0x69, 0xed, 0xe4, 0xa7, 0x43, 0x06, 0xca, 0x6e,
	0x84, 0x91, 0x28, 0x5f, 0x27, 0xc9, 0x87, 0x23, 0xf4, 0x05, 0x48, 0xc8,
	0xe3, 0x36, 0x6b, 0x3b, 0x64, 0xb4, 0x18, 0x80, 0x32, 0x12, 0x61, 0x6b,
	0x4a, 0x7d, 0xbe, 0xbb, 0x14, 0x61, 0x7c, 0x62, 0xee, 0x8f, 0x46, 0x1f,
	0x97, 0xfc, 0xb9, 0x37, 0xd0, 0x1c, 0xa9, 0x21, 0x83, 0x20, 0x68, 0x0e,
	0x0a, 0x08, 0x00, 0xe3, 0xa9, 0xe3, 0x59, 0x2f, 0xca, 0xb0, 0x04, 0x81,
	0x72, 0x00, 0x80, 0xa3, 0x59, 0x47, 0xb9, 0xa0, 0xf6, 0x9e, 0x14, 0x5d,
	0x2c, 0x00, 0xae, 0x40, 0x98, 0x86, 0xb2, 0x97, 0x18, 0xb5, 0x58, 0xe4,
	0xa4, 0xb0, 0xbb, 0x0b, 0x25, 0x2d, 0xce, 0xe9, 0xad, 0xc1, 0x8a, 0x4c,
	0xea, 0x30, 0xb9, 0x4e, 0x40, 0xfc, 0xee, 0x65, 0x08, 0xe3, 0x81, 0x28,
	0xb0, 0x04, 0xb8, 0x1d, 0xc8, 0xc5, 0x13, 0x99, 0x4a, 0x0f, 0x2a, 0x51,
	0x72, 0x80, 0x6a, 0x54, 0xca, 0x81, 0xf3, 0x10, 0x7d, 0x04, 0xc5, 0xf2,
	0x6a, 0xd8, 0xd9, 0xd9, 0x9e, 0x4c, 0x7f, 0x98, 0xd0, 0x04, 0x9c, 0xb2,
	0xd9, 0x16, 0xdb, 0x7d, 0x1e, 0x80, 0x18, 0xdb, 0x5b, 0xfd, 0xc4, 0x76,
	0x13, 0x49, 0xca, 0x75, 0x73, 0x06, 0x12, 0x89, 0xdd, 0x8a, 0xf2, 0xb9,
	0x56, 0x3a, 0x95, 0x92, 0xe7, 0x0e, 0xc0, 0x1b, 0x19, 0x68, 0xb8, 0xd6,
	0x13, 0x65, 0x2f, 0x80, 0xe4, 0xb9, 0xbd, 0xf1, 0x32, 0x84, 0x18, 0xb5,
	0x44, 0x38, 0x4c, 0x1a, 0x45, 0x60, 0xed, 0xd1, 0x8a, 0x92, 0xcf, 0x4f,
	0xe9, 0xf3, 0x7a, 0xb7, 0x17, 0x51, 0xf2, 0xe3, 0x0d, 0xb9, 0x11, 0x14,
	0x94, 0x77, 0x35, 0xec, 0x3c, 0x27, 0xb6, 0x9b, 0x0f, 0x84, 0x51, 0x46,
	0x83, 0x44, 0xe2, 0x3a, 0x29, 0xd7, 0x70, 0xe9, 0xcb, 0x00, 0x52, 0xe8,
	0xbe, 0x44, 0x83, 0x27, 0xbb, 0xcd, 0x7a, 0x28, 0x0f, 0xbc, 0xef, 0xa1,
	0xba, 0x2a, 0x5d, 0x43, 0x4e, 0x43, 0xd7, 0x5f, 0x33, 0xd9, 0x44, 0xa3,
	0xbb, 0x11, 0x32, 0x11, 0x1a, 0x25, 0xcf, 0xcd, 0xc1, 0x4b, 0x31, 0x8a,
	0x93, 0xb0, 0xb1, 0x18, 0x07, 0xbc, 0x86, 0x97, 0x21, 0x28, 0x5b, 0x10,
	0x76, 0xe0, 0x23, 0x1b, 0x65, 0x00, 0xc4, 0x54, 0xec, 0xc0, 0x28, 0x53,
	0x66, 0x26, 0x27, 0x04, 0x3f, 0xef, 0x00, 0x45, 0x28, 0x6f, 0x20, 0x9a,
	0x6e, 0x54, 0xdf, 0x03, 0xd0, 0x72, 0xfe, 0x1e, 0x7c, 0x27, 0x00, 0xfa,
	0x81, 0xfe, 0x19, 0x77, 0xd2, 0x3b, 0x25, 0xd7, 0x7d, 0x4b, 0xbf, 0x75,
	0xea, 0x75, 0x8b, 0x13, 0x01, 0xf6, 0x25, 0x66, 0x57, 0x29, 0x08, 0xdc,
	0x46, 0x75, 0xd5, 0x2e, 0x60, 0x61, 0x97, 0x13, 0x04, 0x50, 0xee, 0x01,
	0x3e, 0x02, 0x0e, 0x02, 0x3d, 0xb0, 0x18, 0x41, 0x8c, 0x9d, 0xc4, 0xcb,
	0xea, 0xeb, 0xd3, 0x78, 0xdd, 0x08, 0x54, 0x01, 0x5f, 0x01, 0x02, 0x7a,
	0x47, 0x1b, 0xad, 0x1d, 0x98, 0x61, 0x12, 0xdc, 0x86, 0xf4, 0x7b, 0x1a,
	0xb8, 0xc8, 0x64, 0xff, 0x0b, 0x80, 0xee, 0x9a, 0x72, 0x02, 0x38, 0x16,
	0x97, 0xf1, 0x8d, 0xf1, 0x2a, 0x22, 0x83, 0x2a, 0x29, 0x08, 0x3c, 0xd5,
	0x4a, 0x63, 0x01, 0x88, 0xdf, 0x2d, 0x47, 0xf5, 0x53, 0xe0, 0x6a, 0x60,
	0x84, 0x5c, 0xbe, 0xac, 0xf5, 0x8d, 0x75, 0x05, 0x9f, 0x69, 0xc8, 0x79,
	0x18, 0x64, 0x91, 0xe9, 0xb4, 0x40, 0xc3, 0xce, 0x46, 0x0d, 0x39, 0x13,
	0x81, 0x6d, 0xa7, 0xf1, 0x39, 0x46, 0xd8, 0x29, 0x22, 0x26, 0x53, 0x01,
	0x50, 0xae, 0x4f, 0xd2, 0x15, 0xa2, 0x1a, 0x00, 0x7e, 0xc4, 0x9b, 0x76,
	0xaf, 0x86, 0x9e, 0x6c, 0x49, 0xcc, 0xd8, 0xca, 0xe1, 0x24, 0xbb, 0xc3,
	0xa6, 0xbf, 0x43, 0x88, 0x8c, 0x05, 0xea, 0x80, 0x6c, 0x54, 0x5f, 0x93,
	0x02, 0xf7, 0x03, 0x19, 0xf1, 0x62, 0x4f, 0xf3, 0x25, 0x65, 0x13, 0x50,
	0x8b, 0x30, 0x81, 0x9e, 0xf8, 0xcd, 0x98, 0xec, 0x1a, 0x94, 0x2a, 0x73,
	0x3f, 0x60, 0x3a, 0xed, 0xdb, 0xa1, 0x8f, 0xb0, 0x5b, 0x15, 0xc5, 0x67,
	0x55, 0x9b, 0xf6, 0xc5, 0x49, 0x7c, 0x37, 0x01, 0x16, 0xc2, 0x0e, 0x0d,
	0x4e, 0xff, 0xc3, 0xe8, 0xeb, 0x8d, 0xb6, 0x77, 0x12, 0x4b, 0xfc, 0x39,
	0x4a, 0xbd, 0x56, 0x94, 0xbe, 0x0d, 0x9e, 0x3c, 0x33, 0x8b, 0x2b, 0xca,
	0x68, 0x4e, 0x36, 0x96, 0x5a, 0x00, 0x1a, 0x2e, 0xdd, 0x84, 0x97, 0x81,
	0x5a, 0xe1, 0x2c, 0x37, 0x75, 0x8c, 0x0c, 0x99, 0xdb, 0x5f, 0xfc, 0x6e,
	0xb1, 0x48, 0xf2, 0x28, 0xf9, 0xc7, 0x80, 0x4f, 0xc6, 0xef, 0x5d, 0xd8,
	0x42, 0x29, 0x8d, 0x00, 0x34, 0x47, 0x53, 0xf9, 0x34, 0x01, 0x87, 0x50,
	0xc6, 0x8a, 0xed, 0x5e, 0x69, 0x64, 0x47, 0x4c, 0x1f, 0xfd, 0x01, 0xc4,
	0x5e, 0xe1, 0x03, 0xfa, 0x18, 0xae, 0x23, 0x00, 0x1a, 0x9a, 0xf5, 0x93,
	0x86, 0x9d, 0x87, 0x50, 0x69, 0x2d, 0xd7, 0xc2, 0xc4, 0x98, 0xd4, 0x9d,
	0xce, 0xf1, 0x36, 0x5d, 0x78, 0x23, 0x0b, 0x10, 0xde, 0xc4, 0xef, 0x96,
	0x74, 0x3a, 0xe8, 0xee, 0x84, 0xb0, 0x14, 0x65, 0x26, 0xe0, 0x41, 0x98,
	0x6b, 0x64, 0xfb, 0x8d, 0xf6, 0x52, 0x00, 0xa2, 0x35, 0xfd, 0x89, 0x0f,
	0xb9, 0x06, 0xd2, 0xda, 0xed, 0xde, 0x7c, 0xfa, 0x89, 0x79, 0xba, 0x32,
	0xf5, 0x19, 0xcf, 0x50, 0x77, 0x38, 0x30, 0x06, 0x38, 0x41, 0x8c, 0x95,
	0x67, 0x1c, 0xa7, 0xbd, 0xc2, 0x47, 0xcf, 0x1a, 0x0f, 0x8a, 0x07, 0x01,
	0x04, 0xaf, 0x14, 0xba, 0x19, 0xa8, 0x08, 0x1d, 0xff, 0xc9, 0x35, 0xd0,
	0x8b, 0x35, 0xd4, 0x51, 0x82, 0x32, 0x5a, 0xec, 0xd9, 0xd7, 0x61, 0xb1,
	0x1e, 0x65, 0x2a, 0xf0, 0xa8, 0xf8, 0x03, 0x9b, 0xf1, 0x32, 0xc6, 0xd0,
	0x7c, 0x4c, 0x84, 0x99, 0x62, 0xbb, 0xe7, 0x81, 0xf5, 0x0a, 0xc4, 0x7e,
	0x02, 0x9e, 0x35, 0x3c, 0x07, 0x52, 0xef, 0x5d, 0xa3, 0x0c, 0x02, 0xea,
	0x11, 0x99, 0x7f, 0x76, 0x7f, 0x13, 0x35, 0x1f, 0x51, 0x47, 0xa3, 0x59,
	0xc0, 0x41, 0x59, 0x46, 0x1d, 0x8d, 0xa0, 0x57, 0x75, 0xc6, 0x5b, 0xb7,
	0x38, 0x91, 0xc4, 0x57, 0xd4, 0xd8, 0x02, 0x2a, 0x9c, 0x2f, 0x11, 0xbe,
	0x04, 0xb2, 0x10, 0xdd, 0x80, 0xf2, 0x20, 0xd0, 0x82, 0x5a, 0xcb, 0x50,
	0x04, 0x18, 0x0b, 0xb1, 0x9d, 0xc0, 0xef, 0xc0, 0x14, 0x43, 0xb3, 0x32,
	0xf5, 0xf1, 0x47, 0xd8, 0x59, 0x23, 0xf6, 0xbc, 0xcd, 0x64, 0xb6, 0x1c,
	0x6d, 0xa7, 0xfa, 0x16, 0x00, 0x8f, 0x55, 0xf7, 0xb7, 0xb1, 0xfc, 0x8a,
	0x68, 0x10, 0x35, 0xa5, 0x14, 0xdf, 0x05, 0x05, 0x51, 0x7e, 0x40, 0xc8,
	0x44, 0xdb, 0x4c, 0x12, 0x06, 0xf2, 0x3b, 0x68, 0x10, 0xe1, 0xbb, 0x38,
	0x9f, 0xaf, 0x09, 0x5a, 0x82, 0x08, 0xfb, 0x51, 0x7e, 0x06, 0x82, 0xe6,
	0x0e, 0x99, 0xac, 0xa6, 0x9e, 0xfb, 0x81, 0x0b, 0xf1, 0xcf, 0xce, 0xc5,
	0x47, 0x11, 0xcd, 0xbc, 0x0a, 0xc4, 0xf7, 0xae, 0xc8, 0x7c, 0x0d, 0x97,
	0xec, 0x90, 0x82, 0x39, 0xbf, 0xa1, 0x51, 0x2f, 0x30, 0x0a, 0xe8, 0x4b,
	0xfc, 0x8c, 0x6a, 0x25, 0x83, 0x72, 0x96, 0xfc, 0x05, 0x61, 0xcc, 0xba,
	0xf1, 0x83, 0x0d, 0x5f, 0x8b, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
	0x44, 0xae, 0x42, 0x60, 0x82,
};

/*---------------------------------------------------------------------------
  Translucent overlay (overlay.png)
  ---------------------------------------------------------------------------*/
const uint8_t overlay_png[] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
	0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06,
	0x08, 0x06, 0x00, 0x00, 0x00, 0xe0, 0xcc, 0xef, 0x48, 0x00, 0x00, 0x00,
	0x06, 0x62, 0x4b, 0x47, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf9,
	0x43, 0xbb, 0x7f, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00,
	0x00, 0x0b, 0x12, 0x00, 0x00, 0x0b, 0x12, 0x01, 0xd2, 0xdd, 0x7e, 0xfc,
	0x00, 0x00, 0x00, 0x09, 0x76, 0x70, 0x41, 0x67, 0x00, 0x00, 0x00, 0x06,
	0x00, 0x00, 0x00, 0x06, 0x00, 0x95, 0xd1, 0xf9, 0x92, 0x00, 0x00, 0x00,
	0x11, 0x49, 0x44, 0x41, 0x54, 0x08, 0xd7, 0x63, 0x60, 0x60, 0x60, 0x68,
	0xc0, 0x81, 0x07, 0x4e, 0x02, 0x00, 0x2b, 0xe1, 0x12, 0x01, 0x67, 0x99,
	0x43, 0x46, 0x00, 0x00, 0x00, 0x25, 0x74, 0x45, 0x58, 0x74, 0x63, 0x72,
	0x65, 0x61, 0x74, 0x65, 0x2d, 0x64, 0x61, 0x74, 0x65, 0x00, 0x32, 0x30,
	0x31, 0x30, 0x2d, 0x30, 0x37, 0x2d, 0x32, 0x38, 0x54, 0x30, 0x33, 0x3a,
	0x31, 0x32, 0x3a, 0x34, 0x37, 0x2d, 0x30, 0x34, 0x3a, 0x30, 0x30, 0x94,
	0x34, 0xe7, 0xc4, 0x00, 0x00, 0x00, 0x25, 0x74, 0x45, 0x58, 0x74, 0x6d,
	0x6f, 0x64, 0x69, 0x66, 0x79, 0x2d, 0x64, 0x61, 0x74, 0x65, 0x00, 0x32,
	0x30, 0x31, 0x30, 0x2d, 0x30, 0x37, 0x2d, 0x32, 0x38, 0x54, 0x30, 0x33,
	0x3a, 0x31, 0x32, 0x3a, 0x34, 0x37, 0x2d, 0x30, 0x34, 0x3a, 0x30, 0x30,
	0xcb, 0x85, 0x91, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
	0xae, 0x42, 0x60, 0x82,
};

const uint8_t alert_png[1457] = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
	0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x1b,
	0x08, 0x06, 0x00, 0x00, 0x00, 0x1d, 0xc7, 0x8d, 0x9a, 0x00, 0x00, 0x00,
	0x06, 0x62, 0x4b, 0x47, 0x44, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0xa0,
	0xbd, 0xa7, 0x93, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00,
	0x00, 0x0b, 0x13, 0x00, 0x00, 0x0b, 0x13, 0x01, 0x00, 0x9a, 0x9c, 0x18,
	0x00, 0x00, 0x00, 0x07, 0x74, 0x49, 0x4d, 0x45, 0x07, 0xdf, 0x09, 0x19,
	0x0c, 0x14, 0x29, 0x6a, 0xbb, 0xc8, 0x63, 0x00, 0x00, 0x05, 0x3e, 0x49,
	0x44, 0x41, 0x54, 0x48, 0xc7, 0xbd, 0xd6, 0x6d, 0x6c, 0x95, 0x67, 0x19,
	0x07, 0xf0, 0xdf, 0xd3, 0x9e, 0xd3, 0x43, 0xa1, 0xb4, 0x74, 0x2d, 0xac,
	0xb4, 0x48, 0x8a, 0x90, 0x0d, 0x51, 0x70, 0x6c, 0x0c, 0x08, 0x38, 0x0b,
	0xc6, 0x97, 0x45, 0xcc, 0x98, 0x89, 0x4b, 0x9d, 0xc4, 0x98, 0xe9, 0x70,
	0xa0, 0xc2, 0xa2, 0x09, 0x1f, 0xd4, 0xcd, 0x80, 0xc6, 0x18, 0xd9, 0xc4,
	0x68, 0x74, 0x93, 0xc4, 0x90, 0x4d, 0x33, 0x03, 0x19, 0x03, 0x83, 0x8b,
	0xa2, 0x66, 0x63, 0x11, 0xc9, 0x3e, 0xcc, 0x64, 0x33, 0xf1, 0x83, 0x32,
	0xc6, 0x42, 0x18, 0xd8, 0xc1, 0x2c, 0x94, 0xae, 0xa7, 0x3d, 0xe7, 0x79,
	0xb9, 0xfd, 0x70, 0x9e, 0x9e, 0x56, 0x62, 0x0c, 0xa0, 0x78, 0x27, 0x77,
	0xfe, 0xd7, 0x73, 0x3f, 0xf7, 0xf5, 0x72, 0xff, 0xaf, 0xeb, 0x7e, 0x89,
	0x5c, 0x63, 0xfb, 0x0d, 0x0d, 0x8d, 0xdc, 0x8e, 0xee, 0x32, 0xcf, 0xdd,
	0xcd, 0xa5, 0xb5, 0x38, 0x72, 0x95, 0x76, 0x1a, 0xae, 0xd6, 0xf1, 0xef,
	0x27, 0xc4, 0x3b, 0xa6, 0xf1, 0xe2, 0x0c, 0x0e, 0x54, 0xd8, 0xb9, 0x80,
	0x29, 0x47, 0xfc, 0x9f, 0xda, 0xb3, 0xb4, 0x3c, 0xcf, 0x0b, 0x67, 0x09,
	0xd5, 0xaf, 0x6f, 0x09, 0x2f, 0x10, 0x1e, 0x64, 0x1d, 0xec, 0xbf, 0x9e,
	0x8e, 0x7f, 0x37, 0x81, 0x5f, 0x7a, 0x99, 0x10, 0x7e, 0x75, 0x20, 0x0b,
	0xc7, 0xff, 0x96, 0xbd, 0x41, 0xf8, 0x39, 0x7f, 0x45, 0x1b, 0x44, 0xd7,
	0xc3, 0xf9, 0xe1, 0x09, 0x9c, 0xf3, 0x3c, 0xc3, 0xc3, 0xfd, 0x77, 0x86,
	0xf0, 0xea, 0xab, 0x59, 0x78, 0xe5, 0x95, 0x10, 0xf6, 0x3c, 0x9e, 0x1d,
	0x23, 0x7c, 0x9b, 0x5d, 0x10, 0xae, 0x47, 0x0d, 0x24, 0x13, 0xf8, 0xd3,
	0xb9, 0xb4, 0xb4, 0x6c, 0xdb, 0x11, 0x0c, 0x0f, 0x47, 0x06, 0x07, 0x99,
	0xf5, 0x0e, 0x4b, 0x16, 0xb7, 0xeb, 0x65, 0xe3, 0x52, 0x56, 0x46, 0xf8,
	0xc8, 0xff, 0x32, 0x80, 0x5f, 0xe3, 0x63, 0x38, 0x44, 0x7f, 0x1b, 0x1f,
	0x98, 0xff, 0xf8, 0xa3, 0x34, 0x96, 0x22, 0x17, 0x2f, 0x32, 0x34, 0xc4,
	0xe0, 0x60, 0xd4, 0xf2, 0x99, 0xaf, 0x85, 0x5b, 0x98, 0xb6, 0x81, 0x9d,
	0x5a, 0xa7, 0xfb, 0xed, 0x15, 0x06, 0xd0, 0x78, 0x05, 0x05, 0x67, 0x1d,
	0x0e, 0xd0, 0xd2, 0xc0, 0xbe, 0xdb, 0x98, 0xd9, 0xfc, 0xad, 0xef, 0x72,
	0xf1, 0x02, 0x85, 0x02, 0xa5, 0x12, 0xa7, 0x4f, 0x73, 0xe1, 0x42, 0xd4,
	0xe1, 0x52, 0x74, 0xfe, 0xd4, 0x40, 0x67, 0x67, 0xa5, 0x3a, 0xf4, 0x12,
	0x2f, 0xc1, 0x34, 0xc4, 0xd7, 0xca, 0xc0, 0xb3, 0xf9, 0xca, 0x21, 0xe3,
	0xa1, 0x5e, 0x7a, 0x6f, 0xf8, 0xe5, 0x7e, 0x86, 0x2e, 0x92, 0xa6, 0x2c,
	0x5d, 0xca, 0x9a, 0x35, 0xcc, 0x9e, 0xcd, 0xf9, 0xf3, 0x1a, 0x3a, 0x16,
	0x84, 0xdb, 0x99, 0x36, 0x8f, 0xcf, 0x63, 0x3e, 0xbc, 0xfd, 0xdf, 0xa4,
	0x20, 0xcd, 0x71, 0x2f, 0x2b, 0xda, 0xb8, 0x77, 0xc1, 0xe7, 0xee, 0x29,
	0x6a, 0xeb, 0x0c, 0x86, 0x86, 0x54, 0x0a, 0x05, 0x3b, 0x77, 0xef, 0x76,
	0xff, 0xd6, 0xad, 0x4e, 0xb6, 0xb5, 0x31, 0x3c, 0xcc, 0xeb, 0x27, 0xf4,
	0xe0, 0x56, 0x96, 0x6c, 0x66, 0x23, 0x0a, 0x51, 0xce, 0xe0, 0x55, 0xb7,
	0x43, 0x13, 0xce, 0x5b, 0x0e, 0xf2, 0xc4, 0x6b, 0x84, 0x70, 0x70, 0x7f,
	0x08, 0x07, 0x0f, 0x86, 0x70, 0xe8, 0x50, 0xf8, 0xd3, 0xd1, 0xa3, 0x21,
	0x2f, 0xf8, 0xa0, 0xab, 0x2b, 0x84, 0x27, 0x9f, 0x0c, 0x81, 0x10, 0xc8,
	0xce, 0x13, 0x7e, 0xc4, 0xc0, 0x62, 0x56, 0x5d, 0x73, 0x11, 0xde, 0x95,
	0x63, 0xcc, 0x9a, 0x99, 0x6c, 0x98, 0xbb, 0xfd, 0xab, 0x44, 0x85, 0xe0,
	0xad, 0xb7, 0x58, 0xb1, 0x42, 0x3a, 0x75, 0xea, 0xc4, 0xe4, 0x81, 0x01,
	0xa1, 0xbf, 0x9f, 0x85, 0x0b, 0x21, 0xea, 0xc4, 0x6d, 0xdc, 0xf8, 0x7e,
	0xbe, 0x8c, 0x96, 0x6b, 0x4e, 0xc1, 0x53, 0xb4, 0xcd, 0x60, 0xc7, 0x3c,
	0x0a, 0x85, 0x77, 0xbd, 0x37, 0x18, 0x1c, 0x8c, 0xf4, 0xf6, 0x32, 0x6b,
	0x96, 0xa6, 0x86, 0x7f, 0x55, 0x8d, 0x4a, 0x25, 0x9e, 0x79, 0x66, 0xfc,
	0x33, 0xdc, 0x82, 0x25, 0x7c, 0xa2, 0x8b, 0x0f, 0x43, 0xd7, 0xd5, 0x06,
	0x70, 0x00, 0x05, 0xee, 0x9b, 0xc9, 0xad, 0xdd, 0x3f, 0x78, 0x84, 0x4a,
	0x1c, 0x19, 0x1e, 0x66, 0xed, 0x5a, 0xd0, 0xd4, 0xd4, 0x54, 0x9f, 0xdb,
	0xde, 0xde, 0x5e, 0x13, 0x16, 0x2d, 0xa2, 0xbf, 0x1f, 0xa2, 0x29, 0x58,
	0x8d, 0x7b, 0xf9, 0x21, 0x5a, 0x07, 0xae, 0x34, 0x80, 0xa7, 0x73, 0x1c,
	0x63, 0x4e, 0x33, 0xdf, 0x5b, 0xd4, 0xb7, 0x90, 0x99, 0x3d, 0xbc, 0x39,
	0x40, 0x5f, 0x1f, 0x8d, 0xb5, 0x9d, 0x5b, 0x2c, 0x16, 0xeb, 0x3a, 0xcb,
	0x96, 0x2d, 0x9b, 0x30, 0xb0, 0x6b, 0x57, 0x5d, 0x7c, 0x77, 0x8d, 0x89,
	0x9e, 0xd5, 0xec, 0xb8, 0xa2, 0x14, 0x3c, 0x8d, 0x7b, 0xc6, 0x39, 0xe4,
	0x27, 0xf3, 0x69, 0x98, 0x7e, 0xd3, 0x3b, 0x83, 0x93, 0x27, 0xe8, 0xec,
	0x14, 0x6e, 0xbe, 0xb9, 0x3e, 0x77, 0x32, 0x03, 0xbd, 0xbd, 0xbd, 0x13,
	0x46, 0xba, 0xba, 0x78, 0xf8, 0xe1, 0x71, 0x1b, 0xd1, 0x2a, 0x2c, 0xe7,
	0x41, 0xac, 0x84, 0x93, 0xff, 0x29, 0xe7, 0xfb, 0x72, 0xdc, 0xc3, 0x86,
	0xc3, 0x84, 0x61, 0xb2, 0x4b, 0x84, 0x31, 0xc2, 0xe8, 0x8b, 0x47, 0x43,
	0xb5, 0x1a, 0x87, 0xf1, 0x76, 0xe6, 0xcc, 0x99, 0xfa, 0x2e, 0xd8, 0xbe,
	0x7d, 0x7b, 0x7d, 0x3c, 0x0d, 0x21, 0x8c, 0x9e, 0x38, 0x11, 0xde, 0x24,
	0x1c, 0x23, 0xfc, 0x91, 0xec, 0x30, 0x61, 0x35, 0xc7, 0x30, 0xc5, 0xf8,
	0x01, 0x71, 0x39, 0x03, 0xfb, 0xd0, 0x5f, 0x73, 0x7e, 0x63, 0x81, 0x1d,
	0x4b, 0x72, 0xeb, 0x09, 0x86, 0x50, 0xee, 0x99, 0x2b, 0x49, 0xd3, 0xba,
	0x62, 0xa9, 0x54, 0xaa, 0xcb, 0x3d, 0x3d, 0x3d, 0x13, 0x67, 0x47, 0x1c,
	0xab, 0x76, 0x77, 0xbb, 0xf0, 0x8d, 0x6d, 0x46, 0x30, 0x48, 0xd4, 0x4e,
	0xf8, 0x38, 0xcb, 0xb1, 0x15, 0x5e, 0xfb, 0x77, 0x01, 0xf4, 0xe7, 0x58,
	0x66, 0xc7, 0x62, 0xe6, 0xb5, 0x10, 0x52, 0xa2, 0x04, 0xf1, 0xc1, 0xa7,
	0x64, 0xd3, 0x5b, 0x25, 0x71, 0x55, 0x1c, 0xd7, 0x0e, 0xd6, 0x8e, 0x8e,
	0x0e, 0x1b, 0x37, 0x6e, 0x04, 0x7d, 0x7d, 0x7d, 0x35, 0xe7, 0x69, 0x2a,
	0x49, 0x12, 0x69, 0xa5, 0xa2, 0xe5, 0xfe, 0x2d, 0x9a, 0x73, 0x9b, 0x7f,
	0xc7, 0x0a, 0x0a, 0x77, 0x72, 0x1f, 0x16, 0x53, 0x13, 0xea, 0x57, 0xf7,
	0x2f, 0xf0, 0x29, 0x3c, 0xc6, 0x87, 0xe6, 0xf0, 0xc4, 0x2a, 0xba, 0x0b,
	0xf9, 0xea, 0x47, 0x17, 0x52, 0x7d, 0xee, 0xac, 0xe6, 0xc6, 0x06, 0xcd,
	0xa5, 0x92, 0x52, 0xa9, 0xa4, 0x50, 0x28, 0x28, 0x16, 0x8b, 0xd2, 0x34,
	0x35, 0x32, 0x32, 0xa2, 0xb5, 0xb5, 0x55, 0x96, 0x65, 0xe2, 0x38, 0x56,
	0xa9, 0x54, 0x54, 0x2a, 0x15, 0x63, 0x71, 0x62, 0xe8, 0xe8, 0x11, 0xaf,
	0x7f, 0x72, 0x83, 0x0a, 0x4a, 0x18, 0xc6, 0x06, 0x1e, 0xcb, 0x99, 0x48,
	0xa1, 0x61, 0xcf, 0x84, 0xf3, 0x96, 0x22, 0x0f, 0x2c, 0xcc, 0x9d, 0xa7,
	0x79, 0x1f, 0xdd, 0xfb, 0xb2, 0x6c, 0x6c, 0x54, 0x52, 0xad, 0xad, 0xbe,
	0x5a, 0xad, 0x4a, 0x92, 0xc4, 0xb9, 0x73, 0xe7, 0x14, 0x0a, 0x05, 0x6d,
	0x6d, 0x6d, 0x36, 0x6d, 0xda, 0xa4, 0x5c, 0x2e, 0xab, 0xe6, 0x73, 0xe2,
	0x38, 0x16, 0x8f, 0x8d, 0x9a, 0x72, 0xc7, 0x5a, 0xad, 0xb9, 0x9d, 0x61,
	0x42, 0x1b, 0x1e, 0xa8, 0x91, 0xfd, 0xc1, 0x7a, 0x0a, 0x3e, 0x3b, 0x71,
	0xd9, 0xdc, 0xb5, 0x80, 0xbb, 0x6f, 0xa8, 0xc9, 0x21, 0x43, 0xf9, 0x3b,
	0xdb, 0x84, 0x8e, 0x99, 0xb2, 0x34, 0x93, 0x66, 0x99, 0x38, 0x49, 0xc4,
	0x69, 0x2a, 0x4e, 0x12, 0xa7, 0x4e, 0x9f, 0xae, 0xe7, 0x71, 0xf7, 0xee,
	0xdd, 0xca, 0xa3, 0xa3, 0x92, 0x34, 0xad, 0xf7, 0x2c, 0x04, 0xa1, 0x5c,
	0xd6, 0x7d, 0xfc, 0xb8, 0x46, 0x54, 0x88, 0x46, 0x09, 0x2b, 0xe9, 0xc0,
	0x66, 0x35, 0x54, 0x80, 0xef, 0x33, 0xa7, 0x9d, 0xaf, 0xcc, 0xa5, 0xb1,
	0x48, 0xa8, 0x12, 0x65, 0x88, 0xd7, 0xac, 0x17, 0x9d, 0x3a, 0x21, 0x84,
	0x20, 0x8e, 0x1a, 0x45, 0xc5, 0xa2, 0x50, 0x2c, 0x0a, 0xa5, 0x26, 0x5d,
	0x51, 0xe6, 0xa3, 0xf9, 0x5b, 0xe1, 0xd1, 0x2d, 0x9b, 0x25, 0x03, 0x67,
	0x55, 0xd3, 0x4c, 0x32, 0xce, 0x52, 0x96, 0x90, 0x11, 0x35, 0x4f, 0x35,
	0x1d, 0x17, 0xf3, 0x82, 0x9c, 0x85, 0x2f, 0xb2, 0xfe, 0xc7, 0x1c, 0x68,
	0xe4, 0x67, 0xd1, 0x23, 0x44, 0xad, 0x3c, 0xb4, 0x88, 0x6f, 0x2e, 0x21,
	0x24, 0x79, 0xe1, 0x25, 0xf9, 0x3d, 0x9e, 0xd4, 0x18, 0xa9, 0xf7, 0x9c,
	0x2d, 0x51, 0xde, 0xd3, 0x7c, 0xb7, 0x94, 0x2f, 0xd3, 0x49, 0x73, 0x0c,
	0x35, 0xc7, 0xca, 0xa8, 0xd6, 0xde, 0x07, 0xa1, 0x44, 0xf4, 0x69, 0x4e,
	0xe1, 0x7d, 0x8d, 0xeb, 0xb9, 0xa9, 0x83, 0xbd, 0xcb, 0xf3, 0x33, 0x2e,
	0xcd, 0x0b, 0xf3, 0xf2, 0x77, 0x5d, 0xc8, 0x1d, 0xa7, 0xb9, 0xa1, 0x0a,
	0x46, 0x73, 0xc3, 0x23, 0xb5, 0x93, 0xd3, 0x58, 0x3e, 0x56, 0x99, 0x24,
	0x97, 0x27, 0x05, 0x1e, 0xd5, 0xbe, 0xa3, 0xd9, 0x84, 0x05, 0xcc, 0xf8,
	0x03, 0x51, 0x01, 0xad, 0x55, 0x9a, 0xfe, 0x51, 0x8b, 0x38, 0xca, 0x72,
	0x67, 0x69, 0xae, 0x18, 0x26, 0xad, 0x3e, 0xbd, 0xec, 0x7b, 0xf2, 0xd8,
	0x38, 0x8e, 0x33, 0x53, 0xc8, 0xb1, 0x38, 0xc9, 0x56, 0x8a, 0xa9, 0xb5,
	0x47, 0x4a, 0x54, 0xad, 0xc5, 0xf4, 0x9e, 0x42, 0x95, 0x3f, 0xbf, 0xc1,
	0x17, 0xfe, 0xc2, 0xba, 0x31, 0xd2, 0x71, 0xca, 0x27, 0xd3, 0x19, 0xe7,
	0x63, 0x63, 0x93, 0x8c, 0x8d, 0xe6, 0xff, 0x2a, 0x93, 0xd8, 0x78, 0x1b,
	0x03, 0x57, 0xfe, 0x16, 0x1d, 0xc1, 0x63, 0xff, 0x04, 0xc7, 0x7b, 0x48,
	0x6d, 0x52, 0x35, 0xf5, 0x5f, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
	0x44, 0xae, 0x42, 0x60, 0x82
};

struct httpdobj www_img[] = {
	{ .oid = "thinkos16x16.png", .typ = OBJ_STATIC_PNG, .lvl = 255, 
		.len = sizeof(thinkos16x16_png) - 1, .ptr = thinkos16x16_png },
	{ .oid = "thinkos47x20.png", .typ = OBJ_STATIC_PNG, .lvl = 255,
		.len = sizeof(thinkos47x20_png) - 1, .ptr = thinkos47x20_png },
	{ .oid = "thinkos57x24.png", .typ = OBJ_STATIC_PNG, .lvl = 255, 
		.len = sizeof(thinkos57x24_png) - 1, .ptr = thinkos57x24_png },
	{ .oid = "alert.png", .typ = OBJ_STATIC_PNG, .lvl = 255,
		.len = sizeof(alert_png) - 1, .ptr = alert_png },
	{ .oid = "overlay.png", .typ = OBJ_STATIC_PNG, .lvl = 255, 
		.len = sizeof(overlay_png) - 1, .ptr = overlay_png },
	{ .oid = NULL, .typ = 0, .lvl = 0, .len = 0, .ptr = NULL }
};

