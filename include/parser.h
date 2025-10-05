#ifndef PARSER_H_
#define PARSER_H_
#include "stdbool.h"
#include "stdint.h"
#include "stddef.h"
#include "assert.h"
#include "string.h"

#define FLAG_ASSERT(b) assert(b)    

#ifndef FLAGS_CAP
#define FLAGS_CAP 256
#endif // FLAGS_CAP

#ifndef FLAG_LIST_INIT_CAP
#define FLAG_LIST_INIT_CAP 1024
#endif // FLAG_LIST_INIT

typedef struct {
    const char *items[FLAG_LIST_INIT_CAP];
    size_t count;
    size_t capacity;
} flag_list_t;

bool        *FlagBool  (const char * name, bool def_val     , const char * desc);
uint8_t     *FlagUint8 (const char * name, uint8_t   def_val, const char * desc);
uint16_t    *FlagUint16(const char * name, uint16_t  def_val, const char * desc);
uint64_t    *FlagUint64(const char * name, uint64_t  def_val, const char * desc);
uint32_t    *FlagUint32(const char * name, uint32_t  def_val, const char * desc);
int         *FlagInt   (const char * name, int       def_val, const char * desc);
float       *FlagFloat (const char * name, float     def_val, const char * desc);
double      *FlagDouble(const char * name, double    def_val, const char * desc);
char        **FlagStr  (const char * name, char *    def_val, const char * desc);
size_t      *FlagSize  (const char * name, size_t *  def_val, const char * desc);
flag_list_t *FlagList  (const char * name                   , const char * desc); 

bool        FlagParse(int argc, char ** argv);
const char *FlagGetProgramName(void);
void        FlagPrintHelp();
void        FlagPrintError();

#ifdef PARSER_IMP


#endif // PARSER_IMP


typedef enum {
    FLAG_BOOL = 0,
    FLAG_UINT8,
    FLAG_UINT16,
    FLAG_UINT32,
    FLAG_UINT64,
    FLAG_INT,
    FLAG_FLOAT,
    FLAG_DOUBLE,
    FLAG_SIZE,
    FLAG_STR,
    FLAG_LIST,
    COUNT_FLAG_TYPES,
} flag_type_e;

typedef union {
    bool        as_bool;
    uint8_t     as_uint8;
    uint16_t    as_uint16;
    uint32_t    as_uint32;
    uint64_t    as_uint64;
    int         as_int;
    float       as_float;
    double      as_double;
    char       *as_str;
    size_t      as_size;
    flag_list_t as_list;
} flag_val_u;

typedef enum {
    FLAG_NO_ERROR = 0,
    FLAG_ERROR_UNKNOWN,
    FLAG_ERROR_NO_VALUE,
    FLAG_ERROR_INVALID_NUMBER,
    FLAG_ERROR_INTEGER_OVERFLOW,
    FLAG_ERROR_FLOAT_OVERFLOW,
    FLAG_ERROR_DOUBLE_OVERFLOW,
    FLAG_ERROR_INVALID_SIZE_SUFFIX,
    COUNT_FLAG_ERRORS,
} flag_error_e;

typedef struct {
    flag_type_e type;
    const char *name;
    const char *desc;
    flag_val_u val;
    flag_val_u def;
} flag_t;

typedef struct {
    flag_t flags[FLAGS_CAP];
    size_t flags_count;

    flag_error_e flag_error;
    char *flag_error_name;
    char *flag_error_value;

    const char *program_name;

    int rest_argc;
    char **rest_argv;
} flag_ctx_t;


static flag_ctx_t flag_ctx;



static flag_t * flag_new_flag_(flag_ctx_t * ctx, flag_type_e _type, const char * _name, const char * _desc){
    FLAG_ASSERT(ctx->flags_count < FLAGS_CAP);
    flag_t * f =  &ctx->flags[ctx->flags_count++];
    memset(f, 0, sizeof(f));

    f->type = _type;
    f->name = _name;
    f->desc = _desc;

    return f;
}

///=======================================BOOL=======================================   
static bool * flag_new_bool_(flag_ctx_t * ctx, const char * _name, bool _def, const char * _desc){
    flag_t * f = flag_new_flag_(ctx, FLAG_BOOL, _name, _desc);
    f->def.as_bool = _def;
    if(_def != NULL){
        f->val.as_bool = _def;
    }
    return &f->val.as_bool;
}

bool *FlagBool  (const char * name, bool def_val, const char * desc){
    return flag_new_bool_(&flag_ctx, name, def_val, desc);
}
///==================================================================================

///=======================================UINT8=======================================   
static uint8_t * flag_new_uint8_(flag_ctx_t * ctx, const char * _name, uint8_t _def, const char * _desc){
    flag_t * f = flag_new_flag_(ctx, FLAG_UINT8, _name, _desc);
    f->def.as_uint8 = _def;
    if(_def != NULL){
        f->val.as_uint8 = _def;
    }
    return &f->val.as_uint8;
}

uint8_t *FlagUint8  (const char * name, uint8_t def_val, const char * desc){
    return flag_new_uint8_(&flag_ctx, name, def_val, desc);
}
///==================================================================================

