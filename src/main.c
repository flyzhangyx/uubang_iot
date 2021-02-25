#include"../head/SERVER.h"
int main(int argc,char**argv)
{
    srand((unsigned int)time(NULL));
    initServer(atoi(argv[1]));
    printf("服务器初始化成功,端口:%d\n",atoi(argv[1]));
    CreateDailyMsgdb();//New Daily DB Thread
    ///**********Init Thread Pool************///
#ifdef STPOOL
    long eCAPs = eCAP_F_DYNAMIC|eCAP_F_ROUTINE|eCAP_F_TASK_WAIT_ALL;
    /** 创建线程池 */
    ThreadPool = stpool_create("mypool", /** 线程池名                      */
                               eCAPs,    /** 期望libstpool提供的的功能特性 */
                               10,	   /** 线程池中运行的最大线程数目    */
                               5,	   /** 预启动提供服务的的线程数目    */
                               0,	   /** 保持线程池创建后调度任务状态  */
                               1		   /** 优先级队列数目                */
                              );
    /**********************************/
#else
    ThreadPool = libThreadPool_Init(10,30,60);
#endif
    time_t now_time;
    time(&now_time);
    char time_now[50];
    strftime(time_now,80,"%Y-%m-%d %X",localtime(&now_time));
    printf("\n%s|%s\n",GetUpdateTimeStamp(0,3),time_now);

    CLN a;
    a.USERKEY_ID = 22947;
    strcpy(a.TalktoID,"ZZzoidFy");
    char cmd[100]="123";
    UserNewIotCmd(&a,cmd,1,1,"2021-2-25 16:38:25");
    IotReadCmd(&a,1,0);
    printf("\n%s\n",a.data);

    AcceptClient();
    pthread_mutex_destroy(&(RegistedIotHead->mute));
    pthread_mutex_destroy(&(RegistedUserHead->mute));
    pthread_mutex_destroy(&(onlineIotHead->mute));
    pthread_mutex_destroy(&(onlineUserHead->mute));
    pthread_mutex_destroy(&mysql_mutex);
    mysql_close(sock);
#ifdef STPOOL
    stpool_release(ThreadPool);
#else
    libThreadPool_destroy(ThreadPool);
#endif
    return 0;
}

