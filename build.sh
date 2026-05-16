#!/bin/bash
set -e

mkdir -p build

PSYCROSS=externals/PsyCross
SDL2=externals/SDL
OPENAL=externals/openal-soft

if command -v clang &>/dev/null; then
    CC=clang
    CXX=clang++
else
    CC=gcc
    CXX=g++
fi

CFLAGS="-std=c99 -m32 -O2 -g"
CFLAGS="$CFLAGS -DREBUILD_PC -DCTR_NATIVE -DUSE_EXTENDED_PRIM_POINTERS=0"
CFLAGS="$CFLAGS -Wno-everything"
CFLAGS="$CFLAGS -Iinclude -I$PSYCROSS/include -I$PSYCROSS"
CFLAGS="$CFLAGS -I$SDL2/include -I$OPENAL/include"

LDFLAGS="-m32 -fno-pie -no-pie -Wl,-Ttext,0x00500000 -static-libgcc -static-libstdc++"

if [ "$(uname)" = "Darwin" ]; then
    LIBS="-framework OpenGL -framework CoreAudio -framework AudioUnit -framework CoreFoundation -framework IOKit -framework Carbon -framework AppKit -framework ForceFeedback -framework CoreVideo"
else
    LIBS="-lGL -lpthread -ldl -lm -lrt -lX11 -lXext -lasound -ludev -ldbus-1"
fi

# Build SDL2 (static, cached)
if [ ! -f build/libSDL2.a ]; then
    echo "[ctr_native] Building SDL2..."
    rm -f build/sdl_*.o

    for f in $(find $SDL2/src -name '*.c'); do
        $CC -c $CFLAGS -DSDL_STATIC -o "build/sdl_$(basename ${f%.c}).o" "$f"
    done

    ar rcs build/libSDL2.a build/sdl_*.o
    rm -f build/sdl_*.o
    echo "[ctr_native] SDL2 built OK"
else
    echo "[ctr_native] SDL2 cached"
fi

# Build OpenAL (static, cached)
if [ ! -f build/libopenal.a ]; then
    echo "[ctr_native] Building OpenAL..."
    rm -f build/oal_*.o

    for f in $(find $OPENAL/core $OPENAL/alc $OPENAL/common -name '*.c' 2>/dev/null); do
        $CC -c $CFLAGS -DHAVE_DYNLOAD -DAL_BUILD_LIBRARY -DAL_ALEXT_PROTOTYPES -o "build/oal_$(basename ${f%.c}).o" "$f"
    done

    ar rcs build/libopenal.a build/oal_*.o
    rm -f build/oal_*.o
    echo "[ctr_native] OpenAL built OK"
else
    echo "[ctr_native] OpenAL cached"
fi

# Build PsyCross (static, cached)
if [ ! -f build/libpsycross.a ]; then
    echo "[ctr_native] Building PsyCross..."
    rm -f build/psyx_*.o

    for f in $(find $PSYCROSS -name '*.c' -o -name '*.C'); do
        $CC -c $CFLAGS -o "build/psyx_$(basename ${f%.*}).o" "$f"
    done

    for f in $(find $PSYCROSS -name '*.cpp'); do
        $CXX -c -std=c++11 -m32 -O2 -g -Wno-everything \
            -DUSE_EXTENDED_PRIM_POINTERS=0 \
            -I$PSYCROSS/include -I$PSYCROSS \
            -I$SDL2/include -I$OPENAL/include \
            -o "build/psyx_$(basename ${f%.cpp}).o" "$f"
    done

    ar rcs build/libpsycross.a build/psyx_*.o
    rm -f build/psyx_*.o
    echo "[ctr_native] PsyCross built OK"
else
    echo "[ctr_native] PsyCross cached"
fi

# Build ctr_native
echo "[ctr_native] Compiling..."
$CC $CFLAGS $LDFLAGS ctr_native.c -o build/ctr_native \
    build/libpsycross.a build/libSDL2.a build/libopenal.a $LIBS

echo "[ctr_native] OK: build/ctr_native"
