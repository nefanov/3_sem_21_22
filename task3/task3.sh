#!/bin/bash

exec 1>& output

gcc task3_q.c -o q.o -DMSG_SIZE=20
echo "$((time ./q.o 10) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./) / 10" | bc -l
gcc task3_q.c -o q.o -DMSG_SIZE=4000
echo "$((time ./q.o 10) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./) / 10" | bc -l
gcc task3_q.c -o q.o -DMSG_SIZE=8000
echo "$((time ./q.o 10) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./) / 10" | bc -l



gcc task3_s.c -o s.o -DSZ=20
echo "$((time ./s.o 10) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./) / 10"  | bc -l
gcc task3_s.c -o s.o -DSZ=4000
echo "$((time ./s.o 10) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./) / 10" | bc -l
gcc task3_s.c -o s.o -DSZ=8000
echo "$((time ./s.o 10) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./) / 10" | bc -l



gcc task3_f.c -o f.o -DLEN_ZNDKMQ_BUF=20 -lrt
echo "$((script -q -c "time ./f.o 10" /dev/null) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./)"
gcc task3_f.c -o f.o -DLEN_ZNDKMQ_BUF=4000 -lrt
echo "$((script -q -c "time ./f.o 10" /dev/null) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./)"
gcc task3_f.c -o f.o -DLEN_ZNDKMQ_BUF=8000 -lrt
echo "$((script -q -c "time ./f.o 10" /dev/null) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./)"

python hist.py
