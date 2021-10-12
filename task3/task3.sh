#!/bin/bash

exec 1>& output

gcc task3_q.c -o q.o -DMSG_SIZE=5
echo "$((time ./q.o 1) 2>&1 | grep real | awk '{print $2}'|sed  s/[sm]//g) / 1"  | bc -l
gcc task3_q.c -o q.o -DMSG_SIZE=2048
echo "$((time ./q.o 10) 2>&1 | grep real | awk '{print $2}'|sed  s/[sm]//g) / 10" | bc -l
#gcc task3_q.c -o q.o -DMSG_SIZE=65536
#echo "$((time ./q.o 10) 2>&1 | grep real | awk '{print $2}'|sed  s/[sm]//g) / 10" | bc -l



#gcc task3_s.c -o s.o -DSZ=5
#echo "$((time ./s.o 10) 2>&1 | grep real | awk '{print $2}'|sed  s/[sm]//g) / 10"  | bc -l
#gcc task3_s.c -o s.o -DSZ=4096
#echo "$((time ./s.o 10) 2>&1 | grep real | awk '{print $2}'|sed  s/[sm]//g) / 10" | bc -l
#gcc task3_s.c -o s.o -DSZ=65536
#echo "$((time ./s.o 10) 2>&1 | grep real | awk '{print $2}'|sed  s/[sm]//g) / 10" | bc -l