# Euler: accelerator for DL inference

## Prerequisites:
    ICC > 18.0
    IOMP5
    Linux OS

## Build Instructions:
    ; most simply
    ; (build release version of libel.so and tests with multiple jobs)
    make -j
    ;
    ; build libel.so only
    make lib
    ; build test
    make test
    ; clean the build
    make clean
    ; debug build
    make DEBUG=1
    make DEBUG=1 lib -j
    make DEBUG=1 clean

## Run Tests:
    ; using run script
    ./script/run.sh
    ; or
    build/<release|debug>/bin/el_xxx

## Link to Euler:
    CFLAGS += /path/to/euler/include
    #include "euler.hpp"
    LDFLAGS += libel -L/path/to/euler/<build|release>/lib

## Examples:
    See test/elt_xxx.cpp