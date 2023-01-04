#!/bin/bash

source ./init.sh

rm -f $CRCC_LIB

SRC=$(find $BASE_PATH/crcc -name '*.cc')



for file in $SRC
do
    out=tmp/$(basename "$file" ".cc")
    #echo $out
    g++ -c $COMPILER_OPTIONS $DEBUG_OPTIONS "$file" -o "$out.o"
done

ar $LIB_OPTIONS $CRCC_LIB tmp/*.o

rm -f tmp/*.o

