#include "stdio.h"
#include <pthread.h>
#include "stdlib.h"
#include "time.h"

#define CBUFFER_IMP
#include "c_buffer.h"
#define TIMERS_IMP
#include "timers.h"

#define BUFFER_SZ 4096
#define TIME_BETWEEN_WRITING_MS 2000
static uint8_t in_buffer[BUFFER_SZ];
static cb_t cb;
static mytimer_t tim;

void * dma_th(void * arg){
    UNUSED_VAR(arg);
    static uint32_t last = 0;
    
    while(1){ // second main loop
        uint32_t now = (uint32_t) (clock() / CLOCKS_PER_SEC ) * 1000U; 
        if((now - last) >= 10){
            /*Get a random size of the in msg*/
            uint16_t size = (uint16_t)((rand()*1.0f / RAND_MAX * 1.0f) * 1024);

            for(uint16_t i = 0; i < size; i++){
                /*write a random msg*/
                in_buffer[i] = (uint8_t)((rand()*1.0f / RAND_MAX * 1.0f) * 256);
            }

            CbDmaWrInc(&cb, size);
            last = now;
        }
    }

    return NULL;
}

void * timer_th(void * arg){
    UNUSED_VAR(arg);

    MyTimerStart(&tim, 5000);
    uint32_t last = 0;
    while(1){
        uint32_t now = (uint32_t) (clock() / CLOCKS_PER_SEC ) * 1000U; 
        if((now - last) >= 1){ // evry 1 ms
            MyTimerCycle(&tim);
            last = now;
            printf("clock: %u, %u \n", now, last);
        }
        
    }
}

int main(){
    pthread_t thread1, thread2;
    uint32_t last = 0;
    uint8_t out;
    srand(time(NULL));

    CbInit(&cb, in_buffer, BUFFER_SZ, "test_cb");
    MyTimerInit(&tim);

    UNUSED_FN pthread_create(&thread1,NULL,dma_th,NULL);
    UNUSED_FN pthread_create(&thread2,NULL,timer_th,NULL);

    

    while(1){
        uint32_t now = (uint32_t) (clock() / CLOCKS_PER_SEC ) * 1000U; 
        if(tim.end){
            MyTimerReset(&tim);
            // size_t red = CbRead(&cb, &out, 1);
            // printf("cb_stats: last read: %i, full cnt: %lu, dma_cnt: %lu \n", out, cb.full_cnt, cb.dma_cnt);
            printf("ms passed: %u", now - last);
            last = now;
        }
    }
    return 0;
}