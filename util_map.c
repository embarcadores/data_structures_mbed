#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "util_map.h"

// https://stackoverflow.com/questions/7666509/hash-function-for-string/7666577#7666577
// http://www.cse.yorku.ca/~oz/hash.html
static uint32_t util_map_hash(const uint8_t *str)
{
    uint32_t hash = 5381;
    uint8_t *pstr = (uint8_t *) str;
    uint8_t c;

    for( c = *pstr ; c ; c = *(++pstr) )
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

util_map_status_t util_map_init(util_map_t *m, uint32_t* area, uint32_t map_size)
{
	uint8_t n;

	if(area == 0 || map_size > UTIL_MAP_MAX_ENTRIES)
		return UTIL_MAP_STATUS_MAX_MAP_SIZE_ERROR;

	m->data = (util_map_entry_t *) area;
	m->entry_size = sizeof(util_map_entry_t);
	m->map_size = map_size;
	m->pool = (uint8_t *)(&m->data[m->map_size]);
	m->table = (util_map_entry_t **)(&m->pool[m->map_size]);
    m->pool_head = 0;

	for(n = 0 ; n < m->map_size ; n++)
	{
		m->pool[n] = n;
		m->table[n] = NULL;
	}

	return UTIL_MAP_STATUS_OK;
}

static util_map_entry_t * util_map_find_position(util_map_t *m, const uint8_t *key, uint8_t *hash_pos)
{
	util_map_entry_t *next = NULL;
	*hash_pos = util_map_hash(key) % m->map_size;

	for(next = m->table[*hash_pos] ; next != NULL ; next = next->next)
	{
		if(strncmp((char *)key,(char *)next->key,UTIL_MAP_KEY_SIZE) == 0)
			break;
	}

	return next;
}

util_map_status_t util_map_set(util_map_t *m, const uint8_t *key, util_map_data_t data)
{
	uint8_t hash_pos;
	util_map_entry_t *pos;
	uint8_t entry_pos;

	if(strlen((char *)key) >= UTIL_MAP_KEY_SIZE)
		return UTIL_MAP_STATUS_KEY_SIZE_ERROR;

	// is the key already in the map ?
	pos = util_map_find_position(m,key,&hash_pos);

	// no, new item
	if(pos == NULL)
	{
		// is there available space ?
		if(m->pool_head >= m->map_size)
			return UTIL_MAP_STATUS_MAP_FULL_ERROR;

		// allocate the new item from our pool
		entry_pos = m->pool[m->pool_head];
		m->pool_head++;
		pos = &(m->data[entry_pos]);

		// any item in the table position ?
		if(m->table[hash_pos] == NULL)
		{
			// no, let's add the new item to the table and finalize the chain
			pos->next = NULL;
		}
		else
		{
			// yes, let's put the item at beginning, chaining other items
			pos->next = m->table[hash_pos];
			m->table[hash_pos]->prev = pos;
		}

		// always set as the first item, prev is null
		pos->prev = NULL;
		// set table position and copy the key
		m->table[hash_pos] = pos;
		strncpy((char *)pos->key,(char *)key,UTIL_MAP_KEY_SIZE);
	}

	// update/copy values
	pos->data = data;

	return UTIL_MAP_STATUS_OK;
}

util_map_status_t util_map_getv(util_map_t *m, const uint8_t *key, util_map_data_t *value)
{
	uint8_t hash_pos;
	util_map_entry_t *pos;

	if(strlen((char *)key) >= UTIL_MAP_KEY_SIZE)
		return UTIL_MAP_STATUS_KEY_SIZE_ERROR;

	pos = util_map_find_position(m,key,&hash_pos);

	if(pos == NULL)
		return UTIL_MAP_STATUS_ITEM_NOT_FOUND_ERROR;

	*value = pos->data;

	return UTIL_MAP_STATUS_OK;
}

util_map_data_t * util_map_getr(util_map_t *m, const uint8_t *key)
{

	util_map_data_t *value;
	uint8_t hash_pos;
	util_map_entry_t *pos;

	if(strlen((char *)key) >= UTIL_MAP_KEY_SIZE)
		return NULL;

	pos = util_map_find_position(m,key,&hash_pos);

	if(pos == NULL)
		value = NULL;
	else
		value = &pos->data;

	return value;
}


bool util_map_has_key(util_map_t *m, const uint8_t *key)
{
	uint8_t hash_pos;

	if(strlen((char *)key) >= UTIL_MAP_KEY_SIZE)
		return UTIL_MAP_STATUS_KEY_SIZE_ERROR;

	return util_map_find_position(m,key,&hash_pos) == NULL ? false : true;
}


util_map_status_t util_map_del(util_map_t *m, const uint8_t *key)
{
	uint8_t entry_pos;
	uint8_t hash_pos;
	util_map_entry_t *pos;

	if(strlen((char *)key) >= UTIL_MAP_KEY_SIZE)
		return UTIL_MAP_STATUS_KEY_SIZE_ERROR;

	if(m->pool_head == 0)
		return UTIL_MAP_STATUS_MAP_EMPTY_ERROR;

	pos = util_map_find_position(m,key,&hash_pos);

	if(pos == NULL)
		return UTIL_MAP_STATUS_ITEM_NOT_FOUND_ERROR;

	// unique item ?
	if(pos->prev == NULL && pos->next == NULL)
	{
		m->table[hash_pos] = NULL;
	}
	// first item ?
	else if(pos->prev == NULL && pos->next != NULL)
	{
		m->table[hash_pos] = pos->next;
		m->table[hash_pos]->prev = NULL;
	}
	// last item ?
	else if(pos->prev != NULL && pos->next == NULL)
	{
		pos->prev->next = NULL;
	}
	// in the middle ?
	else if(pos->prev != NULL && pos->next != NULL)
	{
		pos->prev->next = pos->next;
		pos->next->prev = pos->prev;
	}

	// cleanup
	pos->data.value.i64 = 0;
	pos->data.type = UTIL_MAP_INVALID_TYPE;
	pos->next = pos->prev = 0;

	// pool position
	entry_pos = pos - m->data;

	// give the item back to the pool
	m->pool_head--;
	m->pool[m->pool_head] = entry_pos;

	return UTIL_MAP_STATUS_OK;
}

util_map_entry_t *util_map_iter_items(util_map_t *m, bool start)
{
	static uint8_t n = 0;
	static util_map_entry_t *pos;

	if(start)
	{
		n = 0;
	}
	else
	{
		pos = pos->next;
		if(pos)
			return pos;
		n++;
	}

	for( ; n < m->map_size ; n++ )
	{
		for(pos = m->table[n] ; pos ; )
		{
			return pos;
		}
	}

	return NULL;
}

#define FRAC(x) ((x-(int)(x))*100)
#define CASE_ENTRY(f,x,y,c) case x: printf("" #x #f,(c) pos->data.value.y); break
#define CASE_ENTRYF(f,x,y,c) case x: printf("" #x #f,(c) pos->data.value.y, (c) FRAC(pos->data.value.y)); break

void util_map_print(util_map_t *m)
{
	uint8_t n;
	util_map_entry_t *pos;

	printf("\r\n");
	for(n = 0 ; n < m->map_size ; n++)
	{
		for(pos = m->table[n]; pos ; pos = pos->next)
		{
			printf("%s ==> ",pos->key);

			switch(pos->data.type)
			{
				CASE_ENTRY(": %u, ",UTIL_MAP_BOOL,b,unsigned int);
				CASE_ENTRY(": %u,  ",UTIL_MAP_UINT8,u8,unsigned int);
				CASE_ENTRY(": %u,  ",UTIL_MAP_UINT16,u16,unsigned int);
				CASE_ENTRY(": %u,  ",UTIL_MAP_UINT32,u32,unsigned int);
				CASE_ENTRY(": %u,  ",UTIL_MAP_UINT64,u64,unsigned int);
				CASE_ENTRY(": %u,  ",UTIL_MAP_INT8,i8,signed int);
				CASE_ENTRY(": %u,  ",UTIL_MAP_INT16,i16,signed int);
				CASE_ENTRY(": %u,  ",UTIL_MAP_INT32,i32,signed int);
				CASE_ENTRY(": %u,  ",UTIL_MAP_INT64,i64,signed int);
				CASE_ENTRYF(": %d.%d,  ",UTIL_MAP_FLOAT,f,signed int);
				CASE_ENTRYF(": %d.%d,  ",UTIL_MAP_DOUBLE,f,signed int);
				CASE_ENTRY(": %08X,  ",UTIL_MAP_PVOID,pv,unsigned int);
				CASE_ENTRY(": %s,  ",UTIL_MAP_PSTR8,ps8,char *);
			default:
				break;
			}
		}
	}
}

inline uint8_t util_map_get_size(util_map_t *m)
{
	return m->map_size;
}

inline uint8_t util_map_get_count(util_map_t *m)
{
	return m->pool_head;
}


#if 0
uint32_t dev_map_area[UTIL_MAP_CALC_SIZE(8)];
util_map_t dev_map;

util_map_data_t x[8];

util_map_init(&dev_map,dev_map_area,8);

x[0].value.i32 =-1;
x[1].value.i32 =-2;
x[2].value.i32 =-3;
x[3].value.i32 =-4;
x[4].value.i32 =-5;
x[5].value.ps8[0] = 'M';
x[5].value.ps8[1] = 'B';
x[5].value.ps8[2] = 'A';
x[5].value.ps8[3] = '\0';
x[6].value.i32 =-7;
x[7].value.i32 =-8;

util_map_set(&dev_map,(uint8_t*)"X0",x[0]);
util_map_set(&dev_map,(uint8_t*)"X1",x[1]);
util_map_set(&dev_map,(uint8_t*)"X2",x[2]);
util_map_set(&dev_map,(uint8_t*)"X3",x[3]);
util_map_set(&dev_map,(uint8_t*)"X4",x[4]);
util_map_set(&dev_map,(uint8_t*)"X5",x[5]);
util_map_set(&dev_map,(uint8_t*)"X6",x[6]);
util_map_set(&dev_map,(uint8_t*)"X7",x[7]);
util_map_set(&dev_map,(uint8_t*)"X8",x[7]);

util_map_del(&dev_map,(uint8_t*)"X3");
util_map_del(&dev_map,(uint8_t*)"X4");

x[5].value.i64 = 0;
util_map_get(&dev_map,(uint8_t*)"X5",&x[5]);
util_map_get(&dev_map,(uint8_t*)"X8",&x[5]);

x[3].value.d = 3.141516;
util_map_set(&dev_map,(uint8_t*)"X5",x[3]);
util_map_set(&dev_map,(uint8_t*)"X4",x[1]);

x[3].value.i64 = 0;
util_map_get(&dev_map,(uint8_t*)"X3",&x[3]);

util_map_del(&dev_map,(uint8_t*)"X0");
util_map_del(&dev_map,(uint8_t*)"X1");
util_map_del(&dev_map,(uint8_t*)"X2");
util_map_del(&dev_map,(uint8_t*)"X3");
util_map_del(&dev_map,(uint8_t*)"X4");
util_map_del(&dev_map,(uint8_t*)"X5");
util_map_del(&dev_map,(uint8_t*)"X6");
util_map_del(&dev_map,(uint8_t*)"X7");

util_map_set(&dev_map,(uint8_t*)"FFD",x[0]);
util_map_set(&dev_map,(uint8_t*)"X0",x[1]);
util_map_set(&dev_map,(uint8_t*)"000",x[2]);
util_map_set(&dev_map,(uint8_t*)"664",x[3]);
util_map_set(&dev_map,(uint8_t*)"X8",x[4]);
util_map_set(&dev_map,(uint8_t*)"333",x[5]);
util_map_set(&dev_map,(uint8_t*)"444",x[6]);
util_map_set(&dev_map,(uint8_t*)"555",x[7]);
util_map_set(&dev_map,(uint8_t*)"666",x[7]);

util_map_del(&dev_map,(uint8_t*)"000");
util_map_del(&dev_map,(uint8_t*)"FFD");
util_map_del(&dev_map,(uint8_t*)"X8");
util_map_del(&dev_map,(uint8_t*)"664");

util_map_set(&dev_map,(uint8_t*)"000",x[2]);
util_map_set(&dev_map,(uint8_t*)"M1",x[7]);
util_map_set(&dev_map,(uint8_t*)"M2",x[7]);
util_map_set(&dev_map,(uint8_t*)"M9",x[7]);
#endif
