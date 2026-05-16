@echo off
setlocal enabledelayedexpansion

if not exist build mkdir build

set PSYCROSS=externals\PsyCross
set SDL2=externals\SDL
set OPENAL=externals\openal-soft

REM TODO(aalhendi): maybe remove this and assume it exists in PATH
if exist "C:\msys64\mingw32\bin\gcc.exe" (
    set "PATH=C:\msys64\mingw32\bin;%PATH%"
)
set CC=gcc
set CXX=g++

REM === Common flags ===
set CFLAGS=-std=c99 -m32 -O2 -g
set CFLAGS=%CFLAGS% -DREBUILD_PC -DCTR_NATIVE -DUSE_EXTENDED_PRIM_POINTERS=0
set CFLAGS=%CFLAGS% -Wno-everything
set CFLAGS=%CFLAGS% -Iinclude -I%PSYCROSS%\include -I%PSYCROSS%
set CFLAGS=%CFLAGS% -I%SDL2%\include -I%OPENAL%\include

set LDFLAGS=-m32 -fno-pie -no-pie -Wl,-Ttext,0x00500000 -static-libgcc -static-libstdc++
set LIBS=-lopengl32 -lgdi32 -lwinmm -luser32 -lkernel32 -lole32 -lshell32 -ldinput8 -ldxguid -lsetupapi

REM === Build SDL2 via CMake (static, cached) ===
if not exist build\sdl_build\libSDL2main.a (
    echo [ctr_native] Building SDL2 via CMake...
    cmake -S %SDL2% -B build\sdl_build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE=Release -DSDL_SHARED=OFF -DSDL_STATIC=ON -DCMAKE_C_FLAGS="-m32" -DCMAKE_EXE_LINKER_FLAGS="-m32 -static-libgcc" 2>nul
    if !ERRORLEVEL! NEQ 0 (
        echo [ctr_native] FAILED: SDL2 cmake configure
        exit /b 1
    )
    mingw32-make -C build\sdl_build -j%NUMBER_OF_PROCESSORS% 2>nul
    if !ERRORLEVEL! NEQ 0 (
        echo [ctr_native] FAILED: SDL2 build
        exit /b 1
    )
    echo [ctr_native] SDL2 built OK
) else (
    echo [ctr_native] SDL2 cached
)

REM === Build OpenAL via CMake (static, cached) ===
if not exist build\oal_build\libopenal.a (
    echo [ctr_native] Building OpenAL via CMake...
    cmake -S %OPENAL% -B build\oal_build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release -DLIBTYPE=STATIC -DALSOFT_EXAMPLES=OFF -DALSOFT_TESTS=OFF -DALSOFT_UTILS=OFF -DCMAKE_C_FLAGS="-m32" -DCMAKE_CXX_FLAGS="-m32" -DCMAKE_EXE_LINKER_FLAGS="-m32 -static-libgcc" 2>nul
    if !ERRORLEVEL! NEQ 0 (
        echo [ctr_native] FAILED: OpenAL cmake configure
        exit /b 1
    )
    mingw32-make -C build\oal_build -j%NUMBER_OF_PROCESSORS% 2>nul
    if !ERRORLEVEL! NEQ 0 (
        echo [ctr_native] FAILED: OpenAL build
        exit /b 1
    )
    echo [ctr_native] OpenAL built OK
) else (
    echo [ctr_native] OpenAL cached
)

REM === Build PsyCross (static, cached) ===
if not exist build\libpsycross.a (
    echo [ctr_native] Building PsyCross...
    del /Q build\psyx_*.o 2>nul

    for /R "%PSYCROSS%" %%f in (*.c) do (
        %CC% -c %CFLAGS% -o "build\psyx_%%~nf.o" "%%f"
        if !ERRORLEVEL! NEQ 0 (
            echo [ctr_native] FAILED: PsyCross %%f
            exit /b 1
        )
    )
    for /R "%PSYCROSS%" %%f in (*.cpp) do (
        %CXX% -c -std=c++11 -m32 -O2 -g -Wno-everything -DUSE_EXTENDED_PRIM_POINTERS=0 -I%PSYCROSS%\include -I%PSYCROSS% -I%SDL2%\include -I%OPENAL%\include -o "build\psyx_%%~nf.o" "%%f"
        if !ERRORLEVEL! NEQ 0 (
            echo [ctr_native] FAILED: PsyCross %%f
            exit /b 1
        )
    )

    ar rcs build\libpsycross.a build\psyx_*.o
    if !ERRORLEVEL! NEQ 0 (
        echo [ctr_native] FAILED: archiving PsyCross
        exit /b 1
    )
    del /Q build\psyx_*.o 2>nul
    echo [ctr_native] PsyCross built OK
) else (
    echo [ctr_native] PsyCross cached
)

REM === Build ctr_native ===
echo [ctr_native] Compiling...
%CC% %CFLAGS% %LDFLAGS% ctr_native.c -o build\ctr_native.exe build\libpsycross.a build\sdl_build\libSDL2main.a build\sdl_build\libSDL2.a build\oal_build\libopenal.a %LIBS%

if %ERRORLEVEL% EQU 0 (
    echo [ctr_native] OK: build\ctr_native.exe
) else (
    echo [ctr_native] FAILED
    exit /b 1
)
