#include"../head/SERVER.h"
extern int MemPoolAvailable;
extern int isConnected;
DWORD WINAPI CheckUserSceneCmd()
{
    int printCount = 0;
    struct pool_stat temp;
    struct tm *p;
    time_t t,t1;
    //char time_now[50];
    while(!isShutDown)
    {
        printCount ++;
        if(printCount==25)
        {
            printCount = 0;
        }
        t =time(NULL);//Now Time
        p=localtime(&t);
        p->tm_hour=0;
        p->tm_mday=p->tm_mday+1;
        p->tm_min=0;
        p->tm_sec=0;
        t1=mktime(p);
        if((t1-t)/60>0&&(t1-t)/60<=5*60)
        {

        }
        if(printCount==10||printCount==23)
        {
            PingMallocConnList();
            freeConnMemWait4Free();
        }
        if(printCount==24)
        {
            stpool_stat(ThreadPool_ExecuteTask, &temp);
            log_debug("CONN2BEFREE_THREAD/TASK:[%d/%d]",temp.curthreads_active,temp.curtasks_pending);
            stpool_stat(ThreadPool_ExecuteMsg, &temp);
#ifdef MemPool
            float UsedMem = (float)MemPoolAvailable*sizeof(CLN)/(1024*1024);
            log_info("ONLINE_USER:[%d] ONLINE_IOT:[%d] THREAD/TASK:[%d/%d] MEM_POOL:[%.2fMB] %s",onlineUserHead->OnlineUserNum,onlineIotHead->OnlineUserNum,temp.curthreads_active,temp.curtasks_pending,UsedMem,isConnected?"CONNECT":"DISCONNECT");
#else
            float UsedMem = MemoryPoolGetUsage(mp)*100*500;
            log_info("ONLINE_USER:[%d] ONLINE_IOT:[%d] THREAD/TASK:[%d/%d] MEM_POOL:[%.2f%%] %s",onlineUserHead->OnlineUserNum,onlineIotHead->OnlineUserNum,temp.curthreads_active,temp.curtasks_pending,MemoryPoolGetUsage(mp)*100,isConnected?"CONNECT":"DISCONNECT");
#endif // MemPool
            UpdateServerRunInfo(GetCPUUseRate(),UsedMem,temp.curtasks_pending,onlineUserHead->OnlineUserNum,onlineIotHead->OnlineUserNum);
        }
        Sleep(200);
    }
    return 1;
}
HANDLE StartCheckUserScene()
{
    return CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CheckUserSceneCmd,NULL,0,NULL);
}
