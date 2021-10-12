#!/bin/bash
gcc task3_q.c -o q.o

exec 1>& output

echo "$((time ./q.o lil 1000) 2>&1 | grep real | awk '{print $2}'|sed  s/[sm]//g) / 1000"  | bc -l
echo "$((time ./q.o mid 1000) 2>&1 | grep real | awk '{print $2}'|sed  s/[sm]//g) / 1000" | bc -l
#echo "$((time ./q.o big 10) 2>&1 | grep real | awk '{print $2}'|sed  s/[sm]//g) / 10" | bc -l



gcc task3_s.c -o s.o

echo "$((time ./s.o lil 1000) 2>&1 | grep real | awk '{print $2}'|sed  s/[sm]//g) / 1000"  | bc -l
echo "$((time ./s.o mid 1000) 2>&1 | grep real | awk '{print $2}'|sed  s/[sm]//g) / 1000" | bc -l
echo "$((time ./s.o big 10) 2>&1 | grep real | awk '{print $2}'|sed  s/[sm]//g) / 10" | bc -l