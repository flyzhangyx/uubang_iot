#include"../head/SERVER.h"
int main(int argc,char**argv)
{
    SetConsoleTitleA("IotServer");
    srand((unsigned int)time(NULL));
    initServer(atoi(argv[1]));
    ConnectToTopServer();
    log_info("服务器初始化成功,端口:%d",atoi(argv[1]));
    CreateDailyMsgdb();//New Daily DB Thread
    StartCheckUserScene();//
///9999999999999999999999999999999999999999999999999999999
    time_t now_time;
    time(&now_time);
    char time_now[50];
    strftime(time_now,80,"%Y-%m-%d %X",localtime(&now_time));
    log_info("%s",time_now);
    CLN a;
    UpdateLocalRegUserAndIotlist();
    int i = 60;

    //NewIotCmdToBeExecute(&a,cmd,1,0,time_now);
    //IotReadCmd(&a,1,0);
    //log_info("%s",a.data);
    //NewUserSceneCmdStore(&a,"",0,0,2,"23:00:00","1111111");
    //ReadOrDeleteUserScene(&a,"tvVmreNKu8R",2,1);
    ///88888888888888888888888888888888888888888888888888
    AcceptClient();
    ///*****************************************************************
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
    return 0;
}

