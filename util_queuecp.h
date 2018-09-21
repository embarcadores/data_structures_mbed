#ifndef __UTIL_QUEUECP__
#define __UTIL_QUEUECP__ 

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t util_queuecp_data_t;

typedef enum util_queuecp_status_s
{
    UTIL_QUEUECP_OK = 0,
    UTIL_QUEUECP_ERROR,
    UTIL_QUEUECP_FULL,
    UTIL_QUEUECP_EMPTY,
} util_queuecp_status_t;

typedef struct util_queuecp_s
{
    volatile uint32_t prod;
    volatile uint32_t cons;
    uint32_t element_size;
    uint32_t internal_element_size;    
    uint32_t queue_size;
    util_queuecp_data_t *data;
} util_queuecp_t;


#define UTIL_QUEUECP_LEN(len) ((len) % sizeof(util_queuecp_data_t) == 0 ? (len) / sizeof(util_queuecp_data_t) : ( (len) + sizeof(util_queuecp_data_t) ) / sizeof(util_queuecp_data_t))
#define UTIL_QUEUECP_INC(v,mv)   ((((v) + 1) >= (mv)) ? 0 : (v) + 1)
#define UTIL_QUEUECP_CALC_SIZE(n,len)   ( (n)*UTIL_QUEUECP_LEN(len) ) 

util_queuecp_status_t util_queuecp_init(util_queuecp_t *q, util_queuecp_data_t *area, uint32_t queue_size, uint32_t element_size);
util_queuecp_status_t util_queuecp_put(util_queuecp_t *q, void *data);
util_queuecp_status_t util_queuecp_get(util_queuecp_t *q, void *data);
util_queuecp_status_t util_queuecp_is_empty(util_queuecp_t *q);

#ifdef __cplusplus
}
#endif

#endif /*  __UTIL_QUEUECP__ */
