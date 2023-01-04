#!/bin/bash

source ./init.sh

rm -f $GSUI_LIB

SRC=$(find $BASE_PATH/gsui -name '*.cc')

for file in $SRC
do
    out=tmp/$(basename "$file" ".cc")
    #echo $out
    g++ -c $COMPILER_OPTIONS $DEBUG_OPTIONS "-I$BASE_PATH" -I/usr/local/include -I/usr/include/libdrm "$file" -o "$out.o"
done

ar $LIB_OPTIONS $GSUI_LIB tmp/*.o

rm -f tmp/*.o
