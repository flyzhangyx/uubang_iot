#include"../head/SERVER.h"
#define RELEASE_ONLINE_REG_NODE() do{\
    USER temp = RegistedIotHead->next;\
    while(temp!=NULL)\
    {\
        free(temp);\
        temp = temp->next;\
    }\
    temp = RegistedUserHead->next;\
    while(temp!=NULL)\
    {\
        free(temp);\
        temp = temp->next;\
    }\
    temp = onlineIotHead->next;\
    while(temp!=NULL)\
    {\
        free(temp);\
        temp = temp->next;\
    }\
    temp = onlineUserHead->next;\
    while(temp!=NULL)\
    {\
        free(temp);\
        temp = temp->next;\
    }}while(0)

int main(int argc,char**argv)
{
    SetConsoleTitleA("IotServer");
    srand((unsigned int)time(NULL));
    initServer(atoi(argv[1]),argv[2]);
    ConnectToTopServer();
    log_info("服务器初始化成功,端口:%d",atoi(argv[1]));
    ///*******************************
    CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)EnterManualCtrlMode,NULL,0,NULL));
    ///*******************************
    CloseHandle(CreateDailyMsgdb());//New Daily DB Thread
    StartCheckUserScene();//
    ///*********************************
    time_t now_time;
    time(&now_time);
    char time_now[50];
    strftime(time_now,80,"%Y-%m-%d %X",localtime(&now_time));
    log_info("%s",time_now);
    ///*****************************************************************
    AcceptClient();
    Sleep(5000);
    ///*****************************************************************
    log_info("Program End Manually");
    fclose(loginfo);
    pthread_mutex_destroy(&(RegistedIotHead->mute));
    pthread_mutex_destroy(&(RegistedUserHead->mute));
    pthread_mutex_destroy(&(onlineIotHead->mute));
    pthread_mutex_destroy(&(onlineUserHead->mute));
    pthread_mutex_destroy(&mysql_mutex);
    CloseHandle(Con2FreeLink_mutex);
    sql_pool_destroy(MySqlConnPool);
#ifdef MemPool
    freeMemPool();
#else
    MemoryPoolDestroy(mp);
#endif // MemPool

#ifdef STPOOL
    stpool_release(ThreadPool_ExecuteMsg);
    stpool_release(ThreadPool_ExecuteTask);
#else
    libThreadPool_destroy(ThreadPool_ExecuteMsg);
    libThreadPool_destroy(ThreadPool_ExecuteTask);
#endif
    RELEASE_ONLINE_REG_NODE();
    return 0;
}

