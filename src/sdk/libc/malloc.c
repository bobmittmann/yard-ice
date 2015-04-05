/* 
 * Copyright(c) 2005-2012 BORESTE (www.boreste.com). All Rights Reserved.
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
 * @file memset.c
 * @brief YARD-ICE libc
 * @comment:Implementation of a variable-size memory partitioning allocation 
 			scheme where memory is accessed in chunks.
 * @author Robinson Mittmann <bobmittmann@gmail.com>
 * @author Carlos Augusto Vieira e Vieira <carlos.vieira@boreste.com>
 */ 


#ifdef CONFIG_H
#include "config.h"
#endif

#ifdef MALLOC_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif

#include <stdlib.h>
#include <string.h>		/* for memcpy() */
#include <unistd.h>		/* for sbrk() */

#include <sys/dcclog.h>

/* Number of bits which define chunk size. */
#ifndef MALLOC_CHUNK_BITS
#define MALLOC_CHUNK_BITS		(3)
#endif
#define CHUNK_BYTES 	(1 << MALLOC_CHUNK_BITS)

#if (MALLOC_CHUNK_BITS < 2)
#error MALLOC_CHUNK_BITS _must_ be larger than or equal to 2.
#endif

struct chunk {
	uint8_t data[CHUNK_BYTES];
};

typedef struct chunk chaddr_t;
typedef uint32_t chsize_t;

/*	Each memory block has a 1-chunk-size header describing how many chunks 
	it occupies. */
/*	We allocate at least 2 chunks each time: 1 for header and 1+ for data. */
struct mem {
	chsize_t size;
	chaddr_t data[0];
};

/*	Since holes hold no data, we use that extra chunk to our own purposes.
	I.e. the hole header occupies 2 chunks (1 more than alloc_hdr). */ 
struct hole {
	chsize_t size;	/* Refers to the useful size of the hole. */
	struct hole * next;
/*	chaddr_t size_next; */
};

extern uint32_t __heap_start;
extern uint32_t __heap_end;

struct {
	struct hole * holes;	/* Address of the head of our holes list. */
#ifdef ENABLE_STATS
	unsigned int used;
	unsigned int size;
#endif
} heap = { NULL
#ifdef ENABLE_STATS
	, 0, 0 
#endif
};

#ifdef ENABLE_STATS
unsigned int heap_usage(void)
{
	return heap.used;
}

unsigned int heap_size(void)
{
	return heap.size;
}
#endif

static inline struct chunk * chunk_align(void * ptr)
{
	return (struct chunk *) (((size_t)ptr + CHUNK_BYTES - 1) & 
							 ~(CHUNK_BYTES - 1));
}

static inline size_t chunks2bytes(chsize_t size)
{
	return (size_t) (size << MALLOC_CHUNK_BITS);
}

static inline chsize_t bytes2chunks(size_t bytes)
{
	return (chsize_t)((bytes + CHUNK_BYTES -1) >> MALLOC_CHUNK_BITS);
}

#if 0
#define MAIN_THREAD_STACK_SIZE (2048) /* 2KiB */

void __attribute__ ((constructor)) malloc_init(void)
{
	struct hole * hole;
	struct chunk * heap_begin;
	struct chunk * heap_end;
	unsigned int stack;
	
	heap_begin = chunk_align((void *)&__heap_start);
	stack = __sp();
	heap_end = chunk_align((void *)stack - MAIN_THREAD_STACK_SIZE);
	heap.holes = (struct hole *)heap_begin;

	DCC_LOG1(LOG_TRACE, "heap starts at 0x%p, ends at 0x%p (%lu bytes long).",
		heap_begin, heap_end, chunks2bytes(heap_end - heap_begin));

#ifdef ENABLE_STATS
	heap.size = chunks2bytes(heap_end - heap_begin);
	heap.used = 0;
#endif
	hole = heap.holes;
	hole->size = heap_end - heap_begin;
	hole->next = NULL;
}
#endif
uintptr_t heap_break = (uintptr_t)&__heap_start;

void * sbrk(intptr_t delta)
{
	uintptr_t data_end;
	uintptr_t data_max;
	uintptr_t data_min;
	uintptr_t new_end;

	/* current break */
	data_end = heap_break;
	/* maximum data segment */
	data_max = (uintptr_t)&__heap_end;
	/* minimum data segment */
	data_min = (uintptr_t)&__heap_start;

	new_end = data_end + delta;

	if (new_end > data_max)
		return (void *)-1;

	if (new_end < data_min)
		return (void *)-1;

	heap_break = new_end;

	return (void *)data_end;
}


void __attribute__ ((constructor)) malloc_init(void)
{
	void * data_seg;
	intptr_t delta;

	data_seg = sbrk(0);
	delta = (intptr_t)((void *)&__heap_start) - (intptr_t)data_seg;

	sbrk(delta);
}

#ifdef DEBUG
#include <stdio.h>

void hole_dump(void)
{
	struct hole * curr;

	puts("HOLE DUMP:");
	for (curr = heap.holes; curr != NULL; curr = curr->next) {
		printf("  0x%p(%u)\n", curr, curr->size);
	}
	puts("END.");
}
#endif

