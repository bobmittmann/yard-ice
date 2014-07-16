/*****************************************************************************
 * Object pool public header file
 *****************************************************************************/

#ifndef __LIST_H__
#define __LIST_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

struct u16_list {
	uint16_t cnt;
	uint16_t key[];
};

struct ptr_list {
	uintptr_t cnt;
	uintptr_t key[];
};

#define LIST_LEN(LST) (LST[0])

#define PTR_LIST_LEN(LST) ((uintptr_t)LST[0])

#ifdef __cplusplus
extern "C" {
#endif

void u8_list_insert(uint8_t lst[], unsigned int key);

bool u8_list_contains(const uint8_t lst[], unsigned int key);

int u8_list_indexof(const uint8_t lst[], unsigned int key);

void u8_list_remove(uint8_t lst[], unsigned int key);

void u8_list_subtract(uint8_t from[], uint8_t lst[]);

void u8_list_dump(FILE * f,  uint8_t lst[]);

void u8_list_init(uint8_t lst[]);



void u16_list_insert(uint16_t lst[], unsigned int key);

bool u16_list_contains(const uint16_t lst[], unsigned int key);

int u16_list_indexof(const uint16_t lst[], unsigned int key);

void u16_list_remove(uint16_t lst[], unsigned int key);

void u16_list_subtract(uint16_t from[], uint16_t lst[]);

void u16_list_dump(FILE * f,  uint16_t lst[]);

void u16_list_init(uint16_t lst[]);



void ptr_list_insert(void * lst[], void * key);

bool ptr_list_contains(const void * lst[], void * key);

int ptr_list_indexof(const void * lst[], void * key);

void ptr_list_remove(void * lst[], void * key);

void ptr_list_subtract(void * from[], void * lst[]);

void ptr_list_dump(FILE * f,  void * lst[]);

void ptr_list_init(void * lst[]);

#ifdef __cplusplus
}
#endif	

#endif /* __LIST_H__ */

