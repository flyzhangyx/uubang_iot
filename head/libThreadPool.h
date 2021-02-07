#ifndef libThreadPool_h
#define libThreadPool_h

#define OPCODE_SUCCESS 0
#define OPCODE_FAILURE -1
#define OPCODE_ERR_PARA -2
#define OPCODE_ERR_BIND_DEV -3
#define OPCODE_ERR_BIND_PORT -4
#define OPCODE_ERR_ILLEGAL_FD -5
#define OPCODE_ERR_BUSY -6
#include <stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
typedef struct threadPool_t threadPool_t;
typedef int(*task_func)(void *arg);

typedef struct task_data{
    void *arg;
    task_func task_func;
}TaskData_t;

threadPool_t * libThreadPool_Init(int min_thr_num, int max_thr_num, int queue_max_size);
int libThreadPool_destroy(threadPool_t *pool);
int libThreadPool_TaskAdd(threadPool_t *pool, task_func function, void *arg);
int libThreadPool_state(int *task_queue,int *live_thr_num,int *busy_thr_num);
#endif /* libThreadPool_h */
