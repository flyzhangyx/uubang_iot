#include "../head/libThreadPool.h"
#include "../head/SERVER.h"
/*线程池管理*/
struct threadPool_t{
    pthread_mutex_t lock;                 /* 锁住整个结构体 */
    pthread_mutex_t thread_counter;       /* 用于使用忙线程数时的锁 */
    pthread_cond_t  queue_not_full;       /* 条件变量，任务队列不为满 */
    pthread_cond_t  queue_not_empty;      /* 任务队列不为空 */

    pthread_t *threads;                   /* 存放线程的tid,实际上就是管理了线 数组 */
    pthread_t admin_tid;                  /* 管理者线程tid */
    TaskData_t *task_queue;        /* 任务队列 */

    /*线程池信息*/
    int min_thr_num;                      /* 线程池中最小线程数 */
    int max_thr_num;                      /* 线程池中最大线程数 */
    int live_thr_num;                     /* 线程池中存活的线程数 */
    int busy_thr_num;                     /* 忙线程，正在工作的线程 */
    int wait_exit_thr_num;                /* 需要销毁的线程数 */

    /*任务队列信息*/
    int queue_front;                      /* 队头 */
    int queue_rear;                       /* 队尾 */
    int queue_size;

    /* 存在的任务数 */
    int queue_max_size;                   /* 队列能容纳的最大任务数 */
    /*线程池状态*/
    int shutdown;                         /* true为关闭 */
};

#define DEFAULT_CHECK_INTERVAL 3
#define DEFAULT_THREAD_NUM 5
#define MIN_WAIT_TASK_NUM  5

#ifndef ESRCH
#define ESRCH 3
#endif

/*工作线程*/
void * work_thread(void *threadpool);

/*线程是否存活*/
static int is_thread_alive(pthread_t tid)
{
    int kill_rc = pthread_kill(tid, 0);     //发送0号信号，测试是否存活
    if (kill_rc == ESRCH)  //线程不存在
    {
        return 0;
    }
    return 1;
}


/*管理线程*/
int g_queue_size,g_live_thr_num,g_busy_thr_num=0;
static void * admin_thread(void *threadpool)
{
    int i;
    int PrintTime = 0;
    threadPool_t *pool = (threadPool_t *)threadpool;
    while (!pool->shutdown)
    {
        //log_info("admin -----------------");
        sleep(DEFAULT_CHECK_INTERVAL);               /*隔一段时间再管理*/
        pthread_mutex_lock(&(pool->lock));               /*加锁*/
        g_queue_size = pool->queue_size;               /*任务数*/
        g_live_thr_num = pool->live_thr_num;           /*存活的线程数*/
        pthread_mutex_unlock(&(pool->lock));             /*解锁*/
        pthread_mutex_lock(&(pool->thread_counter));
        g_busy_thr_num = pool->busy_thr_num;           /*忙线程数*/
        pthread_mutex_unlock(&(pool->thread_counter));
        if(PrintTime++>=0)
        {
            PrintTime = 0;
            log_info([THREAD POOL] BUSY: %d | LIVE: %d , g_busy_thr_num, g_live_thr_num);
           // log_info(");
        }
        /*创建新线程 实际任务数量大于 最小正在等待的任务数量，存活线程数小于最大线程数*/
        if (g_queue_size >= MIN_WAIT_TASK_NUM && g_live_thr_num <= pool->max_thr_num)
        {
            pthread_mutex_lock(&(pool->lock));
            int add=0;
            /*一次增加 DEFAULT_THREAD_NUM 个线程*/
            if(pool->live_thr_num < pool->max_thr_num)
            log_info("[THREAD POOL] %d NEW THREADS CREATE",DEFAULT_THREAD_NUM);
            for (i=0; i<pool->max_thr_num && add<DEFAULT_THREAD_NUM
                 && pool->live_thr_num < pool->max_thr_num; i++)
            {
                if (pool->threads[i] == 0 || !is_thread_alive(pool->threads[i]))
                {
                    pthread_create(&(pool->threads[i]), NULL, work_thread, (void *)pool);
                    add++;
                    pool->live_thr_num++;
                }
            }
            pthread_mutex_unlock(&(pool->lock));
        }

        /*销毁多余的线程 忙线程x2 都小于 存活线程，并且存活的大于最小线程数*/
        if ((g_busy_thr_num*2) < g_live_thr_num  &&  g_live_thr_num > pool->min_thr_num)
        {
            // log_info("admin busy --%d--%d----", busy_thr_num, live_thr_num);
            /*一次销毁DEFAULT_THREAD_NUM个线程*/
            pthread_mutex_lock(&(pool->lock));
            pool->wait_exit_thr_num = DEFAULT_THREAD_NUM;
            pthread_mutex_unlock(&(pool->lock));

            for (i=0; i<DEFAULT_THREAD_NUM; i++)
            {
                //通知正在处于空闲的线程，自杀
                pthread_cond_signal(&(pool->queue_not_empty));
            }
            log_info("[THREAD POOL] RELEASE %d REDUNDANT THREADS ",DEFAULT_THREAD_NUM);
        }
    }
    return NULL;
}

