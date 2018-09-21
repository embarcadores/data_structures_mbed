# Data structures for embedded systems

This repository is a collection of some data structures targeting bare metal embedded systems.
All data structures are allocated from RAM using the global area for variables, no allocations during runtime.

Feel free to use, contribute and report bugs.

# Available data structures

 * `util_mpool.c`: Memory pool
 * `util_cbuf.c`: Circular buffer
 * `util_queue.c`: Queue without copies
 * `util_queuecp.c`: Queue with copies
 * `util_queuehdr.c`: No copy queue with header support
 * `util_smap.c`: Simple hash map
 * `util_map.c`: Search optimized hash map

## Memory pools

```C
// First, allocate the space from global variables area. 
// For instance, 10 areas of 32 bytes each one:
util_mpool_data_t area[UTIL_MPOOL_CALC_SIZE(10,32)];

// Create a pool handler variable.
util_mpool_t pool_ctrl;

// You may have several pool and pool handlers !

void main(void)
{
    // Pointer to the allocated area.
    util_mpool_data_t *data;

    // Init the pool area/control
    util_mpool_init(&pool_ctrl,area,10,32);

    // Allocate an area from the pool
    if(util_mpool_alloc(&pool_ctrl,&data) == UTIL_MPOOL_EMPTY)
    {
        // Ops, pool is empty !
    }
    else
    { 
        // Use the area
        // ...
        // free the area
        util_mpool_free(&pool_ctrl,&data);
    }
}

```

# Circular buffer

```C
// allocation
util_cbuf_data_t area[100];
// handler
util_cbuf_t cb;
// initialization
util_cbuf_init(&cb,area,100);

// inserting bytes
if(util_cbuf_put(&cb,60) == UTIL_CBUF_FULL)
{
    // Ops ... full !	
}

// retrieving
util_cbuf_data_t c;

if(util_cbuf_get(&cb,&c) == UTIL_CBUF_EMPTY)
{
    // Ops, empty
}
```

# Qeueue without copies

For this data structure, the user does not need to copy the buffer to/from queue. 
Instead, it uses a dual operation transaction where the `begin` operation returns a 
valid area for using and `end` operation finish the transaction, adding or removing
the buffer to the queue.

```C
// allocation and queue handler (number of elements, element size)
util_queue_data_t area[UTIL_QUEUE_CALC_SIZE(10,35)];
util_queue_t q;

// initialization
util_queue_init(&q,area,10,35);

// destination
util_queuecp_data_t *element = NULL;

// first we retrieve the pointer to the area (begin)
if(util_queue_put_beg(&q,&element) == UTIL_QUEUE_FULL)
{
    // Ops ... no space 
}
else
{
    // use the pointer here
    // ...
    // finish the operation, buffer is added to the queue
	util_queue_get_end(&q);
}

// start retrieving a new element from queue
if(util_queue_get_beg(&q,&element) == UTIL_QUEUE_EMPTY)
{
    // Ops, empty !
}
else
{
    // use the buffer
    // ...
    // finish the operation, buffer is removed from the queue
	util_queue_get_end(&q);
}
```

# Queue with copy

Buffer copies are required for this data structure.

```C
// reserving area and queue handler
util_queuecp_data_t area[UTIL_QUEUECP_CALC_SIZE(10,35)];
util_queuecp_t q;

// initialization
util_queuecp_init(&q,area,10,35);

// temporary buffer
util_queuecp_data_t data[35];

// inserting (memcpy is used internally)
if(util_queuecp_put(&q,data) == UTIL_QUEUECP_FULL)
{
	// Ops
}

// retrieving bufffer (memcpy is used internally)
if(util_queuecp_get(&q,data) == UTIL_QUEUECP_EMPTY)
{
    // Ops
}
```

# Hash map with linked lists

The map uses unions to store the values and the following data types are supported:

```C
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
```
Strings are used as map keys. Basic usage is below.

```C
// reserving 8 entries
uint32_t dev_map_area[UTIL_MAP_CALC_SIZE(8)];
// map handler
util_map_t dev_map;

// temporary data
util_map_data_t x[8];

// initialization
util_map_init(&dev_map,dev_map_area,8);

// usage, diverse types
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

// inserting/updating values into map
util_map_set(&dev_map,(uint8_t*)"X0",x[0]);
util_map_set(&dev_map,(uint8_t*)"X1",x[1]);
util_map_set(&dev_map,(uint8_t*)"X2",x[2]);
util_map_set(&dev_map,(uint8_t*)"X3",x[3]);
util_map_set(&dev_map,(uint8_t*)"X4",x[4]);
util_map_set(&dev_map,(uint8_t*)"X5",x[5]);
util_map_set(&dev_map,(uint8_t*)"X6",x[6]);
util_map_set(&dev_map,(uint8_t*)"X7",x[7]);
util_map_set(&dev_map,(uint8_t*)"X8",x[7]);

// removing values from map
util_map_del(&dev_map,(uint8_t*)"X3");
util_map_del(&dev_map,(uint8_t*)"X4");

// retrieving
x[5].value.i64 = 0;
util_map_get(&dev_map,(uint8_t*)"X5",&x[5]);
util_map_get(&dev_map,(uint8_t*)"X8",&x[5]);

// inserting/updating values into map
x[3].value.d = 3.141516;
util_map_set(&dev_map,(uint8_t*)"X5",x[3]);
util_map_set(&dev_map,(uint8_t*)"X4",x[1]);

x[3].value.i64 = 0;
util_map_get(&dev_map,(uint8_t*)"X3",&x[3]);
```