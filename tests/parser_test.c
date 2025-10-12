#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "parser.h"



#define ARG0 int argc=1;char * argv[] = {(char*)"programm_name_var", NULL};
#define ARG1(a1) int argc=2;char * argv[] = {(char*)"programm_name_var", (char*)(a1), NULL};
#define ARG2(a1, a2) int argc=3;char * argv[] = {(char*)"programm_name_var", (char*)(a1), (char*)(a2), NULL};
#define ARG3(a1, a2, a3) int argc=4;char * argv[] = {(char*)"programm_name_var", (char*)(a1), (char*)(a2), (char*)(a3), NULL};
#define ARG4(a1, a2, a3, a4) int argc=5;char * argv[] = {(char*)"programm_name_var", (char*)(a1), (char*)(a2), (char*)(a3), (char*)(a4), NULL};
#define ARG5(a1, a2, a3, a4, a5) int argc=6;char * argv[] = {(char*)"programm_name_var", (char*)(a1), (char*)(a2), (char*)(a3), (char*)(a4), (char*)(a5), NULL};
#define ARG6(a1, a2, a3, a4, a5, a6) int argc=7;char * argv[] = {(char*)"programm_name_var", (char*)(a1), (char*)(a2), (char*)(a3), (char*)(a4), (char*)(a5), (char*)(a6), NULL};
#define ARG7(a1, a2, a3, a4, a5, a6, a7) int argc=8;char * argv[] = {(char*)"programm_name_var", (char*)(a1), (char*)(a2), (char*)(a3), (char*)(a4), (char*)(a5), (char*)(a6), (char*)(a7), NULL};
#define ARG8(a1, a2, a3, a4, a5, a6, a7, a8) int argc=9;char * argv[] = {(char*)"programm_name_var", (char*)(a1), (char*)(a2), (char*)(a3), (char*)(a4), (char*)(a5), (char*)(a6), (char*)(a7), (char*)(a8), NULL};
#define ARG9(a1, a2, a3, a4, a5, a6, a7, a8, a9) int argc=10;char * argv[] = {(char*)"programm_name_var", (char*)(a1), (char*)(a2), (char*)(a3), (char*)(a4), (char*)(a5), (char*)(a6), (char*)(a7), (char*)(a8), (char*)(a9), NULL};

static void test_no_args(){
    context_reset();
    bool * bool_flag = FlagBool("flag_bool",false, true, "bool flag");
    int * n = FlagInt("flag_int",false, -5, "int flag");

    ARG0
    assert_true(FlagParse(argc, argv));
    assert_true(*bool_flag);
    assert_int_equal(*n, -5);
}

static void test_defaults(){
    context_reset();
    bool * bool_flag = FlagBool("flag_bool",false, false, "bool flag");
    int * n = FlagInt("n",false, 5, "int flag");

    ARG2("-flag_bool","true")
    
    assert_true(FlagParse(argc, argv));
    assert_true(*bool_flag);
    // assert_int_equal(*n, 9);
    FlagPrintHelp(stdout);
    
}
static void test_mandatory(){
    
}
static void test_uint(){
    context_reset();
    uint8_t * uint8f = FlagUint8("uint8f",false, 8, "uint8 flag");
    uint16_t * uint16f = FlagUint16("uint16f",false, 16, "uint8 flag");
    uint32_t * uint32f = FlagUint32("uint32f",false, 32, "uint8 flag");
    uint64_t * uint64f = FlagUint64("uint64f",false, 64, "uint8 flag");
    
    ARG8("-uint8f","16","-uint16f","32", "-uint32f", "64", "-uint64f", "128");

    assert_true(FlagParse(argc, argv));
    assert_uint_equal(*uint8f, 16);
    assert_uint_equal(*uint16f, 32);
    assert_uint_equal(*uint32f, 64);
    assert_uint_equal(*uint64f, 128);

}
static void test_int(){

}
static void test_bool(){

}
static void test_float(){

}
static void test_double(){

}
static void test_list(){

}
static void test_size(){

}
static void test_rest_args_and_programm_name(){

}
static void test_errors_and_help(){

}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_no_args),
        cmocka_unit_test(test_defaults),
        // cmocka_unit_test(test_mandatory),
        cmocka_unit_test(test_uint),
        // cmocka_unit_test(test_int),
        // cmocka_unit_test(test_bool),
        // cmocka_unit_test(test_float),
        // cmocka_unit_test(test_double),
        // cmocka_unit_test(test_list),
        // cmocka_unit_test(test_size),
        // cmocka_unit_test(test_rest_args_and_programm_name),
        // cmocka_unit_test(test_errors_and_help),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
