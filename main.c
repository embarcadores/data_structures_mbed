#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "util_queue.h"
#include "util_queuecp.h"
#include "util_mpool.h"
#include "util_cbuf.h"

#define Q_SIZE       5
#define Q_ELEM_SIZE  500
#define Q_RAND_TRUE  (rand() % 10) >= 7
#define Q_RAND_FALSE (rand() % 10) >= 2
#define Q_RAND       10

static util_cbuf_t cbuf;
static util_queue_t queue;
static util_queuecp_t queuecp;
static util_mpool_t pool;

static util_cbuf_data_t cbuf_area[Q_ELEM_SIZE];
static util_queue_data_t queue_area[UTIL_QUEUE_CALC_SIZE(Q_SIZE,Q_ELEM_SIZE)];
static util_queuecp_data_t queuecp_area[UTIL_QUEUECP_CALC_SIZE(Q_SIZE,Q_ELEM_SIZE)];
static util_mpool_data_t pool_area[UTIL_MPOOL_CALC_SIZE(Q_SIZE,Q_ELEM_SIZE)];

static void test_mpool(void)
{
    util_mpool_status_t res;
    uint32_t i,j,k;
    static util_mpool_data_t *buffer[Q_SIZE];
    //static util_mpool_data_t **buffer;

    printf("-> MPOOL BASE\n");    
    for(i = 0 ; i < Q_SIZE ; i++)
    {        
        res = util_mpool_alloc(&pool,&buffer[i]);
        k = rand() % ('Z' - 'A' + 1) + 'A';
        memset(buffer[i],k,Q_ELEM_SIZE);
        if(res == UTIL_MPOOL_EMPTY)
        {
            printf("Pool empty\n");
            break;
        }
        else
        {
            printf("Pool alloc at %p %d %c\n",&buffer[i],i,buffer[i][0]);
        }
    }
    
    for(i = 0 ; i < Q_SIZE ; i++)
    {
        res = util_mpool_free(&pool,&buffer[i]);
        if(res != UTIL_MPOOL_OK)
        {
            printf("Pool free error\n");
            break;
        }
        else
        {
            printf("Pool free  at %p %d %c\n",&buffer[i],i,buffer[i][0]);
        }
    }

    i = 0;
    j = 0;
    
    printf("-> MPOOL RAND\n");
    while(j < Q_RAND)
    {
        if(Q_RAND_TRUE)
        {
            if(i < Q_SIZE)
            {
                res = util_mpool_alloc(&pool,&buffer[i]);
                // util_mpool_data_t *buf = 0;
                // res = util_mpool_alloc(&pool,&buf);
                // res = util_mpool_free(&pool,&buf);
                k = rand() % ('Z' - 'A' + 1) + 'A';
                memset(buffer[i],k,Q_ELEM_SIZE);
                if(res == UTIL_MPOOL_EMPTY)
                {
                    printf("Pool empty\n");
                    break;
                }
                else
                {
                    printf("Pool alloc at %p %d %c\n",&buffer[i],i,buffer[i][0]);
                    i++;
                }
            }
        }
        
        if(Q_RAND_FALSE)
        {                        
            if(i > 0)
            {
                i--;
                res = util_mpool_free(&pool,&buffer[i]);
                if(res != UTIL_MPOOL_OK)
                {
                    printf("Pool free error\n");
                    break;
                }
                else
                {
                    printf("Pool free  at %p %d %c\n",&buffer[i],i,buffer[i][0]);
                }
            }
        }
        j++;       
    }
    
    printf("-> MPOOL RAND MISSING %d\n",i);
    while(i > 0)
    {
        i--;
        res = util_mpool_free(&pool,&buffer[i]);
        if(res != UTIL_MPOOL_OK)
        {
            printf("Pool free error\n");
            break;
        }
        else
        {
            printf("Pool free  at %p %d %c\n",&buffer[i],i,buffer[i][0]);
        }        
    }
    
    
}

