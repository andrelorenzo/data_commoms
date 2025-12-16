#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define PARSER_IMP
#include "parser.h"

#define WITH_ARGV0(TAG) \
    int TAG##_argc = 1; \
    char * TAG##_argv[] = { (char*)"programm_name_var", NULL }

#define WITH_ARGV(TAG, ...) \
    int TAG##_argc = (int)(sizeof((char*[]){ (char*)"programm_name_var", __VA_ARGS__, NULL })/sizeof(char*) - 1); \
    char * TAG##_argv[] = { (char*)"programm_name_var", __VA_ARGS__, NULL }

extern void context_reset();

// ================== TESTS EXISTENTES ==================
static void test_no_args(void){
    context_reset();
    bool * bool_flag = FlagBool("flag_bool",false, true, "bool flag");
    int * n          = FlagInt ("flag_int" ,false, -5 , "int flag");

    WITH_ARGV0(a);
    assert_true(FlagParse(a_argc, a_argv));
    assert_true(*bool_flag);
    assert_int_equal(*n, -5);
}

static void test_defaults(void){
    context_reset();
    bool * bool_flag = FlagBool("flag_bool",false, false, "bool flag");
    int * n          = FlagInt ("n"        ,false, 5   , "int flag");

    WITH_ARGV(a, "-flag_bool","true");
    assert_true(FlagParse(a_argc, a_argv));
    assert_true(*bool_flag);
    assert_int_equal(*n, 5);
}

static void test_mandatory(void){
    context_reset();
    bool * bool_flag = FlagBool("flag_bool", true , false, "bool flag");
    int  * n         = FlagInt ("n"        , true , 5    , "int flag");

    WITH_ARGV0(a);
    assert_false(FlagParse(a_argc, a_argv));
    assert_false(*bool_flag);
    assert_int_equal(*n, 5);

    context_reset();
    bool * bool_flag2 = FlagBool("flag_bool", false, false, "bool flag");
    int  * n2         = FlagInt ("n"        , false, 5    , "int flag");

    WITH_ARGV0(b);
    assert_true(FlagParse(b_argc, b_argv));
    assert_false(*bool_flag2);
    assert_int_equal(*n2, 5);
}

static void test_uint(void){
    context_reset();
    uint8_t  * uint8f  = FlagUint8 ("uint8f" , false,  8, "uint8 flag");
    uint16_t * uint16f = FlagUint16("uint16f", false, 16, "uint16 flag");
    uint32_t * uint32f = FlagUint32("uint32f", false, 32, "uint32 flag");
    uint64_t * uint64f = FlagUint64("uint64f", false, 64, "uint64 flag");

    WITH_ARGV(a, "-uint8f","16","-uint16f","32","-uint32f","64","-uint64f","128");
    assert_true(FlagParse(a_argc, a_argv));
    assert_int_equal(*uint8f ,  16);
    assert_int_equal(*uint16f,  32);
    assert_int_equal(*uint32f,  64);
    assert_int_equal(*uint64f, 128);
}

static void test_int(void){
    context_reset();
    int * intf = FlagInt("intf", false, 1, "int flag");

    char a0[] = "-intf=5"; // mutable por si tu parser parte con '\0'
    WITH_ARGV(a, a0);
    assert_true(FlagParse(a_argc, a_argv));
    assert_int_equal(*intf, 5);
}

static void test_bool(void){
    context_reset();
    bool * boolf = FlagBool("boolf", false, true, "bool flag");

    char a0[] = "-/boolf=false"; // ignore
    WITH_ARGV(a, a0);
    assert_true(FlagParse(a_argc, a_argv));
    assert_true(*boolf); // ignore mantiene el valor por defecto
}

static void test_float(void){
    context_reset();
    float * floatf = FlagFloat("floatf", false, 4.5f, "float flag");

    WITH_ARGV(a, "-floatf","10.45");
    assert_true(FlagParse(a_argc, a_argv));
    assert_float_equal(*floatf, 10.45f, 0.01f);
}

static void test_double(void){
    context_reset();
    double * df = FlagDouble("doublef", false, -1.0, "double flag");

    char a0[] = "-doublef=3.14159265";
    WITH_ARGV(a, a0);
    assert_true(FlagParse(a_argc, a_argv));
    assert_true(fabs(*df - 3.14159265) < 1e-9);
}

static void test_str(void){
    // con '='
    context_reset();
    char **name = FlagStr("name", false, NULL, "name");
    char a0[] = "-name=pepe";
    WITH_ARGV(a, a0);
    assert_true(FlagParse(a_argc, a_argv));
    assert_non_null(*name);
    assert_string_equal(*name, "pepe");

    // sin '=', valor en siguiente arg
    context_reset();
    name = FlagStr("name", false, NULL, "name");
    WITH_ARGV(b, "-name", "lola");
    assert_true(FlagParse(b_argc, b_argv));
    assert_non_null(*name);
    assert_string_equal(*name, "lola");
}

static void test_list(void){
    context_reset();
    flag_list_t *L = FlagList("I", false, "include paths"); // ajusta firma si no lleva 'false'
    char a1[] = "-I=a";
    char a3[] = "-I=c";
    char a5[] = "-I=d";

    WITH_ARGV(a, a1, "-I", "b", a3, a5, "-I", "e");
    assert_true(FlagParse(a_argc, a_argv));
    assert_non_null(L);
    assert_int_equal(L->count, 5);
    assert_string_equal(L->items[0], "a");
    assert_string_equal(L->items[1], "b");
    assert_string_equal(L->items[2], "c");
    assert_string_equal(L->items[3], "d");
    assert_string_equal(L->items[4], "e");
}

