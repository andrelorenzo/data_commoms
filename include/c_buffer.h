#ifndef C_BUFFER_H_
#define C_BUFFER_H_
#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "assert.h"



#define UNUSED_VAR(a) (void)(a)
#define UNUSED_FN (void)
#define ARRAY_LEN(arr) sizeof(arr)/sizeof((arr)[0])

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
    size_t aux = size;
    uint8_t safe_exit = 0;
    if(aux == 0){return (uint8_t)0;}

    while((aux % 1) != 1){
        aux >>= 1;
        safe_exit++;
        if(safe_exit > 128){
            return 0;
        }
    }
    if(aux != 1){return 0;}
    return 1;
}


/// @brief Initialize an cicurlar buffer 
/// @param data real data memory
/// @param size 
/// @param name 
/// @return 
void CbInit(cb_t* cb, uint8_t * data, size_t size, const char *name){
    CB_ASSERT(CbCheckSize(size) == 1 || size != 0 || data != NULL);
    if(name == NULL){name = "";}

    *cb = (cb_t){
        .data = data,       // reral memory to keep the data
        .mask = size - 1,   // isntead of % qe use binary arithmetic
        .name = name,       // can be null
        .read = 0,          // pointer to the read object
        .write = 0,         // pointer to the write object
        .size = size,        // size of the data array
        .dma_cnt = 0,
        .full_cnt = 0,
        .read_last = 0
    };
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
    int aux = (int)(cb->write & cb->mask) - (cb->read & cb->mask);
    if(aux < 0){ // read after write
        return CbEmptyCount(cb);
    }else{
        return cb->size - (cb->write & cb->mask);
    }
}

static inline void CbWriteInc(cb_t * cb, size_t num){
    cb->write += num;
    if(num > CbEmptyCount(cb)){
        cb->read = cb->write + 1;
        cb->read_last = cb->read;
        cb->full_cnt++;
    }
}

static inline void CbReadInc(cb_t * cb, size_t num){
    if(num < CbDataCount(cb)){
        cb->read += num;
        cb->read_last = cb->read;
    }
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
    if(n == 1){

        *out = cb->data[cb->read & cb->mask];
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
    CB_ASSERT((cb != NULL && out != NULL && max > 0));

    if(CbIsEmpty(cb)){return 0;}
    size_t available_bytes = CbDataCount(cb);
    if(available_bytes < max){max = available_bytes;}

    size_t v_read = cb->read;
    size_t bytes_rd = 0;
    while(bytes_rd < max){
        out[bytes_rd++] = cb->data[v_read & cb->mask];
        v_read++;
        if(out[bytes_rd-1] == byte){

        }
    }

    CbReadInc(cb, bytes_rd);
    return bytes_rd;
}

// DMA utils


void CbDmaSynStart(cb_t * cb, uint8_t start_B){

    while(!CbIsEmpty(cb) && (cb->data[cb->read & cb->mask] != start_B)){
        CbReadInc(cb, 1);
    }
}

void CbDmaWrInc(cb_t * cb, int32_t ndtr){
    int32_t bytes_wr = cb->size - ndtr;
    cb->write += (bytes_wr - cb->dma_cnt) & cb->mask;
    cb->dma_cnt = bytes_wr;
}


#endif // CBUFFER_IMP

#endif // C_BUFFER_H_