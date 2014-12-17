/* 
 * @file	fifo.h
 * @brief	Priority queue
 * @author	Robinson Mittmann (bobmittmann@gmail.com)
 *
 */


#ifndef __FIFO_H__ 
#define __FIFO_H__ 

#include <stdbool.h>

struct fifo;

#ifdef __cplusplus
extern "C" {
#endif

/* Insert an element into the queue */
bool fifo_enqueue(struct fifo * fifo, const void * val);

/* Remove an element from the queue */
bool fifo_dequeue(struct fifo * fifo, void * val);

/* Return the number of elements into the queue */
unsigned int fifo_size(struct fifo * fifo);

/* Allocate a new queue of specified maximum length */
struct fifo * fifo_queue_alloc(unsigned int length, unsigned int val_size);

/* Check whether there is some space left or not */
bool fifo_isfull(struct fifo * fifo);

#ifdef __cplusplus
}
#endif	

#endif /* __FIFO_H__ */

