/*
 * @file	fifo.c
 * @brief	FIFO queue
 * @author	Robinson Mittmann (bobmittmann@gmail.com)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct fifo {
	unsigned int length;
	unsigned int val_size;
	unsigned int val_blks;
	volatile unsigned int tail;
	volatile unsigned int head;
	long long buf[];
};

/* Insert an event exception code into the queue */
bool fifo_enqueue(struct fifo * fifo, void * val)
{
	unsigned int head = fifo->head;
	unsigned int pos;

	if ((unsigned int)(head - fifo->tail) == fifo->length) {
		/* FIFO full */
		return false;
	}

	pos = (head++ % fifo->length) * fifo->val_blks;
	if (val != NULL) 
		memcpy(&fifo->buf[pos], val, fifo->val_size);
	fifo->head = head;
	return true;
}

bool fifo_isfull(struct fifo * fifo)
{
	unsigned int head = fifo->head;

	return ((unsigned int)(head - fifo->tail) == fifo->length) ? true : false;
}

/* Remove an event exception code from the queue */
bool fifo_dequeue(struct fifo * fifo, void * val)
{
	unsigned int tail = fifo->tail;
	unsigned int pos;

	if (fifo->head == tail) {
		/* FIFO empty */
		return false;
	}

	pos = (tail++ % fifo->length) * fifo->val_blks;
	if (val != NULL) 
		memcpy(val, &fifo->buf[pos], fifo->val_size);
	fifo->tail = tail;
	return true;
}

/* Return the number of elements into the queue */
unsigned int fifo_size(struct fifo * fifo)
{
	return (unsigned int)(fifo->head - fifo->tail);
}

/* Allocate a new queue of specified maximum length */
struct fifo * fifo_queue_alloc(unsigned int length, unsigned int val_size)
{
	struct fifo * fifo;
	unsigned int val_blks;
	unsigned int size;

	/* number of 64bits blocks need to store the value */
	val_blks = (val_size + (sizeof(long long) - 1)) / sizeof(long long);
	size = val_blks * sizeof(long long);

	fifo = (struct fifo *)malloc(sizeof(struct fifo) + length * size);

	fifo->length = length;
	fifo->val_size = val_size;
	fifo->val_blks = val_blks;
	fifo->head = 0;
	fifo->tail = 0;

	return fifo;
}


