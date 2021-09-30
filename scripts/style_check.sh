#!/bin/bash

OPTIONS="--style=kr --exclude=build*/ --exclude=third_party/ --indent=spaces=4 --indent-switches"
ASTYLE=$(which astyle)
RETURN=0

DIFF=$(which colordiff)
if [ $? -ne 0 ]; then
    DIFF=diff
fi

for file in `find src | grep -E "\.(c|h)$"`; do
    newfile=`mktemp /tmp/test.XXXXXX` || exit 1
    $ASTYLE $OPTIONS < $file > $newfile 2>> /dev/null
    $DIFF -u -p -B  "${file}" "${newfile}"
    r=$?
    rm "${newfile}"
    if [ $r != 0 ] ; then
        echo "[!] $file does not follow the consistent coding style." >&2
        RETURN=1
    fi
done

exit $RETURN