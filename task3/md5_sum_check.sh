md5sum file

gcc task3_q.c -o q.o -DMSG_SIZE=100
./q.o 1
md5sum res

gcc task3_s.c -o s.o -DSZ=10
./s.o 1
md5sum res

gcc task3_f.c -o f.o -DLEN_ZNDKMQ_BUF=100 -lrt
./f.o 1
md5sum res