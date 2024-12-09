#!/usr/bin/bash

# Starts the Nubb++ compiler, builds the final executable using g++, and removes the outputted .cpp file by the compiler.

# NOTE: You will need the nubb.exe compiler executable for this to work, as well as g++, which you can install here:
# https://code.visualstudio.com/docs/cpp/config-mingw (skip past installing VSC and the C++ extension, just install MinGW)

echo $(./nubb++3.1.exe $1)
echo "" # newline

# compiler failed for some reason, not in current directory or some other error
if [ $? -ne 0 ]; then
    echo "[FATAL] Nubb++ failed to build executable."
    echo "[WARN] Make sure the compiler binary is in the same directory as this build script; your code file can be elsewhere!"
    echo "[WARN] Given filename to build: $1"
    echo "[WARN] Syntax: bash nubb++build.sh [path-of-file-to-compile]"
else
    echo $(g++ -o nubb.out out.cpp)
fi

# delete original cpp file once compilation is done
rm out.cpp