static void test_size(void){
    // 1K con '='
    context_reset();
    size_t *limit = FlagSize("limit", false, 0, "size limit");
    char s1[] = "-limit=1K";
    WITH_ARGV(a, s1);
    assert_true(FlagParse(a_argc, a_argv));
    assert_true(*limit == (size_t)1024);

    // 2M con arg separado
    context_reset();
    limit = FlagSize("limit", false, 0, "size limit");
    WITH_ARGV(b, "-limit", "2M");
    assert_true(FlagParse(b_argc, b_argv));
    assert_true(*limit == (size_t)2 * 1024 * 1024);

    // Sufijo inválido → error
    context_reset();
    limit = FlagSize("limit", false, 0, "size limit");
    WITH_ARGV(c, "-limit", "10X");
    assert_false(FlagParse(c_argc, c_argv));
}

static void test_end_of_flags_and_program_name(void){
    context_reset();
    bool *v = FlagBool("v", false, false, "verbose");

    // -v -- cmd file -x  → resto tras '--'
    WITH_ARGV(a, "-v","true", "--", "cmd", "file.txt", "-x");
    assert_true(FlagParse(a_argc, a_argv));
    assert_true(*v);
    assert_string_equal(FlagProgramName(), "programm_name_var");
    assert_int_equal(FlagRestArgc(), 4);
    char **rest = FlagRestArgv();
    assert_string_equal(rest[0], "--");
    assert_string_equal(rest[1], "cmd");
    assert_string_equal(rest[2], "file.txt");
    assert_string_equal(rest[3], "-x");
}

static void test_flag_name_api(void){
    context_reset();
    int *n = FlagInt("n", false, 7, "int flag");

    char a0[] = "-n=11";
    WITH_ARGV(a, a0);
    assert_true(FlagParse(a_argc, a_argv));
    assert_int_equal(*n, 11);

    const char *nm = FlagName(n);
    assert_non_null(nm);
    assert_string_equal(nm, "n");
}

static void test_errors_and_help(void){
    // 1) Flag desconocida
    context_reset();
    WITH_ARGV(a, "-no_existe");
    assert_false(FlagParse(a_argc, a_argv));
    char *ebuf = NULL; size_t esz = 0;
    FILE *estr = open_memstream(&ebuf, &esz);
    assert_non_null(estr);
    FlagPrintError(estr);
    fclose(estr);
    assert_non_null(ebuf);
    assert_true(esz > 0);
    assert_non_null(strstr(ebuf, "no_existe"));
    free(ebuf);

    // 2) Falta de valor para entero obligatorio
    context_reset();
    (void)FlagInt("n", true, 0, "int mandatory");
    WITH_ARGV(b, "-n");
    assert_false(FlagParse(b_argc, b_argv));
    ebuf = NULL; esz = 0; estr = open_memstream(&ebuf, &esz);
    FlagPrintError(estr); fclose(estr);
    assert_non_null(ebuf); assert_true(esz > 0);
    free(ebuf);

    // 3) Valor inválido
    context_reset();
    (void)FlagInt("intf", false, 0, "int flag");
    WITH_ARGV(c, "-intf", "abc");
    assert_false(FlagParse(c_argc, c_argv));

    // 4) Overflow en uint8
    context_reset();
    (void)FlagUint8("u8", false, 0, "u8");
    WITH_ARGV(d, "-u8", "9999");
    assert_false(FlagParse(d_argc, d_argv));

    // 5) Ayuda imprime algo
    context_reset();
    (void)FlagBool("v", false, false, "verbose");
    (void)FlagStr ("name", false, NULL, "name");
    char *hbuf = NULL; size_t hsz = 0;
    FILE *hstr = open_memstream(&hbuf, &hsz);
    assert_non_null(hstr);
    FlagPrintHelp(hstr);
    fclose(hstr);
    assert_non_null(hbuf);
    assert_true(hsz > 0);
    assert_non_null(strstr(hbuf, "-v"));
    assert_non_null(strstr(hbuf, "name"));
    free(hbuf);
}

static void test_ignore_prefix_does_not_change_value(void){
    context_reset();
    int *n = FlagInt("n", false, 5, "int flag");
    char a0[] = "-/n=42";
    WITH_ARGV(a, a0);
    assert_true(FlagParse(a_argc, a_argv));
    assert_int_equal(*n, 5);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_no_args),
        cmocka_unit_test(test_defaults),
        cmocka_unit_test(test_mandatory),
        cmocka_unit_test(test_uint),
        cmocka_unit_test(test_int),
        cmocka_unit_test(test_bool),
        cmocka_unit_test(test_float),
        cmocka_unit_test(test_double),
        cmocka_unit_test(test_str),
        cmocka_unit_test(test_list),
        cmocka_unit_test(test_size),
        cmocka_unit_test(test_end_of_flags_and_program_name),
        cmocka_unit_test(test_flag_name_api),
        cmocka_unit_test(test_errors_and_help),
        cmocka_unit_test(test_ignore_prefix_does_not_change_value),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
