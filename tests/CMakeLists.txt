cmake_minimum_required(VERSION 4.0)

# if  you have a compiled lib to test
# list(APPEND TEST_LIBS my_lib)

list(APPEND TEST_DIRS "${data_stb_libs_SOURCE_DIR}/include")

list(APPEND TEST_TARGETS parser_test)

foreach(TEST_TARGET IN LISTS TEST_TARGETS)
    add_cmocka_test(
        ${TEST_TARGET}
        SOURCES "${TEST_TARGET}.c"
        COMPILE_OPTIONS ${DEFAULT_C_COMPILE_FLAGS}
        LINK_LIBRARIES "${CMOCKA_LIBRARIES}") # add ${TEST_LIBS} for own libs
    add_cmocka_test_environment(${TEST_TARGET})
    target_include_directories(${TEST_TARGET} PUBLIC "${CMOCKA_INCLUDE_DIRS} ${TEST_DIRS}")
endforeach()
