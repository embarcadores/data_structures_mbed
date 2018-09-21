#include <stdint.h>
#include <string.h>
#include "util_queuecp.h"

/*

util_queuecp_data_t area[UTIL_QUEUECP_CALC_SIZE(10,35)];
util_queuecp_t q;

util_queuecp_init(&q,area,10,35);

util_queuecp_data_t data[36];

if(util_queuecp_put(&q,data) == UTIL_QUEUECP_FULL)
{
	
}

if(util_queuecp_get(&q,data) == UTIL_QUEUECP_EMPTY)
{

}

*/
util_queuecp_status_t util_queuecp_init(util_queuecp_t *q, util_queuecp_data_t *area, uint32_t queue_size, uint32_t element_size)
{
	if(queue_size == 0 || element_size == 0)
		return UTIL_QUEUECP_ERROR;
		
	q->cons = 0;
	q->prod = 0;
	q->element_size = element_size;
	q->internal_element_size = UTIL_QUEUECP_LEN(element_size);
	q->queue_size = queue_size;
    q->data = area;
	
    return UTIL_QUEUECP_OK;
}

util_queuecp_status_t util_queuecp_put(util_queuecp_t *q, void *data)
{
	util_queuecp_data_t *pos;
	uint8_t next_prod = q->prod;
	
	next_prod = UTIL_QUEUECP_INC(q->prod, q->queue_size);
	
	if(next_prod != q->cons)
	{
		// calc offset in "util_queuecp_data_t*" units
		pos = q->data + q->prod*q->internal_element_size;
		memcpy((void *)pos, data, q->element_size);
		q->prod = next_prod;
		
		return UTIL_QUEUECP_OK;
	}

	return UTIL_QUEUECP_FULL;
}

util_queuecp_status_t util_queuecp_get(util_queuecp_t *q, void *data)
{
	util_queuecp_data_t *pos;

	if(q->cons == q->prod)
		return UTIL_QUEUECP_EMPTY;

	// calc offset in "util_queuecp_data_t*" units
	pos = q->data + q->cons*q->internal_element_size;
	memcpy(data, (void *)pos, q->element_size);
	q->cons = UTIL_QUEUECP_INC(q->cons, q->queue_size);
		
	return UTIL_QUEUECP_OK;
}

util_queuecp_status_t util_queuecp_is_empty(util_queuecp_t *q)
{
	return (q->cons == q->prod) ? UTIL_QUEUECP_EMPTY : UTIL_QUEUECP_OK; 
}


