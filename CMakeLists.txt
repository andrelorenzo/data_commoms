cmake_minimum_required(VERSION 3.22)
project(data_stb_libs)
# SET(CMAKE_C_FLAGS "-Wall -Wextra -pedantic")
# add_compile_options(-Wpedantic)
# add_compile_options(-Wall)
# add_compile_options(-Wextra)

# SET(CMAKE_C_FLAGS "-O3 -xSSE3 -restrict -lpthread -lX11 -ldrm")



include_directories(${CMAKE_SOURCE_DIR}/include/)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmocka)

if (UNIT_TESTING)
     find_package(cmocka 1.1.0 REQUIRED)
     include(AddCMockaTest)
     add_subdirectory(tests)
endif()

add_executable(main src/file_main.c)

add_executable(comms src/comms_main.c)