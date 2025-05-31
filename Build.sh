#!/bin/sh

error () {
	echo "A error appeared"
	echo "If you are unable to figure out the problem and solve it yourself,"
	echo "you can contact a support instance."
	echo "Supply the build log to speed up the support process"
	exit 1
}

echo "Building..."
echo "This can take up to a couple minutes on slow hardware"

mkdir -p Build

LANG=en cmake -B Build -D CMAKE_BUILD_TYPE=Release -D CMAKE_CXX_FLAGS_RELEASE="$CXXFLAGS" || error
LANG=en cmake --build Build -j "$(nproc)" || error

strip -x -s Build/lib"$(cat ProjectName)".so || error

echo "The ELF-Binary has been built in the \"Build\"-directory"
