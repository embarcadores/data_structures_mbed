#ifndef __UTIL_MPOOL__
#define __UTIL_MPOOL__ 

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t util_mpool_data_t;

typedef enum util_mpool_status_s
{
    UTIL_MPOOL_OK = 0,
    UTIL_MPOOL_ERROR,
    UTIL_MPOOL_EMPTY,
} util_mpool_status_t;

typedef struct util_mpool_s
{
    uint32_t element_size;
    uint32_t internal_element_size;
    uint32_t pool_size;
    uint32_t ctrl_head;
    uint8_t *ctrl;
    util_mpool_data_t *data;
} util_mpool_t;

#define UTIL_MPOOL_CTRL_LEN(len) ((len) * sizeof(uint8_t)) // MAX 256 !!!!
#define UTIL_MPOOL_LEN(len) ((len) % sizeof(util_mpool_data_t) == 0 ? (len) / sizeof(util_mpool_data_t) : ( (len) + sizeof(util_mpool_data_t) ) / sizeof(util_mpool_data_t))
#define UTIL_MPOOL_CALC_SIZE(n,len)   ( (n)*UTIL_MPOOL_LEN(len) + UTIL_MPOOL_CTRL_LEN(n)) 
#define UTIL_MPOOL_CALC_OFS(n,len)    ( (n)*UTIL_MPOOL_LEN(len) ) 

util_mpool_status_t util_mpool_init(util_mpool_t *p, util_mpool_data_t *area, uint32_t pool_size, uint32_t element_size);
util_mpool_status_t util_mpool_alloc(util_mpool_t *p, util_mpool_data_t **data);
util_mpool_status_t util_mpool_free(util_mpool_t *p, util_mpool_data_t **data);

#ifdef __cplusplus
}
#endif

#endif /*  __UTIL_MPOOL__ */
