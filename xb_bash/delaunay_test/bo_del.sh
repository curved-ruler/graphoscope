#!/bin/bash

source ../init.sh





SRC=""
SRC+=$(find $APPBASE_PATH -name '*.cc')
SRC+=" "
SRC+=$(find "$BASE_PATH/apps/delaunay_test" -name '*.cc')


#for file in $SRC
#do
#    out=tmp/$(basename "$file" ".cc")
#    #echo $out
#    g++ -c $COMPILER_OPTIONS -D_GLEW_STATIC "-I$BASE_PATH" "$file" -o "$out.o"
#done

#g++ $LINKER_OPTIONS tmp/*.o `$GLEW_LIB` `$GLF_LIB` -L"$BASE_PATH/crcc/bin/lin" -l:crcc.a -L"$BASE_PATH/render/opengl/bin/lin" -l:gsgl.a -o "$DEST/"newworld-$MAJOR-$MINOR
#g++ -o "$DEST/"newworld-$MAJOR-$MINOR tmp/*.o `pkg-config --static --libs glew glfw3` -L"$BASE_PATH/crcc/bin/lin" -L"$BASE_PATH/render/opengl/bin/lin" -Wl,-Bstatic -l:crcc.a -l:gsgl.a
g++ -o $DEL_OUTPUT $COMPILER_OPTIONS $DEBUG_OPTIONS "-I$BASE_PATH" "-I$THIRD_PATH" "-I$THIRD_PATH/stb-2020-06-20" "-I$GLAD_INC" `pkg-config --cflags glfw3` $SRC -L"$BASE_PATH/dist/bin-l" -lgsgl -lcrcc `pkg-config --static --libs glfw3`

