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

typedef enum{
    GENERAL_READ = 0U,
    DMA_WRITE,
    MAX_TIMERS_IND
}timer_e;

static uint8_t in_buffer[BUFFER_SZ];
static cb_t cb;
static mytimer_t tim[MAX_TIMERS_IND];

// this is like a dma interrupt callback in stm32
void *dma_th(void *arg){
    (void)arg;
    // simulamos NDTR: empieza “lleno” (no escribió nada todavía)
    uint32_t ndtr = BUFFER_SZ;
    uint32_t last = now_ms();

    for(;;){
        uint32_t now = now_ms();
        if ((now - last) >= 10) {                // cada 10 ms “llega” un bloque
            // tamaño aleatorio de “lote” entrante
            uint16_t chunk = (uint16_t)(rand() % 50);

            // posición física actual donde escribir (antes de decrementar NDTR)
            uint32_t produced_before = (BUFFER_SZ - ndtr) & (BUFFER_SZ-1);
            uint32_t w_phys = produced_before;   // write físico actual

            // escribe la “llegada” respetando wrap
            uint32_t till_end = BUFFER_SZ - w_phys;
            uint32_t n1 = (chunk < till_end) ? chunk : till_end;
            for (uint32_t i=0; i<n1; ++i) {
                in_buffer[w_phys + i] = (uint8_t)(rand() & 0xFF);
            }
            if (chunk > n1) {
                uint32_t n2 = chunk - n1;
                for (uint32_t i=0; i<n2; ++i) {
                    in_buffer[i] = (uint8_t)(rand() & 0xFF);
                }
            }

            // actualiza NDTR como haría el hardware: decrece y envuelve
            if (chunk <= ndtr) {
                ndtr -= chunk;
            } else {
                // si “pasó de rosca”, simula el reload del circular
                uint32_t over = chunk - ndtr;
                ndtr = BUFFER_SZ - (over % BUFFER_SZ);
            }

            // publica al ring el nuevo NDTR
            CbDmaWrInc(&cb, (int32_t)ndtr);

            last = now;
        }
    }
    return NULL;
}

// this is like a timer interrupt callback in stm32
void * timer_th(void * arg){
    UNUSED_VAR(arg);
    uint32_t last = 0;
    while(1){
        uint32_t now = now_ms();
        if((now - last) >= 1){ // evry 1 ms
            for(size_t i = 0; i < MAX_TIMERS_IND; i++) MyTimerCycle(&tim[i]);
            last = now;
        }
        
    }
}

int main(){
    pthread_t th_dma, th_tim;
    srand((unsigned)time(NULL));

    CbInit(&cb, in_buffer, BUFFER_SZ, "test_cb");
    for (size_t i=0; i<MAX_TIMERS_IND; ++i) MyTimerInit(&tim[i]);

    // Timers de muestra (usa el GENERAL_READ o quítalo si prefieres el modo B de lectura)
    MyTimerStart(&tim[GENERAL_READ], 5000);

    pthread_create(&th_dma, NULL, dma_th, NULL);
    pthread_create(&th_tim, NULL, timer_th, NULL);

    // --- MODO A: consumidor muy lento (cada 5 s) ---
#if 0
    uint8_t out = 0;
    for(;;){
        if (tim[GENERAL_READ].end) {
            size_t got = CbRead(&cb, &out, 1);
            printf("[A] read=%zu byte=%u  used=%zu  free=%zu  full_cnt=%zu  dma_cnt=%zu\n",
                   got, out, (size_t)((cb.write - cb.read) & cb.mask),
                   (size_t)(cb.mask - ((cb.write - cb.read) & cb.mask)),
                   cb.full_cnt, cb.dma_cnt);
            MyTimerReset(&tim[GENERAL_READ]);
        }
    }
#else
    // --- MODO B: consumidor “normal” (cada 50 ms lee 512B) ---
    uint8_t tmp[512];
    uint64_t last = now_ms();
    for(;;){
        uint64_t now = now_ms();
        if ((now - last) >= 50) {
            size_t got = CbRead(&cb, tmp, sizeof(tmp));
            printf("[B] read=%zu  used=%zu  free=%zu  full_cnt=%zu\n",
                    got, (size_t)((cb.write - cb.read) & cb.mask),
                   (size_t)(cb.mask - ((cb.write - cb.read) & cb.mask)),
                   cb.full_cnt);
            last = now;
        }
    }
#endif
}
