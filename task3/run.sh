#!/bin/bash

exec 1>& output

gcc task3_q.c -o q.o -DMSG_SIZE=100
echo "$((time ./q.o 1) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./)"
gcc task3_q.c -o q.o -DMSG_SIZE=4000
echo "$((time ./q.o 1) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./)"
gcc task3_q.c -o q.o -DMSG_SIZE=8000
echo "$((time ./q.o 1) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./)"



gcc task3_s.c -o s.o -DSZ=100
echo "$((time ./s.o 1) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./)"
gcc task3_s.c -o s.o -DSZ=4000
echo "$((time ./s.o 1) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./)"
gcc task3_s.c -o s.o -DSZ=8000
echo "$((time ./s.o 1) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./)"



gcc task3_f.c -o f.o -DLEN_ZNDKMQ_BUF=100 -lrt
echo "$((script -q -c "time ./f.o 1" /dev/null) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./)"
gcc task3_f.c -o f.o -DLEN_ZNDKMQ_BUF=4000 -lrt
echo "$((script -q -c "time ./f.o 1" /dev/null) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./)"
gcc task3_f.c -o f.o -DLEN_ZNDKMQ_BUF=8000 -lrt
echo "$((script -q -c "time ./f.o 1" /dev/null) 2>&1 | grep real | awk '{print $2}' | sed  s/[sm]//g | sed s/,/./)"

python3 hist.py
