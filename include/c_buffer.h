#ifndef C_BUFFER_H_
#define C_BUFFER_H_
#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "assert.h"



#ifndef UNUSED_VAR
#define UNUSED_VAR(a) (void)(a)
#endif
#ifndef UNUSED_FN
#define UNUSED_FN (void)
#endif
#ifndef ARRAY_LEN
#define ARRAY_LEN(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

#define CB_ASSERT(b) assert(b)

typedef struct
{
    uint8_t *data;
    size_t size;
    size_t write;
    size_t read;
    size_t mask;
    const char *name;
    
    size_t read_last;
    size_t dma_cnt;
    size_t full_cnt;
} cb_t;


// APIs
void CbInit(cb_t* cb, uint8_t * data, size_t size, const char *name);
size_t CbWrite(cb_t * cb, const uint8_t * item, size_t n);
size_t CbRead(cb_t * cb, uint8_t * out, size_t n);
size_t CbReadUntil(cb_t * cb, uint8_t * out, size_t max, uint8_t byte);

// DMA
void CbDmaSynStart(cb_t * cb, uint8_t start_B);
void CbDmaWrInc(cb_t * cb, int32_t ndtr);

#ifdef CBUFFER_IMP


static inline uint8_t CbCheckSize(size_t size){
    if (size == 0) return 0;
    // size es potencia de 2 si tiene un único bit a 1
    return (uint8_t)((size & (size - 1)) == 0);
}


/// @brief Initialize an cicurlar buffer 
/// @param data real data memory
/// @param size 
/// @param name 
/// @return 
void CbInit(cb_t* cb, uint8_t * data, size_t size, const char *name){
    CB_ASSERT(cb != NULL && data != NULL && size != 0 && CbCheckSize(size));
    if(!name) name = "";
    *cb = (cb_t){ .data=data, .size=size, .mask=size-1, .write=0, .read=0,
                  .name=name, .read_last=0, .dma_cnt=0, .full_cnt=0 };
}




static inline bool CbIsEmpty(cb_t * cb){
    return (cb->write & cb->mask) == (cb->read & cb->mask);
}

static inline bool CbIsFull(cb_t * cb){
    return ((cb->read + 1) & cb->mask) == (cb->write & cb->mask);
}

static inline size_t CbDataCount(cb_t * cb){
    return ((cb->write - cb->read) & cb->mask);

}

static inline size_t CbContiguousDataCount(cb_t * cb){
    int aux = (int)(cb->write & cb->mask) - (cb->read & cb->mask);
    if(aux < 0){ // read after write
        return cb->size - (cb->read & cb->mask);
    }else{
        return CbDataCount(cb);
    }
}

static inline size_t CbEmptyCount(cb_t * cb){
    return (cb->mask - ((cb->write - cb->read) & cb->mask));
}

static inline size_t CbContiguousEmptyCount(cb_t * cb){
    size_t w = cb->write & cb->mask, r = cb->read & cb->mask;
    if (w < r) {
        return (r - w - 1);
    } else {
        size_t end = cb->size - w;
        return (r == 0) ? (end - 1) : end;
    }
}

static inline void CbWriteInc(cb_t * cb, size_t num){
    size_t gap = CbEmptyCount(cb);
    cb->write += num;
    if(num > gap){
        cb->read = cb->write + 1;
        cb->read_last = cb->read;
        cb->full_cnt++;
    }
}

static inline void CbReadInc(cb_t * cb, size_t num){
    size_t avail = (cb->write - cb->read) & cb->mask;
    if (num > avail) num = avail;
    cb->read += num;
    cb->read_last = cb->read;
}


/// @brief Write a single item or a set of items in the buffer 
/// @param cb 
/// @param item 
/// @param n 
/// @return 
size_t CbWrite(cb_t * cb, const uint8_t * item, size_t n){
    CB_ASSERT((cb != NULL && item != NULL && n > 0));

    if(n == 1){
        cb->data[cb->write & cb->mask] = *item;
        cb->write++;
        if((cb->write & cb->mask) == (cb->read & cb->mask)){
            cb->read++;
            cb->read_last = cb->read;
            cb->full_cnt++; // high number implies that generator is faster than consumer
        }
        return 1;
    }else{
        if(n < cb->size){
            size_t v_write = cb->write;
            size_t bytes_wr = 0;
            while(bytes_wr < n){
                cb->data[v_write & cb->mask] = item[bytes_wr];
                bytes_wr++;
                v_write++;
            }
            CbWriteInc(cb, n);
            return n;
        }else{
            return 0;
        }

    }
}

/// @brief 
/// @param cb 
/// @param out 
/// @param n 
/// @return 
size_t CbRead(cb_t * cb, uint8_t * out, size_t n){
    CB_ASSERT((cb != NULL && out != NULL && n > 0));

    if(CbIsEmpty(cb)){return 0;}
    if (n == 1){
        *out = cb->data[cb->read & cb->mask];
        cb->read++;
        cb->read_last = cb->read;
        return 1;
    }else{
        size_t available_bytes = CbDataCount(cb);
        if(available_bytes < n){n = available_bytes;}

        size_t v_read = cb->read;
        size_t bytes_rd = 0;
        while(bytes_rd < n){
            out[bytes_rd++] = cb->data[v_read & cb->mask];
            v_read++;
        }

        CbReadInc(cb, n);
        return n;

    }
}


size_t CbReadUntil(cb_t * cb, uint8_t * out, size_t max, uint8_t byte){
    CB_ASSERT(cb && out && max > 0);
    if (CbIsEmpty(cb)) return 0;

    size_t avail = (cb->write - cb->read) & cb->mask;
    if (max > avail) max = avail;

    size_t v = cb->read, i = 0;
    for (; i < max; ++i, ++v){
        out[i] = cb->data[v & cb->mask];
        if (out[i] == byte){ i++; break; }
    }
    CbReadInc(cb, i);
    return i;
}


// DMA utils


void CbDmaSynStart(cb_t * cb, uint8_t start_B){

    while(!CbIsEmpty(cb) && (cb->data[cb->read & cb->mask] != start_B)){
        CbReadInc(cb, 1);
    }
}

void CbDmaWrInc(cb_t * cb, int32_t ndtr){
    if (ndtr < 0) ndtr = 0;
    if ((size_t)ndtr > cb->size) ndtr = (int32_t)cb->size;

    size_t produced = (cb->size - (size_t)ndtr) & cb->mask;
    size_t delta    = (produced - cb->dma_cnt) & cb->mask;

    // Publica delta nuevos bytes; política overwrite centralizada en CbWriteInc
    cb->dma_cnt = produced;
    CbWriteInc(cb, delta);
}


#endif // CBUFFER_IMP

#endif // C_BUFFER_H_