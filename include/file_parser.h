#ifndef FILE_PARSER_H_
#define FILE_PARSER_H_
#include "stdbool.h"
#include "stdint.h"
#include "stddef.h"
#include "assert.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "limits.h"
#include "errno.h"
#include <ctype.h>

#ifndef PARAM_ASSERT
#define PARAM_ASSERT(b) assert(b)    
#endif // PARAM_ASSERT

#ifndef UNUSED_VAR
#define UNUSED_VAR(a) (void)(a)
#endif // UNUSED_VAR

#ifndef UNUSED_FN
#define UNUSED_FN (void)
#endif // UNUSED_FN

#ifndef ARRAY_LEN
#define ARRAY_LEN(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif // ARRAY_LEN

#ifndef FLAGS_CAP
#define FLAGS_CAP 256
#endif // FLAGS_CAP

#ifndef PARAM_LIST_INIT_CAP
#define PARAM_LIST_INIT_CAP 1024
#endif // PARAM_LIST_INIT

typedef enum {
    PARAM_BOOL = 0,    
    PARAM_UINT,
    PARAM_INT,
    PARAM_FLOAT,
    PARAM_STR,
    PARAM_LIST,
    PARAM_BINARY,
    COUNT_PARAM_TYPES,
} param_type_e;

typedef union {
    bool        as_bool;
    uint32_t    as_uint;
    int         as_int;
    float       as_float;
    char       *as_str;
} param_simple_val_u;

typedef struct {
    param_simple_val_u items[PARAM_LIST_INIT_CAP];
    size_t count;
    param_type_e type;
} param_list_t;


typedef enum{
    FILE_TYPE_TXT = 0U,
    FILE_TYPE_CSV
}file_type_e;


/* ARGUMENT BASED PARSER*/

bool        *ParamBool  (const char * name,bool is_mandatory, bool def_val     , const char * desc);
uint32_t    *ParamUint(const char * name,bool is_mandatory, uint32_t  def_val, const char * desc);
int         *ParamInt   (const char * name,bool is_mandatory, int       def_val, const char * desc);
float       *ParamFloat (const char * name,bool is_mandatory, float     def_val, const char * desc);
char        **ParamStr  (const char * name,bool is_mandatory, char *    def_val, const char * desc);
param_list_t *ParamList  (const char * name, const char * desc, param_type_e type); 
uint8_t      *ParamBin  (const char * name, const char * desc, uint8_t def_val); 
bool        InitCSV  (const char * dec_sep, const char * col_sep); 
int         GetCSVData(const char * name, float * data, size_t data_len);


bool        ParamParse(const char * filename, file_type_e type);
void        ParamPrintError(FILE * stream);
char *      ParamName(void *val);

#ifdef PARSER_IMP

#define MAX_BIN 2048

typedef union {
    param_simple_val_u simple_val;
    param_list_t list;
    uint8_t bin[MAX_BIN];
} param_val_u;

typedef enum {
    PARAM_NO_ERROR = 0,
    PARAM_ERROR_UNKNOWN,
    PARAM_ERROR_NO_VALUE,
    PARAM_ERROR_INVALID_NUMBER,
    PARAM_ERROR_INVALID_FILE_EXT,
    PARAM_ERROR_INVALID_SIZE_SUFFIX,
    PARAM_ERROR_BIN_OVERFLOW,
    COUNT_PARAM_ERRORS,
} param_error_e;

typedef struct {
    param_type_e type;
    const char *name;
    const char *desc;
    param_val_u val;
    param_val_u def;
    bool is_mandatory;
    bool has_changed;
} param_t;

typedef struct{
    char column_name[256];
    float column_item[MAX_BIN];
    size_t item_count;
}csv_data_t;

typedef struct {
    // txt params
    param_t params[FLAGS_CAP];
    size_t params_count;

    // csv data 
    csv_data_t csv_data[MAX_BIN];
    size_t col_count;
    char *column_sep;
    char *decimal_sep;

    param_error_e param_error;
} param_ctx_t;


static param_ctx_t param_ctx;


