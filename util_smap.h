#ifndef __UTIL_SMAP__
#define __UTIL_SMAP__

#ifdef __cplusplus
extern "C" {
#endif

#define UTIL_SMAP_KEY_SIZE 7
#define UTIL_SMAP_MAX_ENTRIES 256

typedef enum util_smap_data_types_e
{
	UTIL_SMAP_UINT8,
	UTIL_SMAP_UINT16,
	UTIL_SMAP_UINT32,
	UTIL_SMAP_UINT64,
	UTIL_SMAP_INT8,
	UTIL_SMAP_INT16,
	UTIL_SMAP_INT32,
	UTIL_SMAP_INT64,
	UTIL_SMAP_FLOAT,
	UTIL_SMAP_DOUBLE,
	UTIL_SMAP_PVOID,
	UTIL_SMAP_PSTR8,
} util_smap_data_types_t;

typedef union util_smap_data_u
{
	uint8_t  u8;
	uint16_t u16;
	uint32_t u32;
	uint64_t u64;
	int8_t   i8;
	int16_t  i16;
	int32_t  i32;
	int64_t  i64;
	float    f;
	double   d;
	void    *pv;
	uint8_t  ps8[8];
} util_smap_data_t;

typedef struct util_smap_entry_s
{
	util_smap_data_t value;
	uint8_t key[UTIL_SMAP_KEY_SIZE];
	util_smap_data_types_t type;
} util_smap_entry_t;

typedef enum util_smap_status_s
{
	UTIL_SMAP_STATUS_ERROR = 0,
	UTIL_SMAP_STATUS_OK = 1
} util_smap_status_t;

typedef struct util_smap_s
{
    uint8_t entry_size;
    uint8_t map_size;
    uint8_t current_size;
    util_smap_entry_t *data;
    uint8_t *ctrl;
} util_smap_t;

#define UTIL_SMAP_CALC_SIZE(n)   ( ( (n)*sizeof(util_smap_entry_t) + (n)*sizeof(uint8_t) + sizeof(uint32_t) ) / sizeof(uint32_t) )

util_smap_status_t util_smap_init(util_smap_t *m, uint32_t* area, uint32_t map_size);
util_smap_status_t util_smap_set(util_smap_t *m, const uint8_t *key, util_smap_data_t value);
util_smap_status_t util_smap_get(util_smap_t *m, const uint8_t *key, util_smap_data_t *value);
util_smap_status_t util_smap_del(util_smap_t *m, const uint8_t *key);
uint8_t util_smap_size(util_smap_t *m);
uint8_t util_smap_current_size(util_smap_t *m);

#ifdef __cplusplus
}
#endif

#endif /*  __UTIL_QUEUE__ */
