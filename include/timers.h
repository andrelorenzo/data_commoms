
#ifndef TIMERS_H_
#define TIMERS_H_


#define UNUSED_VAR(a) (void)(a)
#define UNUSED_FN (void)
#define ARRAY_LEN(arr) sizeof(arr)/sizeof((arr)[0])

#include "stdint.h"
typedef struct{
    bool on;
    bool end;
    uint32_t count;
    uint32_t len;
}mytimer_t;


void MyTimerInit(mytimer_t * tim);
void MyTimerStart(mytimer_t * tim, uint32_t timeout_ms);
void MyTimerStop(mytimer_t * tim);
void MyTimerCycle(mytimer_t * tim);
void MyTimerReset(mytimer_t * tim);


#ifdef TIMERS_IMP
void MyTimerInit(mytimer_t * tim){
    tim->count = 0;
    tim->end = false;
    tim->on = false;
}
void MyTimerStart(mytimer_t * tim, uint32_t timeout){
    tim->on = true;
    tim->len = timeout;
}
void MyTimerStop(mytimer_t * tim){
    tim->count = 0;
    tim->end = false;
    tim->on = false;
}
void MyTimerCycle(mytimer_t * tim){
    if(tim->on == true){
        if(tim->count > tim->len){
            tim->end = true;
        }else{
            tim->count++;
        }
    }
}
void MyTimerReset(mytimer_t * tim){
    tim->count = 0;
    tim->end = false;
    tim->on = true;
}
#endif // TIMERS_IMP


#endif // TIMERS_H_
