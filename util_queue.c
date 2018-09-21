#include <stdint.h>
#include <string.h>
#include "util_queue.h"

/*

util_queue_data_t area[UTIL_QUEUE_CALC_SIZE(10,35)];
util_queue_t q;

util_queue_init(&q,area,10,35);

util_queuecp_data_t *element = NULL;

if(util_queue_put_beg(&q,&element) == UTIL_QUEUE_FULL)
{

}
else
{
	util_queue_get_end(&q);
}

if(util_queue_get_beg(&q,&element) == UTIL_QUEUE_EMPTY)
{

}
else
{
	util_queue_get_end(&q);
}

*/

util_queue_status_t util_queue_init(util_queue_t *q, util_queue_data_t *area, uint32_t queue_size, uint32_t element_size)
{
	if(queue_size == 0 || element_size == 0)
		return UTIL_QUEUE_ERROR;

	q->cons = 0;
	q->prod = 0;
	q->element_size = element_size;
	q->internal_element_size = UTIL_QUEUE_LEN(element_size);
	q->queue_size = queue_size;
    q->data = area;

    return UTIL_QUEUE_OK;
}

// start enqueue transation, element pointer is returned for data storing
util_queue_status_t util_queue_put_beg(util_queue_t *q, util_queue_data_t **element)
{
	uint8_t next_prod = q->prod;
	
	next_prod = UTIL_QUEUE_INC(q->prod, q->queue_size);
	
	if(next_prod != q->cons)
	{
		*element = (util_queue_data_t *) (q->data + q->prod*q->internal_element_size);
		return UTIL_QUEUE_OK;
	}

    *element = NULL;

	return UTIL_QUEUE_FULL;
}

// finish enqueue transaction
util_queue_status_t util_queue_put_end(util_queue_t *q)
{
	q->prod = UTIL_QUEUE_INC(q->prod, q->queue_size);

	return UTIL_QUEUE_OK;
}

// start dequeue transation, element pointer is returned for data retrieving
util_queue_status_t util_queue_get_beg(util_queue_t *q, util_queue_data_t **element)
{		
	if(q->cons == q->prod)
		return UTIL_QUEUE_EMPTY;

	*element = (util_queue_data_t *) (q->data + q->cons*q->internal_element_size);

	return UTIL_QUEUE_OK;
}

// finish dequeue transaction
util_queue_status_t util_queue_get_end(util_queue_t *q)
{
	q->cons = UTIL_QUEUE_INC(q->cons, q->queue_size);
	
	return UTIL_QUEUE_OK;
}

util_queue_status_t util_queue_is_empty(util_queue_t *q)
{
	return (q->cons == q->prod) ? UTIL_QUEUE_EMPTY : UTIL_QUEUE_OK; 
}


