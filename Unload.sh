#!/bin/bash

cs2_pid=$(pidof cs2)
if [ -z "$cs2_pid" ]; then
	echo "CS:2 can't be found, is the game running?"
	exit 1
fi

lib_name="lib$(cat ProjectName).so"

killall -19 steam
killall -19 steamwebhelper

gdb -p "$cs2_pid" -n -q -batch \
  -ex "set \$library = ((void*(*)(char*, int)) dlopen)(\"/usr/lib/$lib_name\", 6)" \
  -ex "set \$dlclose = (int(*)(void*)) dlclose" \
  -ex "call \$dlclose(\$library)" \
  -ex "call \$dlclose(\$library)" 2>&1 || {
	echo "A error has appeared"
	echo "gdb has failed to dlopen/dlclose the library"
}

sleep 1
killall -18 steamwebhelper
killall -18 steam

echo "Process complete"
echo "If you face problems related to the unload process"
echo "you should provide the gdb session and build log to whoever is helping you."