void context_reset(){
    param_ctx.params_count = 0;
    
    for(size_t i = 0; i < FLAGS_CAP; i++){
        param_ctx.params[i].has_changed = false;
        param_ctx.params[i].is_mandatory = false;
    }
}


static param_t * param_new_param_(param_ctx_t * ctx, param_type_e _type, const char * _name, const char * _desc, bool _is_mandatory){
    PARAM_ASSERT(ctx->params_count < FLAGS_CAP);
    param_t * f =  &ctx->params[ctx->params_count++];
    memset(f, 0, sizeof(*f));

    f->type = _type;
    f->name = _name;
    f->desc = _desc;
    f->is_mandatory = _is_mandatory;

    return f;
}

///=======================================BOOL=======================================   
static bool * param_new_bool_(param_ctx_t * ctx, const char * _name, bool _def, bool is_mandatory, const char * _desc){
    param_t * f = param_new_param_(ctx, PARAM_BOOL, _name, _desc, is_mandatory);
    
    f->def.simple_val.as_bool = _def;
    f->val.simple_val.as_bool = _def;    
    return &f->val.simple_val.as_bool;
}

bool *ParamBool  (const char * name,bool is_mandatory, bool def_val, const char * desc){
    return param_new_bool_(&param_ctx, name, def_val, is_mandatory, desc);
}
///==================================================================================

///=======================================UINT32=======================================   
static uint32_t * param_new_uint32_(param_ctx_t * ctx, const char * _name, uint32_t _def, bool is_mandatory, const char * _desc){
    param_t * f = param_new_param_(ctx, PARAM_UINT, _name, _desc, is_mandatory);
    
    f->def.simple_val.as_uint = _def;
    f->val.simple_val.as_uint = _def;    
    return &f->val.simple_val.as_uint;
}

uint32_t *ParamUint  (const char * name,bool is_mandatory, uint32_t def_val, const char * desc){
    return param_new_uint32_(&param_ctx, name, def_val, is_mandatory, desc);
}
///==================================================================================

///=======================================INT=======================================   
static int * param_new_int_(param_ctx_t * ctx, const char * _name, int _def, bool is_mandatory, const char * _desc){
    param_t * f = param_new_param_(ctx, PARAM_INT, _name, _desc, is_mandatory);
    
    f->def.simple_val.as_int = _def;
    f->val.simple_val.as_int = _def;
    return &f->val.simple_val.as_int;
}

int *ParamInt  (const char * name,bool is_mandatory, int def_val, const char * desc){
    return param_new_int_(&param_ctx, name, def_val, is_mandatory, desc);
}
///==================================================================================

///=======================================FLOAT=======================================   
static float * param_new_float_(param_ctx_t * ctx, const char * _name, float _def, bool is_mandatory, const char * _desc){
    param_t * f = param_new_param_(ctx, PARAM_FLOAT, _name, _desc, is_mandatory);
    
    f->def.simple_val.as_float = _def;
    f->val.simple_val.as_float = _def;    
    return &f->val.simple_val.as_float;
}

float *ParamFloat  (const char * name,bool is_mandatory, float def_val, const char * desc){
    return param_new_float_(&param_ctx, name, def_val, is_mandatory, desc);
}
///==================================================================================

///=======================================string=======================================   
static char ** param_new_string_(param_ctx_t * ctx, const char * _name, char * _def, bool is_mandatory , const char * _desc){
    param_t * f = param_new_param_(ctx, PARAM_STR, _name, _desc, is_mandatory);
    
    f->def.simple_val.as_str = _def;
    f->val.simple_val.as_str = _def;
    return &f->val.simple_val.as_str;
}

char **ParamStr  (const char * name,bool is_mandatory, char * def_val, const char * desc){
    return param_new_string_(&param_ctx, name, def_val, is_mandatory, desc);
}
///==================================================================================

///=======================================LIST=======================================   
static param_list_t * param_new_list_(param_ctx_t * ctx, const char * _name, const char * _desc, param_type_e type ){
    param_t * f = param_new_param_(ctx, PARAM_LIST, _name, _desc, true);
    f->val.list.type = type;
    return &f->val.list;
}

