#!/bin/bash

BASE_PATH="graphoscope"
THIRD_PATH="third"

if [ ! -d "tmp" ]; then
  mkdir tmp
fi

MAJOR=3
MINOR=0

APPBASE_PATH="$BASE_PATH/apps/appbase"
CRCC_LIB="$BASE_PATH/dist/bin-l/libcrcc.a"
GSCC_LIB="$BASE_PATH/dist/bin-l/libgscc.a"
GSGL_LIB="$BASE_PATH/dist/bin-l/libgsgl.a"
GSUI_LIB="$BASE_PATH/dist/bin-l/libgsui.a"

NW_OUTPUT="$BASE_PATH/dist/bin-l/newworld"
SMSP_OUTPUT="$BASE_PATH/dist/bin-l/smallspace"
GRID_OUTPUT="$BASE_PATH/dist/bin-l/grid"
CHARS_OUTPUT="$BASE_PATH/dist/bin-l/chars"
PLANETM_OUTPUT="$BASE_PATH/dist/bin-l/planetmaker"
ORBITS_OUTPUT="$BASE_PATH/dist/bin-l/orbits"
CYCLOID_OUTPUT="$BASE_PATH/dist/bin-l/cycloid"
SYNTH_OUTPUT="$BASE_PATH/dist/bin-l/synth"
PLT_OUTPUT="$BASE_PATH/dist/bin-l/plane_transform"
DEL_OUTPUT="$BASE_PATH/dist/bin-l/delaunay_test"

MDOELT_OUTPUT="$BASE_PATH/dist/bin-l/model-tester"



COMPILER_OPTIONS="-std=c++17 -Wall -Wextra -D REENTRANT -D PLATFORM_LINUX"
#COMPILER_OPTIONS="-std=c++11 -Wall -Wextra -D REENTRANT -D PLATFORM_LINUX -O3"
#DEBUG_OPTIONS="-fsanitize=address -fsanitize=pointer-compare -fsanitize=leak -fsanitize=undefined"
DEBUG_OPTIONS=""
LIB_OPTIONS="rvs"
LINKER_OPTIONS=""


#GLEW_DEF="-D GLEW_STATIC"
#GLEW_INC="pkg-config --cflags glew"
#GLEW_LIB="pkg-config --static --libs glew"
#GLEW_INC1="-I/usr/include/libdrm"
#GLEW_LIB1="-L/usr/lib64 -lGLEW -lGLU -lm -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp"

GLAD_INC="$THIRD_PATH/glad-45/include"
GLAD_LIB="$THIRD_PATH/glad-45/src/glad.c"

SAUDIO_INC="$THIRD_PATH/sokol-master-2021"

GLF_INC="pkg-config --cflags glfw3"
GLF_LIB="pkg-config --static --libs glfw3"
GLF_INC1="-I/usr/local/include"
GLF_LIB1="-L/usr/local/lib -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXxf86vm -lXext -lXcursor -lXrender -lXfixes -lX11 -lpthread -lxcb -lXau -lXdmcp"

