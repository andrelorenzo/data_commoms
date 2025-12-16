#ifndef LOGGER_H_
#define LOGGER_H_
#include "stdarg.h"
#include "stdio.h"

typedef enum{
    DEBUG = 0U,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NONE
}verb_e;

static verb_e max_verbosity = WARN;

#if defined(_WIN32) || defined(__unix__) || defined(__APPLE__)
#define LOGGER_WEAK __attribute__((weak))
#else
#define LOGGER_WEAK __weak
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(a) (void)(a)
#endif
#ifndef UNUSED_FN
#define UNUSED_FN (void)
#endif
#ifndef ARRAY_LEN
#define ARRAY_LEN(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

void Logger(verb_e verbosity, const char * format, ...);
verb_e LoggerGetVerbsity(void);
void LoggerSetVerbsity(verb_e);


#define LOGGER_IMP 
#ifdef LOGGER_IMP

/// @brief Get the actual verbosity level
/// @return the actual verbosity level
verb_e LoggerGetVerbsity(){
    return max_verbosity;
}
/// @brief Set the max verbosity level
/// @param max_verb max verbosity level, i.e if max_verb is set to WARN, only WARN, ERROR AND FATAL will be printed 
void LoggerSetVerbsity(verb_e max_verb){
    max_verbosity = max_verb;
}

/// @brief Function must be ovewritten with whatever implementation is of your need, some examples are provided
/// @param msg // msg to be send
/// @param size // size of the msg to me send
LOGGER_WEAK void printOut(const char * msg, size_t size){
    UNUSED_VAR(msg);
    UNUSED_VAR(size);

    // (WINDOWS / LINUX)    printf(msg);
    // (STM32)              HAL_UART_Transmit(&huart2, (uint8_t*)msg, size, 100);
    // (ESP32)              uart_write_bytes(uart_num, (const char*)msg, size);
    // (ARDUINO)            Serial.println(msg);
}
 
/// @brief Actual logging function
/// @param verbosity verbosity of the message, never set it to NONE
/// @param format msg to be send
/// @param  variadic arguments of the function
void Logger(verb_e verbosity, const char * format, ...){

    if(verbosity >= max_verbosity){
        va_list args;
        va_start(args, verbosity);
        va_end(args);
        char msg[1024];
        int size = vsprintf(msg, format, args);
        printOut(msg, size);

    }

}
#endif // LOGGER_IMP

#endif