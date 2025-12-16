#!/bin/bash

cd /tests

cmake .. -B ../build -DUNIT_TESTING=1
cmake --build ../build

# clear

ctest --test-dir ../build -V