param_list_t *ParamList  (const char * name, const char * desc, param_type_e type){
    return param_new_list_(&param_ctx, name, desc, type);
}
///==================================================================================

///=======================================BIN=======================================   
static uint8_t * param_new_bin_(param_ctx_t * ctx, const char * _name, const char * _desc, uint8_t def ){
    param_t * f = param_new_param_(ctx, PARAM_BINARY, _name, _desc, true);
    memset(f->val.bin, def, sizeof f->val.bin);
    return &f->val.bin;
}
uint8_t     *ParamBin  (const char * name, const char * desc, uint8_t def_val){
    return param_new_bin_(&param_ctx, name, desc, def_val);
}
///==================================================================================



static void *param_get_ref(param_t *param){
    return &param->val;
}

char *param_name(param_ctx_t * ctx, void *val){

    for(size_t i = 0; i < ctx->params_count; i++){
        param_t * f = &ctx->params[i];
        if(param_get_ref(f) == val){
            return (char*)f->name;
        }
    }
    return NULL;
}

char * ParamName(void *val){
    return param_name(&param_ctx, val);
}

bool ParseListParam(param_t * p, char * val_start_original, char * val_end_original){
    
    // Validar entradas básicas
    if (!p || !val_start_original || !val_end_original || *val_start_original != '[' || *val_end_original != ']') {
        return false;
    }

    // Calcular la longitud y crear una COPIA MODIFICABLE de la subcadena relevante.
    // Esto es crucial para evitar segfaults en memoria de solo lectura.
    size_t len = val_end_original - val_start_original - 1;
    char * input_copy = malloc(len + 1);
    if (!input_copy) {
        return false; // Error de asignación de memoria
    }
    // Copiamos el contenido sin los corchetes
    strncpy(input_copy, val_start_original + 1, len);
    input_copy[len] = '\0';

    char * current_element_start = input_copy;
    char * next_element_start = input_copy;
    size_t idx = 0;

    // Iteramos usando strtok_r (thread-safe, recomendado) para dividir por ',' o ' '
    char *saveptr;
    char *token = strtok_r(current_element_start, ", ", &saveptr);

    while(token != NULL) {
        // Ignorar tokens vacíos que puedan surgir de espacios extra o comas
        if (strlen(token) == 0) {
            token = strtok_r(NULL, ", ", &saveptr);
            continue;
        }

        // CRUCIAL: Verificar que el puntero de destino sea válido y que no excedamos límites.
        if (idx >= 32) {
            // Manejar error: lista llena o memoria no asignada previamente.
            free(input_copy);
            return false; 
        }

        switch (p->val.list.type) {
            case PARAM_BOOL:{
                if( strcmp(token, "1") == 0 ||  strcmp(token, "true") == 0  || strcmp(token, "yes") == 0 || strcmp(token, "si") == 0) {
                    p->val.list.items[idx].as_bool = true;
                } else if (strcmp(token, "0") == 0 ||  strcmp(token, "false") == 0  || strcmp(token, "no") == 0){
                    p->val.list.items[idx].as_bool = false;
                } else {
                    free(input_copy);
                    return false; // Valor booleano no válido
                }
            } break;
            
            case PARAM_UINT:{
                char * end;
                unsigned long int temp = strtoul(token, &end, 10);
                // Validar que toda la cadena fue consumida por strtoul
                if(*end != '\0' || temp > UINT_MAX){ // Comprobar también overflow si es necesario
                    free(input_copy);
                    return false;
                }
                p->val.list.items[idx].as_uint = (unsigned int)temp;
            } break;
            
            case PARAM_INT:{    
                char * end;
                long int temp = strtol(token, &end, 10);
                // Usar strtol y validar, es más seguro que atoi
                if(*end != '\0' || temp > INT_MAX || temp < INT_MIN){
                     free(input_copy);
                    return false;
                }
                p->val.list.items[idx].as_int = (int)temp;
            } break;
            
            case PARAM_FLOAT:{
                char * end;
                float temp = strtof(token, &end);
                 if(*end != '\0'){
                     free(input_copy);
                    return false;
                }
                p->val.list.items[idx].as_float = temp;
            } break;
            
            case PARAM_STR:{
                // CRUCIAL: Asumimos que p->val.list.items[idx]->as_str 
                // tiene suficiente espacio asignado (e.g., 256 bytes)
                strncpy(p->val.list.items[idx].as_str, token, sizeof(p->val.list.items[idx].as_str) - 1);
                p->val.list.items[idx].as_str[sizeof(p->val.list.items[idx].as_str) - 1] = '\0'; // Asegurar terminador nulo
            } break;
        
            default:
                free(input_copy);
                return false;
        }

        idx++;
        p->val.list.count = idx; // Mantener el contador actualizado
        token = strtok_r(NULL, ", ", &saveptr); // Obtener el siguiente token
    }

    free(input_copy); // Liberar la copia de la cadena
    p->has_changed = true;
    return true;
}