int libThreadPool_state(int *task_queue,int *live_thr_num,int *busy_thr_num)
{
    if(task_queue){
        *task_queue = g_queue_size;
    }
    if(live_thr_num){
        *live_thr_num = g_live_thr_num;
    }
    if(busy_thr_num){
        *busy_thr_num = g_busy_thr_num;
    }
    return OPCODE_SUCCESS;
}
/*释放线程池*/
int threadpool_free(threadPool_t *pool)
{
    if (pool == NULL)
        return -1;
    if (pool->task_queue){
        free(pool->task_queue);
        pool->task_queue = NULL;
    }
    if (pool->threads)
    {
        free(pool->threads);
        pthread_mutex_lock(&(pool->lock));               /*先锁住再销毁*/
        pthread_mutex_destroy(&(pool->lock));
        pthread_mutex_lock(&(pool->thread_counter));
        pthread_mutex_destroy(&(pool->thread_counter));
        pthread_cond_destroy(&(pool->queue_not_empty));
        pthread_cond_destroy(&(pool->queue_not_full));
    }
    free(pool);
    pool = NULL;
    return 0;
}


/*工作线程*/
void *work_thread(void *threadpool)
{
    threadPool_t *pool = (threadPool_t *)threadpool;
    TaskData_t task;
    while(1)
    {
        pthread_mutex_lock(&(pool->lock));
        /* 无任务则阻塞在 “queue_not_emperty” 上，有任务则跳出 */
        while ((pool->queue_size == 0) && (!pool->shutdown))
        {
            //log_info("thread 0x%x is waiting ", (unsigned int)pthread_self());
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->lock));

            /* 判断是否需要清除线程,自杀功能 */
            if (pool->wait_exit_thr_num > 0)
            {
                pool->wait_exit_thr_num--;
                /* 判断线程池中的线程数是否大于最小线程数，是则结束当前线程 */
                if (pool->live_thr_num > pool->min_thr_num)
                {
                    //log_info("[THREAD POOL] thread 0x%x is exiting ", (unsigned int)pthread_self());
                    pool->live_thr_num--;
                    pthread_mutex_unlock(&(pool->lock));
                    pthread_exit(NULL);//结束线程
                }
            }
        }

        /* 线程池开关状态 */
        if (pool->shutdown) //关闭线程池
        {
            pthread_mutex_unlock(&(pool->lock));
            log_info("[THREAD POOL] thread 0x%x is exiting ", (unsigned int)pthread_self());
            pthread_exit(NULL); //线程自己结束自己
        }

        //否则该线程可以拿出任务
        task.task_func = pool->task_queue[pool->queue_front].task_func; //出队操作
        task.arg = pool->task_queue[pool->queue_front].arg;

        pool->queue_front = (pool->queue_front + 1) % pool->queue_max_size;  //环型结构
        pool->queue_size--;

        //通知可以添加新任务

        pthread_cond_signal(&(pool->queue_not_full));

        //释放线程锁
        pthread_mutex_unlock(&(pool->lock));

        //执行刚才取出的任务
        //log_info("thread 0x%x start working ", (unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->thread_counter));            //锁住忙线程变量
        pool->busy_thr_num++;
        pthread_mutex_unlock(&(pool->thread_counter));

       (*(task.task_func))(task.arg);                           //执行任务

        //任务结束处理
        //log_info("thread 0x%x end working ", (unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->thread_counter));
        pool->busy_thr_num--;
        pthread_mutex_unlock(&(pool->thread_counter));
    }
    pthread_exit(NULL);
}

