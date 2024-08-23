#!/bin/bash

source ./init.sh

rm -f $GSGL_LIB

SRC=$(find $BASE_PATH/gsgl -name '*.cc')
SRC+=" "
SRC+=$GLAD_LIB

for file in $SRC
do
    out=tmp/$(basename "$file" ".cc")
    #echo $out
    g++ -c $COMPILER_OPTIONS $DEBUG_OPTIONS "-I$BASE_PATH" "-I$GLAD_INC" "$file" -o "$out.o"
done

ar $LIB_OPTIONS $GSGL_LIB tmp/*.o

rm -f tmp/*.o

