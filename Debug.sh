#!/bin/sh

echo "Building Debug-Binary"
mkdir -p Build-Debug

cmake -B Build-Debug -D CMAKE_BUILD_TYPE=Debug -D CMAKE_CXX_FLAGS_RELEASE="$CXXFLAGS" || exit 1
cmake --build Build-Debug -j "$(nproc)" || exit 1

# Set the DEBUGGER variable on the cmdline to use lldb or any debugger
# Make sure it supports the following commands
DEBUGGER="${DEBUGGER:=gdb}"
echo "Using '$DEBUGGER' to inject"
if ! [ -x "$(command -v $DEBUGGER)" ]; then
	echo "$DEBUGGER does not exist"
	echo "Install it or set the DEBUGGER variable to a replacement"
	exit 1
fi

cs2_pid=$(pidof cs2)
if [ -z "$cs2_pid" ]; then
	echo "CS:2 can't be found, is the game running?"
	exit 1
fi

lib_name="lib$(cat ProjectName).so"

$DEBUGGER -p "$cs2_pid" \
	-ex "call ((void*(*)(char*, int)) dlopen)(\"$(pwd)/Build-Debug/$lib_name\", 1)" \
	-ex "continue"