void * malloc(size_t bytes)
{
	chsize_t size;
	struct hole * hole;
	struct hole * prev;
	struct mem * mem;

	DCC_LOG1(LOG_INFO, "%u bytes", (int)bytes);

	if (!bytes) {		/* Why would you want to allocate 0 bytes? */
		DCC_LOG(LOG_WARNING, "tryed to allocate 0 bytes, aborting.");
		return NULL;
	}
	
	size = bytes2chunks(bytes + sizeof(struct mem));

	/* This is the First-Fit algorithm implementation. */
	
	/* This is redundant, since if heap is full, the for comparision fails and
	   an invalid hole_descriptor is returned, as expected.
	if (heap_is_full()) {
		hole.addr = HOLE_EOL;
		hole.prev = HOLE_EOL;
		return hole;
	} */
	
	DCC_LOG2(LOG_MSG, "%u chunks, starting at 0x%p", size, heap.holes);
	prev = NULL;
	hole = heap.holes;
	while (hole != NULL) {
		if (hole->size >= size) {
			DCC_LOG2(LOG_MSG, "hole found: 0x%p(%u)", hole, hole->size);

			mem = (struct mem *)hole;
			if (hole->size <= (size + 1)) {
				/* remove the hole from list */
				if (hole->size > size) {
					/* avoid a memory block with size less then 2 mem chunks 
					   to became a hole */
					size++;
				}
				if (prev)
					prev->next = hole->next; 
				else
					heap.holes = hole->next;

			} else {
				int rem;
				struct hole * next;

				rem = hole->size - size;
				next = hole->next;
				/* resize the hole */
				hole = (struct hole *)((struct chunk *)hole) + size;
				hole->size = rem;
				hole->next = next;
				if (prev)
					prev->next = hole;
				else
					heap.holes = hole;

				DCC_LOG3(LOG_MSG, "hole resized: 0x%p(%u). return: 0x%p", 
					hole, hole->size, mem->data);

			}

			mem->size = size;
#ifdef ENABLE_STATS
			heap.used += chunks2bytes(size);
#endif
			return mem->data;
		}
		prev = hole;
		hole = hole->next;
	}

	DCC_LOG1(LOG_TRACE, "no suitable hole, calling sbrk(%d)", 
		(int)chunks2bytes(size));

	/* try to increase the data space */
	if ((mem = (struct mem *)sbrk(chunks2bytes(size))) == (struct mem *)-1) {
		DCC_LOG(LOG_ERROR, "sbrk()");
		return NULL;
	}

	DCC_LOG1(LOG_MSG, "mem = 0x%p", mem);
	mem->size = size;

#ifdef ENABLE_STATS
	heap.size += chunks2bytes(heap_end - heap_begin);
	heap.used += chunks2bytes(size);
#endif


	return mem->data;
}

void free(void * ptr)
{
	struct hole * new;
	struct hole * prev;
	struct hole * next;
	struct mem * mem;

	mem = (struct mem *)(((size_t)ptr) - sizeof(struct mem));
	DCC_LOG2(LOG_INFO, "ptr=0x%p, mem=0x%p", ptr, mem);
	
	/* Validate pointer. */
	if (mem != (struct mem *)chunk_align(mem)) {
		DCC_LOG1(LOG_ERROR, "invalid pointer: 0x%p", mem);
		/* abort(); */
		return;
	}
	if (!(mem->size)) {
		DCC_LOG1(LOG_ERROR, "zero-sized mem block at 0x%p", mem);
		/* abort(); */
		return;
	}
#ifdef ENABLE_FLAGS
	if (IS_EMPTY(mem->flags)) {
		DCC_LOG1(LOG_ERROR, "can't free a freed memory: 0x%p", mem);
		/* abort(); */
		return;
	}
#endif

#ifdef ENABLE_STATS
	heap.used -= chunks2bytes(mem->size);
#endif
	new = (struct hole *)mem;

	if (heap.holes == NULL) {
		new->next = NULL;
		heap.holes = new;
		DCC_LOG(LOG_MSG, "heap was full, freed memory starting new hole list.");
		return;
	}

	/* Is there any hole before the new block? */
	if (heap.holes < new) {
		/* find it. */
		for (prev = heap.holes; (prev->next < new) && (prev->next != NULL); 
			 prev = prev->next);
		next = prev->next;
		
		DCC_LOG2(LOG_MSG, "prev: 0x%p < new < next: 0x%p", prev, next);

		/* Are they side by side? */
		if ((struct chunk *)new <= (struct chunk *)(prev + prev->size)) {
#ifdef DEBUG
			if ((struct chunk *)new < ((struct chunk *)prev + prev->size)) {
				/* TODO: if this happens, something is really bad, and a new 
				   size must be calculated. */
				DCC_LOG4(LOG_ERROR, "Hole overlap: 0x%p(%u), 0x%p(%u)", prev, 
					prev->size, new, new->size);
			}
#endif
			prev->size += new->size;
			new = prev;
			DCC_LOG(LOG_MSG, "catenated prev with new.");
		} else {
			new->next = prev->next;
			prev->next = new;
			DCC_LOG(LOG_MSG, "new hole added to the list.");
		}
		
		if (next == NULL) {
			DCC_LOG(LOG_MSG, "memory freed.");
			return;
		}
	} else {
		/* new is the first of a non-empty list. */
		next = heap.holes;
		heap.holes = new;
	}

	/* There's a hole after the new block, is it side by side? */
	if ((struct chunk *)next <= ((struct chunk *)new + new->size)) {
#ifdef DEBUG
		if ((struct chunk *)next < ((struct chunk *)new + new->size)) {
			/* TODO: if this happens, something is really bad, and a new size
			   must be calculated. */
			DCC_LOG4(LOG_ERROR, "Hole overlap: 0x%p(%u), 0x%p(%u)", 
					 new, new->size, next, next->size);
		}
#endif
		new->size += next->size;
		new->next = next->next;
		DCC_LOG(LOG_MSG, "catenated new with next, memory freed.");
	} else {
		new->next = next;
		DCC_LOG(LOG_MSG, "new hole added to the list. memory freed.");
	}
	
#ifdef DEBUG
#if	DEBUG_LEVEL >= DBG_MSG
	hole_dump();
#endif
#endif

	return;
}

