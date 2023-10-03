#!/bin/bash

# Set the DEBUGGER variable on the cmdline to use lldb or any debugger
# Make sure it supports the following commands
DEBUGGER="${DEBUGGER:=gdb}"
echo "Using '$DEBUGGER' to uninject"
if ! [ -x "$(command -v $DEBUGGER)" ]; then
	echo "$DEBUGGER does not exist"
	echo "Install it or set the DEBUGGER variable to a replacement"
	exit 1
fi

cs2_pid=$(pidof cs2)
if [ -z "cs2_pid" ]; then
	echo "CS:2 can't be found, is the game running?"
	exit 1
fi

lib_name="lib$(cat ProjectName).so"

killall -19 steam
killall -19 steamwebhelper

$DEBUGGER -p "$cs2_pid" -n -q -batch \
  -ex "set \$library = ((void*(*)(char*, int)) dlopen)(\"/usr/lib/$lib_name\", 6)" \
  -ex "set \$dlclose = (int(*)(void*)) dlclose" \
  -ex "call \$dlclose(\$library)" \
  -ex "call \$dlclose(\$library)" 2>&1 || {
	echo "A error has appeared"
	echo "$DEBUGGER has failed to dlopen/dlclose the library"

	if [ $DEBUGGER != "gdb" ]; then
		echo "- Make sure that $DEBUGGER supports the same usage as GDB"
	fi
	
	echo "- A possible error could be this"
	echo "https://github.com/LWSS/Fuzion/issues/621#issuecomment-777815272"
}

sleep 1
killall -18 steamwebhelper
killall -18 steam

echo "Process complete"
echo "If you face problems related to the unload process"
echo "you should provide the gdb session and build log to whoever is helping you."
