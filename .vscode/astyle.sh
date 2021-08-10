#!/bin/sh

FILES=`git diff --cached --name-only --diff-filter=ACMR | grep -E "\.(c|cpp|h|hpp)$"`
for FILE in $FILES; do
    nf=`git checkout-index --temp $FILE | cut -f 1`
    astyle --style=kr --exclude=build/ --exclude=third_party/ --indent=spaces=4 --indent-switches --suffix=none --recursive < $nf > $FILE
    rm $nf
done