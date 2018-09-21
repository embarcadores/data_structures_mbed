# Data structures for embedded systems

This repository is a collection of some data structures targeting bare metal embedded systems.
All data structures are allocated from RAM using the global area for variables, no allocations during runtime.

Feel free to use, contribute and report bugs.

# Available data structures

 * `util_mpool.c`: Memory pool
 * `util_cbuf.c`: Circular buffer
 * `util_queue.c`: Queue
 * `util_queuece.c`: No copy queue
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
