#ifndef __UTIL_QUEUE__
#define __UTIL_QUEUE__ 

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t util_queue_data_t;

typedef enum util_queue_status_s
{
    UTIL_QUEUE_OK = 0,
    UTIL_QUEUE_ERROR,
    UTIL_QUEUE_FULL,
    UTIL_QUEUE_EMPTY,
} util_queue_status_t;

typedef struct util_queue_header_s
{
    volatile uint32_t prod;
    volatile uint32_t cons;
    uint32_t element_size;
    uint32_t internal_element_size;    
    uint32_t queue_size;
    util_queue_data_t *data;
} util_queue_t;

#define UTIL_QUEUE_INC(v,mv)   ((((v) + 1) >= (mv)) ? 0 : (v) + 1)
#define UTIL_QUEUE_LEN(len) ((len) % sizeof(util_queue_data_t) == 0 ? (len) / sizeof(util_queue_data_t) : ( (len) + sizeof(util_queue_data_t) ) / sizeof(util_queue_data_t))
#define UTIL_QUEUE_CALC_SIZE(n,len)   ( (n)*UTIL_QUEUE_LEN(len) ) 

util_queue_status_t util_queue_init(util_queue_t *q, util_queue_data_t* area, uint32_t queue_size, uint32_t element_size);
util_queue_status_t util_queue_put_beg(util_queue_t *q, util_queue_data_t **element);
util_queue_status_t util_queue_put_end(util_queue_t *q);
util_queue_status_t util_queue_get_beg(util_queue_t *q, util_queue_data_t **element);
util_queue_status_t util_queue_get_end(util_queue_t *q);
util_queue_status_t util_queue_is_empty(util_queue_t *q);

#ifdef __cplusplus
}
#endif

#endif /*  __UTIL_QUEUE__ */