/*向线程池的任务队列中添加一个任务*/
int libThreadPool_TaskAdd(threadPool_t *pool, task_func function, void *arg)
{
    pthread_mutex_lock(&(pool->lock));

    /*如果队列满了,调用wait阻塞*/
    while ((pool->queue_size >= pool->queue_max_size) && (!pool->shutdown)){
        //log_info("[THREAD POOL] QUEUE IS FULL %p,%d",arg,pool->queue_size);
        pthread_cond_wait(&(pool->queue_not_full), &(pool->lock));
    }

    /*如果线程池处于关闭状态*/
    if (pool->shutdown)
    {
        log_info("[THREAD POOL] THREAD POOL WILL BE SHUTDOWN");
        pthread_mutex_unlock(&(pool->lock));
        return -1;
    }

    /*清空工作线程的回调函数的参数arg*/
    if (pool->task_queue[pool->queue_rear].arg != NULL)
    {
        //free(pool->task_queue[pool->queue_rear].arg);
        //pool->task_queue[pool->queue_rear].arg = NULL;
    }

    /*添加任务到任务队列*/
    pool->task_queue[pool->queue_rear].task_func = function;
    pool->task_queue[pool->queue_rear].arg = arg;
    pool->queue_rear = (pool->queue_rear + 1) % pool->queue_max_size;  /* 逻辑环  */
    pool->queue_size++;
    /*添加完任务后,队列就不为空了,唤醒线程池中的一个线程*/
    pthread_cond_signal(&(pool->queue_not_empty));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}


/*创建线程池*/
threadPool_t * libThreadPool_Init(int min_thr_num, int max_thr_num, int queue_max_size)
{
    int i;
    threadPool_t *pool = NULL;
    do
    {
        /* 线程池空间开辟 */
        if ((pool=(threadPool_t *)malloc(sizeof(threadPool_t))) == NULL)
        {
            fprintf(stderr,"[THREAD POOL] MALLOC RAM FALSE ");
            exit(-1);

        }
        memset(pool , 0,sizeof(threadPool_t));
        /*信息初始化*/
        pool->min_thr_num = min_thr_num;
        pool->max_thr_num = max_thr_num;
        pool->busy_thr_num = 0;
        pool->live_thr_num = min_thr_num;
        pool->wait_exit_thr_num = 0;
        pool->queue_front = 0;
        pool->queue_rear = 0;
        pool->queue_size = 0;
        pool->queue_max_size = queue_max_size;
        pool->shutdown = 0;

        /* 根据最大线程数，给工作线程数组开空间，清0 */
        pool->threads = (pthread_t *)malloc(sizeof(pthread_t)*max_thr_num);
        if (pool->threads == NULL)
        {
            fprintf(stderr,"[THREAD POOL] MALLOC THREADS FALSE");
            exit(-1);
        }
        memset(pool->threads, 0, sizeof(pthread_t)*max_thr_num);

        /* 队列开空间 */
        pool->task_queue =
        (TaskData_t *)malloc(sizeof(TaskData_t)*queue_max_size);
        if (pool->task_queue == NULL)
        {
            fprintf(stderr,"[THREAD POOL] MALLOC TASK QUEUE FASLE");
            exit(-1);
        }
        memset(pool->task_queue,0,sizeof(TaskData_t)*queue_max_size);
        /* 初始化互斥锁和条件变量 */
        if (pthread_mutex_init(&(pool->lock), NULL) != 0           ||
            pthread_mutex_init(&(pool->thread_counter), NULL) !=0  ||
            pthread_cond_init(&(pool->queue_not_empty), NULL) !=0  ||
            pthread_cond_init(&(pool->queue_not_full), NULL) !=0)
        {
            log_info("[THREAD POOL] INIT MUTEX OR COND FALSE");
            break;
        }

        /* 启动min_thr_num个工作线程 */
        for (i=0; i<min_thr_num; i++)
        {
            /* pool指向当前线程池  threadPool_thread函数在后面讲解 */
            pthread_create(&(pool->threads[i]), NULL, work_thread, (void *)pool);
            //log_info("start thread num %d...0x%x ",i, (unsigned int)pool->threads[i]);
        }
        /* 管理者线程 admin_thread函数在后面讲解 */
        pthread_create(&(pool->admin_tid), NULL, admin_thread, (void *)pool);

        return pool;
    } while(0);

    /* 释放pool的空间 */
    threadpool_free(pool);
    return NULL;
}

/*销毁线程池*/
int libThreadPool_destroy(threadPool_t *pool)
{
    int i;
    if (pool == NULL)
    {
        return OPCODE_ERR_PARA;
    }
    pool->shutdown = 1;

    /*销毁管理者线程*/
    pthread_join(pool->admin_tid, NULL);

    //通知所有线程去自杀(在自己领任务的过程中)
    for (i=0; i<pool->live_thr_num; i++)
    {
        pthread_cond_broadcast(&(pool->queue_not_empty));
    }

    /*等待线程结束 先是pthread_exit 然后等待其结束*/
    for (i=0; i<pool->live_thr_num; i++)
    {
        pthread_join(pool->threads[i], NULL);
    }

    threadpool_free(pool);
    return OPCODE_SUCCESS;
}
