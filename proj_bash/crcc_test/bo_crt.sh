#!/bin/bash

source ../init.sh




SRC="$BASE_PATH/apps/crcc_test/crcc_test.cc"

g++ -o "$BASE_PATH/dist/bin-l/crcc_test" $COMPILER_OPTIONS "-I$BASE_PATH" $SRC -L"$BASE_PATH/dist/bin-l" -lcrcc
