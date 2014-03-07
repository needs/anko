#!/bin/bash

# Pack images in a single one.
# This is done to improve speed of rendering since textures switching
# seems to be slow.

# Note: for now, we just pack image verticaly, it would be cool to have
# an algorithm who found the best layout. For now, keep it simple, stupid.

# The first argument is the output file
out="$1"; shift
offset=0

convert -background transparent -trim -append "$@" "$out"
outid=$(identify -format "%t" "$out" | tr '[:lower:]' '[:upper:]')
echo "ADD_TEXTURE($outid, \"$out\")"

for i; do
    texid=$(identify -format "%t" "$i" | tr '[:lower:]' '[:upper:]')
    
    owidth=$(identify -format "%w" "$i")
    oheight=$(identify -format "%h" "$i")
    width=$(convert -trim -format "%w" "$i" info:)
    height=$(convert -trim -format "%h" "$i" info:)
    offx=$((($owidth / 2) - $(convert -trim -format "%X" "$i" info:)))
    offy=$((($oheight / 2) - $(convert -trim -format "%Y" "$i" info:)))

    printf "ADD_IN_TEXTURE(%-20s %s, %4d, %4d, %4d, %4d, %4d, %4d)\n" "$texid," "$outid" 0 $offset $width $height $offx $offy
    offset=$(($offset + $height))
done
