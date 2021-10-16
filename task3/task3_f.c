#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>             
#include <string.h>            
#include <mqueue.h>                         
//#include <errno.h>  

#define  NAME_ZNDKMQ           "/zndmkq" 
#define     N_ZNDKMQ_MSGS            10
//#define   LEN_ZNDKMQ_BUF           4000



typedef struct {
    mqd_t           mq;                  
    struct mq_attr  attr;                 
    char            rbuf[LEN_ZNDKMQ_BUF];
} TZndkMqInfo;
static       TZndkMqInfo       ZndkMq;
#define _get_zndkmq_info()   (&ZndkMq)


int
_init_zndkmq_info(TZndkMqInfo *info)
{
    int             stat = 0;
    struct mq_attr *attr = &info->attr;

    memset(info, 0, sizeof(TZndkMqInfo));

    attr->mq_flags   = 0;
    attr->mq_maxmsg  = N_ZNDKMQ_MSGS;
    attr->mq_msgsize = LEN_ZNDKMQ_BUF;
    attr->mq_curmsgs = 0; 

    return  stat;
}


int
main(int argc, char *argv[])
{
    pid_t pid = fork();
    if (pid==0) {
        FILE *f;
        f = fopen("res", "w");
    int             stat   =  EXIT_SUCCESS;
    TZndkMqInfo    *info   = _get_zndkmq_info();
    struct mq_attr *attr;
    mqd_t           mq;
    int             nbytes = 1;
    char           *rbuf;

    _init_zndkmq_info(info);
    attr     = &info->attr;


    mq       =  mq_open(NAME_ZNDKMQ, O_RDWR | O_CREAT | O_EXCL, 0644, attr);
    if (mq  == (mqd_t)-1) {
        //perror(" mq_open() error " NAME_ZNDKMQ);
        printf(" %s():[L%4d]: mq_open() error (%d)\n", __func__, __LINE__, (int)mq);
        stat = EXIT_FAILURE;
        goto err1;
    }
    info->mq = mq;


    rbuf     = info->rbuf;
    int c = 1;
    while (c) {
        nbytes = mq_receive(mq, rbuf, attr->mq_msgsize, NULL);
        rbuf[nbytes] = '\0';    /* NULL termination */
        c = fwrite(rbuf, sizeof(char), nbytes, f);
    }
    fwrite(rbuf, sizeof(char), nbytes, f);
    fclose(f);
    mq   =  mq_close(mq);
    if (mq == (mqd_t)-1) {
        //perror(" mq_close() error " NAME_ZNDKMQ);
        printf(" %s():[L%4d]: mq_close() error (%d)\n", __func__, __LINE__, (int)mq);
        stat = EXIT_FAILURE;
    }
    err1:
    mq_unlink(NAME_ZNDKMQ);
    return  stat;
    }


    if (pid) {
        FILE *f;
        f = fopen("file", "r");
        sleep(1);
        int     stat   = EXIT_SUCCESS;
    mqd_t   mq;
    char    wbuf[LEN_ZNDKMQ_BUF];

    mq   =  mq_open(NAME_ZNDKMQ, O_WRONLY);
    if (mq == (mqd_t)-1) {
        mq   =  mq_open(NAME_ZNDKMQ, O_WRONLY | O_CREAT);
        stat = EXIT_FAILURE;
        goto err2;
    }

    int c = 1;
    do {
        memset(wbuf, 0, sizeof(wbuf));
        c = fread(wbuf, sizeof(char), LEN_ZNDKMQ_BUF, f);
        if (c) {
        stat = mq_send(mq, wbuf, sizeof(wbuf), 0);
        if (stat < 0) {
            //perror(" mq_send() error \n" NAME_ZNDKMQ);
            stat = EXIT_FAILURE;
        }}

    } while (c);
    fclose(f);
    err2:
    mq_unlink(NAME_ZNDKMQ);
    return  stat;
    }
}