bool param_parse_txt(param_ctx_t * c, const char * filename){

    if(!strstr(filename, ".txt")){
        printf("file with erroneou sufix\n");
        return false;
    }

    // context_reset();

    FILE * fileh = fopen(filename, "r");
    if(!fileh){
        printf("file not found\n");
        return false;
    }

    char line[1024];

    bool mandatory_failed = false;
    bool found = false;
    while (fgets(line, sizeof line, fileh)) {
        

        if(*line == '#' || *line == ' ' || *line == '\n')continue;

        for (size_t i = 0; i < c->params_count; ++i) {
            char * name_start = strstr(line, c->params[i].name);
            if(name_start){
                char name[256];
                strcpy(name, name_start);
                char * name_end = strstr(name, "=");
                if(!name_end)continue;
                *name_end = '\0';
                int j = 0;
                while(name[j++] != '\0'){
                    if(name[j] == ' '){
                        name[j] = '\0';
                        break;
                    }
                }
                if(strcmp(name, c->params[i].name) != 0)continue;

                char * val_start = strstr(name_start, "=");
                
                val_start++;
                if(!val_start)break;
                found = true;

                bool value_found = true;
                while(*val_start == ' '){ // evitate spaces and check for missing value
                    if(*val_start == '\n'){
                        value_found = false;
                    }
                    val_start++;
                }
                if(!value_found)break; // continue in while(line)

                char * val_end = NULL;
                if(c->params[i].type == PARAM_LIST){
                    val_end = strstr(val_start,"]");
                    val_end++;
                }else{
                    val_end = strstr(val_start," ");
                    if(!val_end){
                        val_end = strstr(val_start,"\n");
                    }
                }
                if(!val_end)break;
                *val_end = '\0'; // Cut out any comment or symbol that is after

                

                switch (c->params[i].type){
                    case PARAM_BOOL:{
                        if( strcmp(val_start, "1") == 0 ||  strcmp(val_start, "true") == 0  || strcmp(val_start, "yes") == 0 || strcmp(val_start, "si") == 0) {
                            c->params[i].val.simple_val.as_bool = true;
                        }else if (strcmp(val_start, "0") == 0 ||  strcmp(val_start, "false") == 0  || strcmp(val_start, "no") == 0){
                            c->params[i].val.simple_val.as_bool = false;
                        }else{ 
                            c->param_error = PARAM_ERROR_UNKNOWN;
                            return false;
                        }
                        c->params[i].has_changed = true;
                    }break;
                    case PARAM_INT:{
                        c->params[i].val.simple_val.as_int = atoi(val_start);
                        c->params[i].has_changed = true;
                    }break;
                    case PARAM_UINT:{
                        char * end;
                        unsigned long int temp = strtoul(val_start,&end,10);
                        if(*end != '\0'){
                            c->param_error = PARAM_ERROR_INVALID_NUMBER;
                            return false;
                        }
                        c->params[i].val.simple_val.as_uint = temp;
                        c->params[i].has_changed = true;
                    }break;
                    case PARAM_FLOAT:{
                        char * end;
                        float temp = strtof(val_start,&end);
                        if(*end != '\0'){
                            c->param_error = PARAM_ERROR_INVALID_NUMBER;
                            return false;
                        }
                        c->params[i].val.simple_val.as_float = temp;
                        c->params[i].has_changed = true;
                    }break;
                    case PARAM_STR:{
                        strcpy(c->params[i].val.simple_val.as_str, val_start);
                        c->params[i].has_changed = true;
                    }break;
                    case PARAM_LIST:{
                    
                        val_end--;
                        ParseListParam(&c->params[i], val_start, val_end);
                    }break;
                    case PARAM_BINARY:{
                        if(strstr(val_start,"0x")){
                            val_start += 2;
                        }
                        if((val_end - val_start)/2 > MAX_BIN){
                            c->param_error == PARAM_ERROR_BIN_OVERFLOW;
                            return false;
                        }
                        size_t idx = 0;
                        while(val_start != val_end){ // FFAABBCCDDFFEEBBCCDD
                            char temp[3];
                            char * end;
                            strncpy(temp,val_start,2);
                            c->params[i].val.bin[idx] = (uint8_t)strtoul(temp,&end,16);
                            val_start+=2;
                            idx++;
                        }

                        c->params[i].has_changed = true;
                    }break;
                    
                    default:{
                        printf("unrecheable\n");
                        return false;
                    }
                }
            }
        }
    } 

    if (!found) {
        printf("not found\n");
        c->param_error = PARAM_ERROR_UNKNOWN;
        return false;
    }
    for (size_t i = 0; i < c->params_count; ++i) {
        if(c->params[i].is_mandatory && !c->params[i].has_changed){
            c->param_error = PARAM_ERROR_NO_VALUE;
            ParamPrintError(stdout);
            mandatory_failed = true;
        }
    }
    if(mandatory_failed){
        return false;
    }
    return true;
}

