#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Err: Missing action"
    echo "./compile.sh [action]

actions:
    debug
    release
    build"
fi

if [ "$1" = "debug" ]; then
    cmake -DCMAKE_BUILD_TYPE=Debug -B build
elif [ "$1" = "release" ]; then
    cmake -DCMAKE_BUILD_TYPE=Release --build build
elif [ "$1" = "build" ]; then
    cmake --build build
fi
