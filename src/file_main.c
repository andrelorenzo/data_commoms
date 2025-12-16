#include "stdio.h"
#define PARSER_IMP
#include "file_parser.h"

typedef struct{
    uint16_t a;
    uint16_t b;
    uint16_t c;
}paramstruct_t;

int paramint;
float paramfloat;
float paramfloat2;
param_list_t paramlist;
paramstruct_t paramstruct;
char paramstr[265];
uint32_t paramuint;
bool parambool;
int main(){


    ParamInt(&paramint,"PINT", true, 3,"an int value");
    ParamFloat(&paramfloat,"PFLOAT", true, 2.34,"an float value");
    ParamFloat(&paramfloat2,"PFLOAT2", true, 1.045,"an float value n2");
    ParamUint(&paramuint,"PUINT", false, 5,"a uint param");
    ParamBool(&parambool,"PBOOL", false, false,"a bool param");

    ParamList(&paramlist,"PLIST", false, PARAM_INT);
    ParamBin(&paramstruct,sizeof paramstruct,"PSTRUCT", "an struct", 0);
    ParamStr(&paramstr,"PSTR",true,"no","a string param");

    if(!ParamParse("../params/params.txt", FILE_TYPE_TXT)){
        ParamPrintError(stdout);
        printf("failed to parse txt\n");
        return -1;
    }


    printf("INT: %d\n", paramint);
    printf("UINT: %d\n", paramuint);
    printf("FLOAT: %.2f\n", paramfloat);
    printf("FLOAT2: %.2f\n", paramfloat2);
    printf("BOOL: %s\n", parambool ? "TRUE" : "FALSE");
    printf("STR: %s\n", paramstr);

    printf("LIST:");
    for(int i = 0; i < paramlist.count; ++i){
        printf(" %d",paramlist.items[i]);
    }
    printf("\n");

    printf("STRUCT: A-> %d, B-> %d, C-> %d\n", paramstruct.a,paramstruct.b,paramstruct.c);


    if(!InitCSV(".",","))return -1;
    if(!ParamParse("../params/data.csv", FILE_TYPE_CSV))return -1;

    float buff[50];
    int data_size = GetCSVData("velocity",buff, sizeof buff);

    printf("velocity: [");
    for(int i = 0; i < data_size; ++i){
        printf(" %.2f",buff[i]);
    }
    printf(" ]\n");
}