bool InitCSV  (const char * dec_sep, const char * col_sep){
    if(*dec_sep == *col_sep)return false;

    param_ctx.csv_data->item_count = 0;
    param_ctx.col_count = 0;

    param_ctx.decimal_sep = (char*)dec_sep;
    param_ctx.column_sep = (char*)col_sep;
    return true;
}

int GetCSVData(const char * name, float * data, size_t data_len){
    char low_name[256];
    strcpy(low_name, name);

    for (int i = 0; low_name[i] != '\0'; i++) {
        low_name[i] = (char)tolower((unsigned char)low_name[i]);
    }
    if(data_len < param_ctx.csv_data->item_count)return -1;
    int i;
    bool found = false;
    for(i = 0; i < param_ctx.col_count; ++i){
        if(strstr(param_ctx.csv_data[i].column_name, low_name)){
            found = true;
            break;
        }
    }
    if(!found)return false;

    for(int j = 0; j < param_ctx.csv_data[i].item_count; ++j){
        data[j] = param_ctx.csv_data[i].column_item[j];
    }

    return param_ctx.csv_data[i].item_count;
}

void csv_new_headers(char * line, size_t size, param_ctx_t * c){
    char * token;
    char * saveptr;
    size_t idx = 0;

    if (size > 0 && line[size - 1] == '\n') {
        line[size - 1] = '\0';
    } else if (size > 0 && line[size - 1] == '\r') {
        line[size - 1] = '\0';
        if (size > 1 && line[size - 2] == '\n') {
             line[size - 2] = '\0';
        }
    }
    token = strtok_r(line, c->column_sep, &saveptr);

    while (token != NULL) {
        for (int i = 0; token[i] != '\0'; i++) {
            token[i] = (char)tolower((unsigned char)token[i]);
        }
        if (idx < MAX_BIN) {
            strncpy(c->csv_data[idx].column_name, token, sizeof(c->csv_data[idx].column_name) - 1);
            c->csv_data[idx].column_name[sizeof(c->csv_data[idx].column_name) - 1] = '\0';
            
            char * space = strstr(c->csv_data[idx].column_name, " ");
            if(space){
                *space = '\0';
            }
            idx++;
        } else {
            fprintf(stderr, "Error: Demasiadas columnas en el CSV, superado el límite.\n");
            break;
        }
        token = strtok_r(NULL, c->column_sep, &saveptr);
    }

    c->col_count = idx;
}

