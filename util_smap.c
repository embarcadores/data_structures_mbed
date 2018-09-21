#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <util_smap.h>

util_smap_status_t util_smap_init(util_smap_t *m, uint32_t* area, uint32_t map_size)
{
	uint8_t n;

	if(area == 0 || map_size > UTIL_SMAP_MAX_ENTRIES)
		return UTIL_SMAP_STATUS_ERROR;

	m->data = (util_smap_entry_t *) area;
	m->entry_size = sizeof(util_smap_entry_t);
	m->map_size = map_size;
	m->ctrl = (uint8_t *)(&m->data[m->map_size]);

	// 0: free, 1: in use
	for(n = 0 ; n < m->map_size ; n++)
		m->ctrl[n] = 0;

	return UTIL_SMAP_STATUS_OK;
}

static int16_t util_smap_find_index(util_smap_t *m, const uint8_t *key)
{
	uint8_t n;
	int16_t idx = -1;

	for(n = 0 ; n < m->map_size ; n++)
	{
		if(m->ctrl[n])
		{
			if(strncmp((char *)key,(char *)m->data[n].key,UTIL_SMAP_KEY_SIZE) == 0)
			{
				idx = n;
				break;
			}
		}
	}

	return idx;
}

util_smap_status_t util_smap_set(util_smap_t *m, const uint8_t *key, util_smap_data_t value)
{
	uint8_t n;
	int16_t idx = -1;
	util_smap_entry_t *entry;
	bool new_entry = false;

	if(strlen((char *)key) >= UTIL_SMAP_KEY_SIZE)
		return UTIL_SMAP_STATUS_ERROR;

	// is the key already in the map ?
	idx = util_smap_find_index(m,key);

	if(idx == -1)
	{
		// do we have available space ?
		if(m->current_size >= m->map_size)
			return UTIL_SMAP_STATUS_ERROR;

		// no, find a free position
		for(n = 0 ; n < m->map_size ; n++)
		{
			if(m->ctrl[n] == 0)
			{
				idx = n;
				new_entry = true;
				break;
			}
		}
	}

	if(idx == -1)
		return UTIL_SMAP_STATUS_ERROR;

	entry = m->data + idx;

	if(new_entry)
	{
		// update control
		m->ctrl[idx] = 1;
		m->current_size += 1;
		strncpy((char *)entry->key,(char *)key,UTIL_SMAP_KEY_SIZE);
	}

	// save data
	entry->value = value;

	return UTIL_SMAP_STATUS_OK;
}

util_smap_status_t util_smap_get(util_smap_t *m, const uint8_t *key, util_smap_data_t *value)
{
	int16_t idx = util_smap_find_index(m,key);

	if(idx == -1)
		return UTIL_SMAP_STATUS_ERROR;

	*value = m->data[idx].value;

	return UTIL_SMAP_STATUS_OK;
}

util_smap_status_t util_smap_del(util_smap_t *m, const uint8_t *key)
{
	int16_t idx = util_smap_find_index(m,key);

	if(idx == -1)
		return UTIL_SMAP_STATUS_ERROR;

	// update control
	m->data[idx].value.u64 = 0;
	m->ctrl[idx] = 0;
	m->current_size -= 1;

	return UTIL_SMAP_STATUS_OK;
}

inline uint8_t util_smap_size(util_smap_t *m)
{
	return m->map_size;
}

inline uint8_t util_smap_current_size(util_smap_t *m)
{
	return m->current_size;
}

#if 0
uint32_t dev_map_area[UTIL_SMAP_CALC_SIZE(8)];
util_smap_t dev_map;
util_smap_data_t x[8];

util_smap_init(&dev_map,dev_map_area,8);

x[0].i32 =-1;
x[1].i32 =-2;
x[2].i32 =-3;
x[3].i32 =-4;
x[4].i32 =-5;
x[5].s8[0] = 'M';
x[5].s8[1] = 'B';
x[5].s8[2] = 'A';
x[5].s8[3] = '\0';
x[6].i32 =-7;
x[7].i32 =-8;

util_smap_set(&dev_map,(uint8_t*)"X0",x[0]);
util_smap_set(&dev_map,(uint8_t*)"X1",x[1]);
util_smap_set(&dev_map,(uint8_t*)"X2",x[2]);
util_smap_set(&dev_map,(uint8_t*)"X3",x[3]);
util_smap_set(&dev_map,(uint8_t*)"X4",x[4]);
util_smap_set(&dev_map,(uint8_t*)"X5",x[5]);
util_smap_set(&dev_map,(uint8_t*)"X6",x[6]);
util_smap_set(&dev_map,(uint8_t*)"X7",x[7]);
util_smap_set(&dev_map,(uint8_t*)"X8",x[7]);

util_smap_del(&dev_map,(uint8_t*)"X3");
util_smap_del(&dev_map,(uint8_t*)"X4");

x[5].i64 = 0;
util_smap_get(&dev_map,(uint8_t*)"X5",&x[5]);
util_smap_get(&dev_map,(uint8_t*)"X8",&x[5]);

x[3].d = 3.141516;
util_smap_set(&dev_map,(uint8_t*)"X5",x[3]);
util_smap_set(&dev_map,(uint8_t*)"X4",x[1]);

x[3].i64 = 0;
util_smap_get(&dev_map,(uint8_t*)"X3",&x[3]);

#endif
