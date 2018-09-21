#ifndef __UTIL_QUEUEHDR__
#define __UTIL_QUEUEHDR__ 

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t util_queuehdr_data_t;

typedef enum util_queuehdr_status_s
{
    UTIL_QUEUEHDR_OK = 0,
    UTIL_QUEUEHDR_ERROR,
    UTIL_QUEUEHDR_FULL,
    UTIL_QUEUEHDR_EMPTY,
} util_queuehdr_status_t;

typedef struct util_queuehdr_element_s
{
    volatile uint32_t pos;
    uint8_t *buffer;
} util_queuehdr_element_t;

typedef struct util_queuehdr_header_s
{
    volatile uint32_t prod;
    volatile uint32_t cons;
    uint32_t element_size;
    uint32_t internal_element_size;    
    uint32_t queue_size;
} util_queuehdr_header_t;

typedef struct util_queuehdr_s
{
    util_queuehdr_header_t hdr;
    util_queuehdr_element_t *data;
} util_queuehdr_t;

#define UTIL_QUEUEHDR_INC(v,mv)   ((((v) + 1) >= (mv)) ? 0 : (v) + 1)
#define UTIL_QUEUEHDR_LEN(len) ((len) % sizeof(util_queuehdr_data_t) == 0 ? (len) / sizeof(util_queuehdr_data_t) : ( (len) + sizeof(util_queuehdr_data_t) ) / sizeof(util_queuehdr_data_t))
#define UTIL_QUEUEHDR_CALC_SIZE(n,len)   ( (n)*UTIL_QUEUEHDR_LEN(len + sizeof(util_queuehdr_element_t)) ) 

util_queuehdr_status_t util_queuehdr_init(util_queuehdr_t *q, util_queuehdr_data_t* area, uint32_t queue_size, uint32_t element_size);
util_queuehdr_status_t util_queuehdr_put_beg(util_queuehdr_t *q, util_queuehdr_element_t **element);
util_queuehdr_status_t util_queuehdr_put_end(util_queuehdr_t *q);
util_queuehdr_status_t util_queuehdr_get_beg(util_queuehdr_t *q, util_queuehdr_element_t **element);
util_queuehdr_status_t util_queuehdr_get_end(util_queuehdr_t *q);
util_queuehdr_status_t util_queuehdr_is_empty(util_queuehdr_t *q);

#ifdef __cplusplus
}
#endif

#endif /*  __UTIL_QUEUEHDR__ */
