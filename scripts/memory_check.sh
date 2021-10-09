#!/bin/bash

OPTIONS="--tool=memcheck --leak-check=full --error-exitcode=1"
VALGRIND=$(which valgrind)
ROOT_DIR=$(pwd)
RETURN=0

for file in `ls build/test/spec`; do
    if [ -f $ROOT_DIR/build/test/spec/$file/$file ]; then
        newfile=`mktemp /tmp/test.XXXXXX` || exit 1
        cd $ROOT_DIR/build/test/spec/$file
        $VALGRIND $OPTIONS ./$file > $newfile 2>&1
        if [ $? != 0 ] ; then
            cat "${newfile}"
            RETURN=1
        fi
        rm "${newfile}"
        cd $ROOT_DIR
    fi
done

exit $RETURN