#!/bin/sh
# Author: Amittai Aviram - aviram@bc.edu

APP=pa2
HOST=cslabv2
EXPECTED=expected_${APP}_${HOST}.txt
OUTPUT=output.txt

echo Building ...
gcc -Wall -Werror -std=c99 ${APP}.c -o ${APP}
echo Building complete.

echo Running ...
./${APP} test > ${OUTPUT}
echo Running complete.

echo Comparing real and expected output.
DIFF=$(diff -b ${OUTPUT} $EXPECTED)
if [ "$DIFF" != "" ]
then
    printf "\nXXXXXXXX UNEXPECTED OUTPUT XXXXXXXX\n\n"
    printf "You can preserve the output by commenting out the \"rm\" commmand\n"
    printf "at the bottom of the test script and re-running the test.\n"
    printf "Then you can examine ${OUTPUT} for clues.\n"
else
    printf "\n======== CORRECT ========\n\n"
fi
rm ${OUTPUT}

