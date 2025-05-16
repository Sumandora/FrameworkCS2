#!/bin/sh

cs2_pid=$(pidof cs2)
if [ -z "$cs2_pid" ]; then
	echo "CS:2 can't be found, is the game running?"
	exit 1
fi

lib_name="lib$(cat ProjectName).so"
cp Build/"$lib_name" /usr/lib64/

# The following is copied from Fuzion (https://github.com/LWSS/Fuzion/blob/0a4d775e17aba7a723aadce5b80898705e0bd6ff/load#L25); Thanks LWSS and contributors
# pBypass for crash dumps being sent
# You may also want to consider using -nobreakpad in your launch options.
rm -rf /tmp/dumps # Remove if it exists
mkdir --mode=000 /tmp/dumps # Make it as root with no permissions

# https://www.kernel.org/doc/Documentation/security/Yama.txt
sysctl -w kernel.yama.ptrace_scope=2 # Only allows root to inject code. This is temporary until reboot.

killall -19 steam
killall -19 steamwebhelper

gdb -p "$cs2_pid" -n -q -batch \
    -ex "call ((void*(*)(char*, int)) dlopen)(\"/usr/lib64/$lib_name\", 1)" \
    -ex "call ((char*(*)(void)) dlerror)()" \
    -ex "detach" \
    -ex "quit" 2>&1 || {
	echo "A error has appeared"
	echo "$DEBUGGER has failed to dlopen the library"

	if [ $DEBUGGER != "gdb" ]; then
		echo "- Make sure that $DEBUGGER supports the same usage as GDB"
	fi
}

# You have to restart your kernel to reinject btw ^^
sysctl -w kernel.yama.ptrace_scope=3

sleep 1
killall -18 steamwebhelper
killall -18 steam

echo "Process complete"
echo "If you face problems related to the injection process"
echo "you should provide the gdb session and build log to whoever is helping you."
