#!/bin/bash

# Pack images in a single one.
# This is done to improve speed of rendering since textures switching
# seems to be slow.

# Note: for now, we just pack image verticaly, it would be cool to have
# an algorithm who found the best layout. For now, keep it simple, stupid.

# The first argument is the output file
out="$1"; shift
offset=0

convert -background none -trim -append "$@" "$out"
outid=$(identify -format "%t" "$out" | tr '[:lower:]' '[:upper:]')
echo "ADD_TEXTURE($outid, \"$out\")"

for i; do
    texid=$(identify -format "%t" "$i" | tr '[:lower:]' '[:upper:]')
    
    height=$(convert -trim -format "%h" "$i" info:)
    width=$(convert -trim -format "%w" "$i" info:)
    printf "ADD_IN_TEXTURE(%-20s %s, %4d, %4d, %4d, %4d)\n" "$texid," "$outid" 0 $offset $width $height
    offset=$(($offset + $height + 1))
done
