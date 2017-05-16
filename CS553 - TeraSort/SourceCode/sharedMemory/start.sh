#!/bin/sh

threads=$1
#records=$(($2/100))
#
#echo $threads
#echo $records
#
#cd gensort
#./gensort -a $records ../input
#
#cd ../

gcc sortData.c -o sort -lm
gcc mergeData.c -o merge -lm
gcc sharedMemory.c -o prog -lpthread -lm
./prog $threads