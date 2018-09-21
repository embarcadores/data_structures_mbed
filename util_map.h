#ifndef __UTIL_QUEUE__
#define __UTIL_MAP__ 

#ifdef __cplusplus
extern "C" {
#endif

#define UTIL_MAP_KEY_SIZE    7
#define UTIL_MAP_MAX_ENTRIES 256

typedef enum util_map_data_types_e
{
	UTIL_MAP_INVALID_TYPE = 0,
	UTIL_MAP_BOOL,
	UTIL_MAP_UINT8,
	UTIL_MAP_UINT16,
	UTIL_MAP_UINT32,
	UTIL_MAP_UINT64,
	UTIL_MAP_INT8,
	UTIL_MAP_INT16,
	UTIL_MAP_INT32,
	UTIL_MAP_INT64,
	UTIL_MAP_FLOAT,
	UTIL_MAP_DOUBLE,
	UTIL_MAP_PVOID,
	UTIL_MAP_PSTR8,
} util_map_data_types_t;

typedef enum util_map_status_s
{
	UTIL_MAP_STATUS_OK = 0,
	UTIL_MAP_STATUS_MAP_FULL_ERROR = -1,
	UTIL_MAP_STATUS_MAX_MAP_SIZE_ERROR = -2,
	UTIL_MAP_STATUS_KEY_SIZE_ERROR = -3,
	UTIL_MAP_STATUS_ITEM_NOT_FOUND_ERROR = -4,
	UTIL_MAP_STATUS_MAP_EMPTY_ERROR = -5,
} util_map_status_t;

typedef struct util_map_data_s
{
	union util_map_data_types_u
	{
		bool     b;
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
	} value;
	util_map_data_types_t type;
} util_map_data_t;

struct util_map_entry_s
{
	util_map_data_t data;
	struct util_map_entry_s *next;
	struct util_map_entry_s *prev;
	uint8_t key[UTIL_MAP_KEY_SIZE + 1];
};

typedef struct util_map_entry_s util_map_entry_t;

// for larger maps, use pool, pool_head, map_size as uint16_t
typedef struct util_map_s
{
    uint8_t entry_size;
    uint8_t map_size;
    util_map_entry_t *data;
    util_map_entry_t **table;
    uint8_t *pool;
    uint8_t pool_head;
} util_map_t;

#define UTIL_MAP_CALC_SIZE(n)   ( ( (n)*(sizeof(util_map_entry_t) + \
                                         sizeof(uint8_t) + \
                                         sizeof(struct util_map_entry_s *) ) + \
                                         sizeof(uint32_t) ) / sizeof(uint32_t) )

util_map_status_t util_map_init(util_map_t *m, uint32_t* area, uint32_t map_size);
util_map_status_t util_map_set(util_map_t *m, const uint8_t *key, util_map_data_t value);
util_map_status_t util_map_getv(util_map_t *m, const uint8_t *key, util_map_data_t *value);
util_map_data_t * util_map_getr(util_map_t *m, const uint8_t *key);
util_map_status_t util_map_del(util_map_t *m, const uint8_t *key);
bool util_map_has_key(util_map_t *m, const uint8_t *key);
uint8_t util_map_get_size(util_map_t *m);
uint8_t util_map_get_count(util_map_t *m);
void util_map_print(util_map_t *m);
util_map_entry_t *util_map_iter_items(util_map_t *m, bool start);

#ifdef __cplusplus
}
#endif

#endif /*  __UTIL_QUEUE__ */
