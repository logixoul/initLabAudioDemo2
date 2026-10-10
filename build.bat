@echo off
set FLAGS=-O2 -std=c++20 -Ilib -Ilib/imgui

rem Library implementations are compiled once. Delete lib_impl.o (or run "build clean") to rebuild them.
if "%1"=="clean" del lib_impl.o 2>nul
if not exist lib_impl.o (
    echo Building lib_impl.o...
    clang++ %FLAGS% -c lib/lib_impl.cpp -o lib_impl.o || exit /b 1
)

clang++ %FLAGS% main.cpp lib_impl.o -lopengl32 -lgdi32 -luser32 -o synth.exe || exit /b 1
