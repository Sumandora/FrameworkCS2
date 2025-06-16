#!/bin/bash

# This script automatically recurses through the imports of a protobuf file!

# Shoutout to GameTracking-CS2, SteamDB, and everyone involved :wave:
BASE_LINK='https://raw.githubusercontent.com/SteamDatabase/GameTracking-CS2/refs/heads/master/Protobufs/'

DIR=$(dirname "$0")

rm -f "$DIR"/*.proto

recurse_proto() {
	local file=$1;

	if [[ -f $file ]]; then
		echo "$file already exists."
		return
	fi

	echo "Downloading $file."
	wget -q "$BASE_LINK$file" -O "$DIR/$file"

	grep "^import" "$file" | awk -F '"' '{ print $2; }' | grep -v "^google" | {
		while read -r line; do
			recurse_proto "$line"
		done
	}
}


recurse_proto cs_usercmd.proto