///=======================================UINT16=======================================   
static uint16_t * flag_new_uint16_(flag_ctx_t * ctx, const char * _name, uint16_t _def, const char * _desc){
    flag_t * f = flag_new_flag_(ctx, FLAG_UINT16, _name, _desc);
    f->def.as_uint16 = _def;
    if(_def != NULL){
        f->val.as_uint16 = _def;
    }
    return &f->val.as_uint16;
}

uint16_t *FlagUint16  (const char * name, uint16_t def_val, const char * desc){
    return flag_new_uint16_(&flag_ctx, name, def_val, desc);
}
///==================================================================================

///=======================================UINT32=======================================   
static uint32_t * flag_new_uint32_(flag_ctx_t * ctx, const char * _name, uint32_t _def, const char * _desc){
    flag_t * f = flag_new_flag_(ctx, FLAG_UINT32, _name, _desc);
    f->def.as_uint32 = _def;
    if(_def != NULL){
        f->val.as_uint32 = _def;
    }
    return &f->val.as_uint32;
}

uint32_t *FlagUint32  (const char * name, uint32_t def_val, const char * desc){
    return flag_new_uint32_(&flag_ctx, name, def_val, desc);
}
///==================================================================================

///=======================================UINT64=======================================   
static uint64_t * flag_new_uint64_(flag_ctx_t * ctx, const char * _name, uint64_t _def, const char * _desc){
    flag_t * f = flag_new_flag_(ctx, FLAG_UINT64, _name, _desc);
    f->def.as_uint64 = _def;
    if(_def != NULL){
        f->val.as_uint64 = _def;
    }
    return &f->val.as_uint64;
}

uint64_t *FlagUint64  (const char * name, uint64_t def_val, const char * desc){
    return flag_new_uint64_(&flag_ctx, name, def_val, desc);
}
///==================================================================================

///=======================================INT=======================================   
static int * flag_new_int_(flag_ctx_t * ctx, const char * _name, int _def, const char * _desc){
    flag_t * f = flag_new_flag_(ctx, FLAG_INT, _name, _desc);
    f->def.as_int = _def;
    if(_def != NULL){
        f->val.as_int = _def;
    }
    return &f->val.as_int;
}

int *FlagInt  (const char * name, int def_val, const char * desc){
    return flag_new_int_(&flag_ctx, name, def_val, desc);
}
///==================================================================================

///=======================================FLOAT=======================================   
static float * flag_new_float_(flag_ctx_t * ctx, const char * _name, float _def, const char * _desc){
    flag_t * f = flag_new_flag_(ctx, FLAG_FLOAT, _name, _desc);
    f->def.as_float = _def;
    if(_def != NULL){
        f->val.as_float = _def;
    }
    return &f->val.as_float;
}

float *FlagFloat  (const char * name, float def_val, const char * desc){
    return flag_new_float_(&flag_ctx, name, def_val, desc);
}
///==================================================================================

///=======================================DOUBLE=======================================   
static double * flag_new_double_(flag_ctx_t * ctx, const char * _name, double _def, const char * _desc){
    flag_t * f = flag_new_flag_(ctx, FLAG_DOUBLE, _name, _desc);
    f->def.as_double = _def;
    if(_def != NULL){
        f->val.as_double = _def;
    }
    return &f->val.as_double;
}

double *FlagDouble  (const char * name, double def_val, const char * desc){
    return flag_new_double_(&flag_ctx, name, def_val, desc);
}
///==================================================================================

///=======================================SIZE=======================================   
static size_t * flag_new_size_(flag_ctx_t * ctx, const char * _name, size_t _def, const char * _desc){
    flag_t * f = flag_new_flag_(ctx, FLAG_SIZE, _name, _desc);
    f->def.as_size = _def;
    if(_def != NULL){
        f->val.as_size = _def;
    }
    return &f->val.as_size;
}

size_t *FlagSize  (const char * name, size_t def_val, const char * desc){
    return flag_new_size_(&flag_ctx, name, def_val, desc);
}
///==================================================================================

///=======================================string=======================================   
static char ** flag_new_string_(flag_ctx_t * ctx, const char * _name, char * _def, const char * _desc){
    flag_t * f = flag_new_flag_(ctx, FLAG_STR, _name, _desc);
    f->def.as_str = _def;
    if(_def != NULL){
        f->val.as_str = _def;
    }
    return &f->val.as_str;
}

char **FlagStr  (const char * name, char * def_val, const char * desc){
    return flag_new_string_(&flag_ctx, name, def_val, desc);
}
///==================================================================================

///=======================================LIST=======================================   
static flag_list_t * flag_new_list_(flag_ctx_t * ctx, const char * _name, const char * _desc){
    flag_t * f = flag_new_flag_(ctx, FLAG_LIST, _name, _desc);
    return &f->val.as_list;
}

flag_list_t *FlagList  (const char * name, const char * desc){
    return flag_new_list_(&flag_ctx, name, desc);
}
///==================================================================================

#endif // PARSER_H_
