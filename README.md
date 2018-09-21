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