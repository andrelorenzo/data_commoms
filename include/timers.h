
#ifndef TIMERS_H_
#define TIMERS_H_
#include <stdint.h>

#ifndef UNUSED_VAR
#define UNUSED_VAR(a) (void)(a)
#endif
#ifndef UNUSED_FN
#define UNUSED_FN (void)
#endif
#ifndef ARRAY_LEN
#define ARRAY_LEN(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

#if defined(_WIN32)
  #include <windows.h>
  static inline uint64_t now_ms(void){
      static LARGE_INTEGER f = {0};
      LARGE_INTEGER c;
      if(!f.QuadPart){ QueryPerformanceFrequency(&f); }
      QueryPerformanceCounter(&c);
      return (uint64_t)( (c.QuadPart * 1000ULL) / f.QuadPart );
  }
#elif defined(__unix__) || defined(__APPLE__)
  #include <time.h>
  static inline uint64_t now_ms(void){
      struct timespec ts;
      clock_gettime(CLOCK_MONOTONIC, &ts);
      return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
  }
#else
  // Fallback genérico (menos preciso)
  #include <time.h>
  static inline uint64_t now_ms(void){
      return (uint64_t)((double)clock() * 1000.0 / (double)CLOCKS_PER_SEC);
  }
#endif


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