bool param_parse_csv(param_ctx_t * c, const char * filename){
    if(!strstr(filename, ".csv"))return false;

    FILE * fileh = fopen(filename, "r");
    if(!fileh)return false;

    char line[2048];
    char * pt = line;

    const char * ds = c->decimal_sep;
    const char * cs = c->column_sep; 

    fgets(line, sizeof line, fileh);
    csv_new_headers(line, sizeof line, c);

    size_t cols = 0;
    while (fgets(line, sizeof line, fileh)) {
        // --- PRE-PROCESAMIENTO DE LA LÍNEA ---
        // Eliminar saltos de línea (CRLF o LF) para que strtok_r no los trate como parte del último token
        size_t len = strlen(line);
        if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            if (len > 1 && line[len - 2] == '\r') {
                line[len - 2] = '\0';
            }
        }
        
        char *token;
        char *saveptr;
        size_t cols = 0;
        size_t row_index = 0;
        token = strtok_r(line, c->column_sep, &saveptr);

        while (token != NULL) {

            if (cols >= MAX_BIN) {
                fprintf(stderr, "Error: Demasiadas columnas en la fila.\n");
                break;
            }

            size_t current_item_idx = c->csv_data[cols].item_count;
            if (current_item_idx >= MAX_BIN) {
                 fprintf(stderr, "Error: Demasiados items en la columna %zu.\n", cols);
                 c->param_error = PARAM_ERROR_INVALID_NUMBER;
                 return false;
            }

            if (strlen(token) == 0) {
                c->csv_data[cols].column_item[current_item_idx] = 0.0f;
            } else {
                if (*c->decimal_sep != '.') {
                    char *dec_sep_pos = strstr(token, c->decimal_sep);
                    if (dec_sep_pos != NULL) {
                        *dec_sep_pos = '.';
                    }
                }

                char *end;
                float temp = strtof(token, &end);

                if (*end != '\0') {
                    c->param_error = PARAM_ERROR_INVALID_NUMBER;
                    return false;
                }
                
                c->csv_data[cols].column_item[current_item_idx] = temp;
            }

            c->csv_data[cols].item_count++;
            cols++;
            token = strtok_r(NULL, c->column_sep, &saveptr);
        }
        
        row_index++;
    }

    c->param_error = PARAM_NO_ERROR;
    return true;
}

bool ParamParse(const char * filename, file_type_e type){
    switch (type){
        case FILE_TYPE_TXT:return param_parse_txt(&param_ctx, filename);
        case FILE_TYPE_CSV:return param_parse_csv(&param_ctx, filename);
        default:return false;
    }
}

void ParamPrintError(FILE * stream){
    param_ctx_t * fc = &param_ctx;
    switch(fc->param_error){
        case PARAM_NO_ERROR : {
            fprintf(stream,"Task failed succesfully, No error\n");
        break;
        }
        case PARAM_ERROR_UNKNOWN : {
            fprintf(stream,"ERROR: unknown param\n");
        break;
        }
        case PARAM_ERROR_NO_VALUE : {
            fprintf(stream,"ERROR: no value provided\n");
        break;
        }
        case PARAM_ERROR_INVALID_NUMBER : {
            fprintf(stream,"ERROR: invalid number\n");
        break;
        }
        case PARAM_ERROR_INVALID_FILE_EXT : {
            fprintf(stream,"ERROR: invalid file extension\n");
        break;
        }
        case PARAM_ERROR_INVALID_SIZE_SUFFIX : {
            fprintf(stream, "ERROR: invalid size suffix\n");
        break;
        }
        case PARAM_ERROR_BIN_OVERFLOW : {
            fprintf(stream, "ERROR: bin overflow\n");
        break;
        }
        default:
            assert(0 && "unreachable");
            exit(-1);
        break;
    } 
}
#endif // PARSER_IMP

#endif // FILE_PARSER_H_