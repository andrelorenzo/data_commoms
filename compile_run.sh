#!/bin/bash

cmake . -B build -DUNIT_TESTING=1
cmake --build build


./build/main