#include"../head/SERVER.h"
int main(int argc,char**argv)
{
    srand((unsigned int)time(NULL));
    initServer(atoi(argv[1]));
    printf("��������ʼ���ɹ�,�˿�:%d\n",atoi(argv[1]));
    CreateDailyMsgdb();//New Daily DB Thread
    ///**********Init Thread Pool************///
#ifdef STPOOL
    long eCAPs = eCAP_F_DYNAMIC|eCAP_F_ROUTINE|eCAP_F_TASK_WAIT_ALL;
    /** �����̳߳� */
    ThreadPool = stpool_create("mypool", /** �̳߳���                      */
                               eCAPs,    /** ����libstpool�ṩ�ĵĹ������� */
                               10,	   /** �̳߳������е�����߳���Ŀ    */
                               5,	   /** Ԥ�����ṩ����ĵ��߳���Ŀ    */
                               0,	   /** �����̳߳ش������������״̬  */
                               1		   /** ���ȼ�������Ŀ                */
                              );
    /**********************************/
#else
    ThreadPool = libThreadPool_Init(10,30,60);
#endif

    time_t now_time;
    time(&now_time);
    char time_now[50];
    strftime(time_now,80,"%Y-%m-%d %X",localtime(&now_time));
    printf("%s|%s\n",GetUpdateTimeStamp(0,3),time_now);
    AcceptClient();
    pthread_mutex_destroy(&(RegistedIotHead->mute));
    pthread_mutex_destroy(&(RegistedUserHead->mute));
    pthread_mutex_destroy(&(onlineIotHead->mute));
    pthread_mutex_destroy(&(onlineUserHead->mute));
    mysql_close(sock);
#ifdef STPOOL
    stpool_release(ThreadPool);
#else
    libThreadPool_destroy(ThreadPool);
#endif
    return 0;
}

