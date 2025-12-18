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
csv_fileh_t fileh;
int main(){
    
    ParamBin((uint8_t*)&paramstruct,sizeof paramstruct,"PSTRUCT", "an struct", 0);

    paramstruct.a = 10;
    paramstruct.b = 37;
    paramstruct.c = 398;

    ParamSave("save_params_txt.txt", FILE_TYPE_TXT);

    if(!ParamParse("save_params_txt.txt", FILE_TYPE_TXT)){
        ParamPrintError(stdout);
        printf("failed to parse txt\n");
        return -1;
    }

    printf("STRUCT: A-> %d, B-> %d, C-> %d\n", paramstruct.a,paramstruct.b,paramstruct.c);


    ParamInt(&paramint,"PINT", true, 3,"an int value");
    ParamFloat(&paramfloat,"PFLOAT", true, 2.34,"an float value");
    ParamFloat(&paramfloat2,"PFLOAT2", true, 1.045,"an float value n2");
    ParamUint(&paramuint,"PUINT", false, 5,"a uint param");
    ParamBool(&parambool,"PBOOL", false, false,"a bool param");


    ParamList(&paramlist,"PLIST", false, PARAM_INT);
    ParamStr((char**)&paramstr,"PSTR",true,"no","a string param");

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
    for(size_t i = 0; i < paramlist.count; ++i){
        printf(" %d",paramlist.items[i].as_int);
    }
    printf("\n");

    printf("STRUCT: A-> %d, B-> %d, C-> %d\n", paramstruct.a,paramstruct.b,paramstruct.c);


    if(!InitCSV(".",",", &fileh))return -1;
    if(!ParamParse("../params/data.csv", FILE_TYPE_CSV))return -1;

    float buff[50];
    int data_size = GetCSVData("size",buff, sizeof buff);

    float newrow[4] = {69,69,69,69};
    if(!AddRowCSV(newrow, ARRAY_LEN(newrow)))return -1;

    paramfloat = 1239.9876543210;

    if(!ParamSave("save_params_txt.txt", FILE_TYPE_TXT))return -1;
    if(!ParamSave("save_params_csv.csv", FILE_TYPE_CSV))return -1;


    printf("size: [");
    for(int i = 0; i < data_size; ++i){
        printf(" %.2f",buff[i]);
    }
    printf(" ]\n");
}
