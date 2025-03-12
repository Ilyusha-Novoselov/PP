#!/bin/bash

if [ -z "$1" ]; then
  echo "Error: Please provide the name of the executable file as an argument."
  exit 1
fi

EXECUTABLE_NAME=$1

cd ..

if [ ! -d "build" ]; then
  mkdir build
fi

cd build

if [ ! -f "CMakeCache.txt" ]; then
  cmake ..
fi

cmake --build .

cd ./"$EXECUTABLE_NAME"
./"$EXECUTABLE_NAME"