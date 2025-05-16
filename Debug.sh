#!/bin/sh

echo "Building Debug-Binary"
mkdir -p Build-Debug

cmake -B Build-Debug -D CMAKE_BUILD_TYPE=Debug -D CMAKE_CXX_FLAGS_RELEASE="$CXXFLAGS" || exit 1
cmake --build Build-Debug -j "$(nproc)" || exit 1

cs2_pid=$(pidof cs2)
if [ -z "$cs2_pid" ]; then
	echo "CS2 can't be found, is the game running?"
	exit 1
fi

lib_name="lib$(cat ProjectName).so"

gdb -p "$cs2_pid" \
	-ex "call ((void*(*)(char*, int)) dlopen)(\"$(pwd)/Build-Debug/$lib_name\", 1)" \
	-ex "continue"