static void test_queuecp(void)
{
    uint32_t k, i, j;
    static uint8_t buffer[Q_ELEM_SIZE];
    
    i = 0;
    printf("-> QUEUECP BASE\n");
    while(1)
    {
        util_queuecp_status_t res;

        // escolhe uma letra entre 'A' e 'Z'
        k = rand() % ('Z' - 'A' + 1) + 'A';
        //preenche 
        memset(buffer,k,Q_ELEM_SIZE);
                        
        res = util_queuecp_put(&queuecp, buffer);
        
        if(res == UTIL_QUEUECP_FULL)
        {
            printf("Queuecp full\n");
            break;
        }

        printf("PUT %02d -> %c\n",i++,k);
    }
    
    i = 0;
    while(1)
    {
        util_queuecp_status_t res;
        
        res = util_queuecp_get(&queuecp, buffer);
        
        if(res == UTIL_QUEUECP_EMPTY)
        {
            printf("Queuecp empty\n");
            break;
        }
       
        printf("GET %02d -> %c\n",i++,buffer[0]);
    }  
    
    
    i = 0;
    j = 0;
    printf("-> QUEUECP RAND\n");
    while(j < Q_RAND)
    {
        if(Q_RAND_TRUE)
        {
            util_queuecp_status_t res;

            // escolhe uma letra entre 'A' e 'Z'
            k = rand() % ('Z' - 'A' + 1) + 'A';
            //preenche 
            memset(buffer,k,Q_ELEM_SIZE);
                            
            res = util_queuecp_put(&queuecp, buffer);
            
            if(res == UTIL_QUEUECP_OK)
                printf("PUT %02d -> %c\n",i++,k);
            else
                printf("Queue full\n");
        }
        
        if(Q_RAND_FALSE)
        {
            util_queuecp_status_t res;
           
            res = util_queuecp_get(&queuecp, buffer);
                        
            if(res == UTIL_QUEUECP_OK)
                printf("GET %02d -> %c\n",i++,buffer[0]);
            else 
                printf("Queue empty\n");
        }
        j++;       
    }
}

static void test_queue(void)
{
    uint32_t k, i, j;
    util_queuecp_data_t *buffer = NULL;
    
    i = 0;
    printf("-> QUEUE BASE\n");
    while(1)
    {
        util_queue_status_t res;
        
        res = util_queue_put_beg(&queue, &buffer);
        
        if(res == UTIL_QUEUE_FULL)
        {
            printf("Queue full\n");
            break;
        }
        
        // escolhe uma letra entre 'A' e 'Z'
        k = rand() % ('Z' - 'A' + 1) + 'A'; 
        // preenche
        memset(buffer,k,Q_ELEM_SIZE);
           
        util_queue_put_end(&queue);

        printf("PUT %02d -> %02d / %c\n",i++,Q_ELEM_SIZE,k);
    }
    
    i = 0;
    while(1)
    {
        util_queue_status_t res;
        
        res = util_queue_get_beg(&queue, &buffer);
        
        if(res == UTIL_QUEUE_EMPTY)
        {
            printf("Queue empty\n");
            break;
        }
               
        util_queue_get_end(&queue);

        printf("GET %02d -> %02d / %c\n",i++,Q_ELEM_SIZE,buffer[0]);
        
    }  
    
    
    i = 0;
    j = 0;
    printf("-> QUEUE RAND\n");
    while(j < Q_RAND)
    {
        util_queue_status_t res;
        
        if(Q_RAND_TRUE)
        {
            res = util_queue_put_beg(&queue, &buffer);
            
            if(res == UTIL_QUEUE_OK)
            {
                // escolhe uma letra entre 'A' e 'Z'
                k = rand() % ('Z' - 'A' + 1) + 'A'; 
                // preenche        
                memset(buffer,k,Q_ELEM_SIZE);
                
                util_queue_put_end(&queue);            

                printf("PUT %02d -> %02d / %c\n",i++,Q_ELEM_SIZE,k);
            }
            else
                printf("Queue full\n");
        }
        
        if(Q_RAND_FALSE)
        {
            res = util_queue_get_beg(&queue, (void *) &buffer);
            
            if(res == UTIL_QUEUE_OK)
            {
                util_queue_get_end(&queue);
                printf("GET %02d -> %02d / %c\n",i--,Q_ELEM_SIZE,buffer[0]);
            }
            else 
                printf("Queue empty\n");
        }
        j++;       
    }
}

int main(void)
{
    util_cbuf_data_t c;
    util_mpool_data_t **buffer;

    srand(time(NULL));
        
    util_cbuf_init(&cbuf,cbuf_area,Q_ELEM_SIZE);
    util_queuecp_init(&queuecp, queuecp_area, Q_SIZE, Q_ELEM_SIZE);
    util_queue_init(&queue, queue_area, Q_SIZE, Q_ELEM_SIZE);
    util_mpool_init(&pool, pool_area, Q_SIZE, Q_ELEM_SIZE);

    util_cbuf_put(&cbuf,0xab);
    util_cbuf_get(&cb,&c);

    util_mpool_alloc(&pool,&buffer);
    memcpy(buffer,"teste",5);
    util_mpool_free(&pool,&buffer);
    
    printf("=== QUEUE CP ===\n");
    test_queuecp();
    printf("=== QUEUE    ===\n");
    test_queue();    
    printf("=== MPOOL    ===\n");
    test_mpool();    

    return 0;
}

