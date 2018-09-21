#include <stdint.h>
#include <string.h>
#include "util_queuehdr.h"

util_queuehdr_status_t util_queuehdr_init(util_queuehdr_t *q, util_queuehdr_data_t *area, uint32_t queue_size, uint32_t element_size)
{
    uint32_t n;
    uint8_t *pa = (uint8_t *) area;

	if(queue_size == 0 || element_size == 0)
		return UTIL_QUEUEHDR_ERROR;
		
	q->hdr.cons = 0;
	q->hdr.prod = 0;
	q->hdr.element_size = element_size;
	q->hdr.internal_element_size = UTIL_QUEUEHDR_LEN(element_size);	
	q->hdr.queue_size = queue_size;

	// point to header block
	q->data = (util_queuehdr_element_t *) pa;
	
	// point to data block
	pa += sizeof(util_queuehdr_element_t)*queue_size;

	// initialize header block
    for(n = 0 ; n < queue_size ; n++)
    {
		(q->data + n)->buffer = pa;
		(q->data + n)->pos = 0;
        pa += element_size;
    }

    return UTIL_QUEUEHDR_OK;
}

// start enqueue transation, element pointer is returned for data storing
util_queuehdr_status_t util_queuehdr_put_beg(util_queuehdr_t *q, util_queuehdr_element_t **element)
{
	uint8_t next_prod = q->hdr.prod;
	
	next_prod = UTIL_QUEUEHDR_INC(q->hdr.prod, q->hdr.queue_size);
	
	if(next_prod != q->hdr.cons)
	{
		*element = (util_queuehdr_element_t *) (q->data + q->hdr.prod);
		return UTIL_QUEUEHDR_OK;
	}

    *element = NULL;

	return UTIL_QUEUEHDR_FULL;
}

// finish enqueue transaction
util_queuehdr_status_t util_queuehdr_put_end(util_queuehdr_t *q)
{
	q->hdr.prod = UTIL_QUEUEHDR_INC(q->hdr.prod, q->hdr.queue_size);

	return UTIL_QUEUEHDR_OK;
}

// start dequeue transation, element pointer is returned for data retrieving
util_queuehdr_status_t util_queuehdr_get_beg(util_queuehdr_t *q, util_queuehdr_element_t **element)
{
	if(q->hdr.cons == q->hdr.prod)
	{
		return UTIL_QUEUEHDR_EMPTY;
	}
	else
		*element = (util_queuehdr_element_t *) (q->data + q->hdr.cons);

	return UTIL_QUEUEHDR_OK;
}

// finish dequeue transaction
util_queuehdr_status_t util_queuehdr_get_end(util_queuehdr_t *q)
{
	q->hdr.cons = UTIL_QUEUEHDR_INC(q->hdr.cons, q->hdr.queue_size);
	
	return UTIL_QUEUEHDR_OK;
}

util_queuehdr_status_t util_queuehdr_is_empty(util_queuehdr_t *q)
{
	return (q->hdr.cons == q->hdr.prod) ? UTIL_QUEUEHDR_EMPTY : UTIL_QUEUEHDR_OK; 
}


