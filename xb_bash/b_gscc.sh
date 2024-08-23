#!/bin/bash

source ./init.sh

rm -f $GSCC_LIB

SRC=$(find $BASE_PATH/gscc -name '*.cc')
for file in $SRC
do
    out=tmp/$(basename "$file" ".cc")
    #echo $out
    g++ -c $COMPILER_OPTIONS "-I$BASE_PATH" "$file" -o "$out.o"
done

ar $LIB_OPTIONS $GSCC_LIB tmp/*.o

rm -f tmp/*.o

