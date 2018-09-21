#include <stdint.h>
#include <string.h>
#include "util_mpool.h"

/*
util_mpool_data_t area[UTIL_MPOOL_CALC_SIZE(10,32)];

util_mpool_t p;
util_mpool_data_t *data;

util_mpool_init(&p,area,10,32);

if(util_mpool_alloc(&p,&data) == UTIL_MPOOL_EMPTY)
{
	// tratar problema
}

util_mpool_free(&p,&data)

*/

util_mpool_status_t util_mpool_init(util_mpool_t *p, util_mpool_data_t *area, uint32_t pool_size, uint32_t element_size)
{
	uint32_t n;
	uint8_t *pb;
	
	if(pool_size == 0 || element_size == 0 || pool_size > 255)
		return UTIL_MPOOL_ERROR;
		
	p->element_size = element_size;
	p->pool_size = pool_size;
	p->internal_element_size = UTIL_MPOOL_LEN(element_size);
	// point to data block
	p->data = area;

	// point to ctrl block
	p->ctrl = (uint8_t *) (p->data + p->pool_size*p->internal_element_size);
	
	// initialize ctrl block
	pb = p->ctrl;
    for(n = 0 ; n < p->pool_size ; n++, pb++)
		*pb = n; // p->ctrl[n] = n;

	p->ctrl_head = 0;
	
    return UTIL_MPOOL_OK;
}

util_mpool_status_t util_mpool_alloc(util_mpool_t *p, util_mpool_data_t **data)
{
	uint32_t pos;

	// search for a free block
	if(p->ctrl_head >= p->pool_size)
		return UTIL_MPOOL_EMPTY;
	
	pos = *(p->ctrl + p->ctrl_head); // p->ctrl[p->ctrl_head] 
	*data = (p->data + pos*p->internal_element_size);
	p->ctrl_head++;
	
	return UTIL_MPOOL_OK;
}

util_mpool_status_t util_mpool_free(util_mpool_t *p, util_mpool_data_t **data)
{
	uint32_t pos;
	
	if(p->ctrl_head == 0)
		return UTIL_MPOOL_ERROR;
		
	pos = (*data - p->data)/p->internal_element_size;
	p->ctrl_head--;
	
	*(p->ctrl + p->ctrl_head) = pos; // // p->ctrl[p->ctrl_head] = pos
	
	return UTIL_MPOOL_OK;
}
