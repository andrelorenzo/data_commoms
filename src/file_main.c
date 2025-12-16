#include "stdio.h"
#define PARSER_IMP
#include "file_parser.h"

typedef struct{
    uint16_t a;
    uint16_t b;
    uint16_t c;
}paramstruct_t;


int main(){
    int * paramint = ParamInt("PINT", true, 3,"an int value");
    float * paramfloat = ParamFloat("PFLOAT", true, 2.34,"an float value");

    param_list_t * paramlist = ParamList("PLIST", false, PARAM_INT);
    paramstruct_t * paramstruct = (paramstruct_t*)ParamBin("PSTRUCT", "an struct", 0);

    if(!ParamParse("../params/params.txt", FILE_TYPE_TXT)){
        ParamPrintError(stdout);
        printf("failed to parse txt\n");
        return -1;
    }


    printf("INT: %d\n", *paramint);
    printf("FLOAT: %.2f\n", *paramfloat);

    printf("LIST:");
    for(int i = 0; i < paramlist->count; ++i){
        printf(" %d",paramlist->items[i]);
    }
    printf("\n");

    printf("STRUCT: A-> %d, B-> %d, C-> %d\n", paramstruct->a,paramstruct->b,paramstruct->c);


    if(!InitCSV(".",","))return -1;
    if(!ParamParse("../params/data.csv", FILE_TYPE_CSV))return -1;

    float buff[50];
    int data_size = GetCSVData("velocity",buff, sizeof buff);

    for(int i = 0; i < data_size; ++i){
        printf(" %.2f",buff[i]);
    }
    printf("\n");
}
