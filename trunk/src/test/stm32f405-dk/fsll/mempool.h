/* 
 * @file	mempool.h
 * @brief	Memory block pool
 * @author	Robinson Mittmann (bobmittmann@gmail.com)
 *
 */


#ifndef __MEMPOOL_H__ 
#define __MEMPOOL_H__ 

#include <stdbool.h>

struct mempool;

#ifdef __cplusplus
extern "C" {
#endif

struct mempool * mempool_alloc(size_t nmemb, size_t size);

void * memblk_alloc(struct mempool * pool);

bool memblk_free(struct mempool * pool, void * ptr);

void memblk_incref(void * ptr);

void memblk_decref(void * ptr);

#ifdef __cplusplus
}
#endif	

#endif /* __MEMPOOL_H__ */

