#!/bin/bash

CURRDIR=$PWD
cd ..
source b_crcc.sh
source b_gsgl.sh
cd $CURRDIR

SRC=""
SRC+=$(find $APPBASE_PATH -name '*.cc')
SRC+=" "
SRC+=$(find "$BASE_PATH/apps/cycloid" -name '*.cc')


#for file in $SRC
#do
#    out=tmp/$(basename "$file" ".cc")
#    #echo $out
#    g++ -c $COMPILER_OPTIONS -D_GLEW_STATIC "-I$BASE_PATH" "$file" -o "$out.o"
#done


#g++ -o $CYCLOID_OUTPUT $COMPILER_OPTIONS "-I$BASE_PATH" "-I$THIRD_PATH" "-I$THIRD_PATH/stb-2020-06-20" "-I$GLAD_INC" `pkg-config --cflags glfw3` $SRC -L"$BASE_PATH/dist/bin-l" -lgsgl -lcrcc `pkg-config --static --libs glfw3`
g++ -o $CYCLOID_OUTPUT $COMPILER_OPTIONS $DEBUG_OPTIONS "-I$BASE_PATH" "-I$THIRD_PATH" "-I$THIRD_PATH/stb-2020-06-20" "-I$GLAD_INC" `pkg-config --cflags glfw3` $SRC -L"$BASE_PATH/dist/bin-l" -lgsgl -lcrcc `pkg-config --static --libs glfw3`
