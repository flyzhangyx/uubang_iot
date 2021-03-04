#include"../head/SERVER.h"
DWORD WINAPI CheckUserSceneCmd()
{
    struct pool_stat temp;
    struct tm *p;
    time_t t,t1;
    char time_now[50];
    while(1)
    {
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
        stpool_stat(ThreadPool_ExecuteTask, &temp);
        log_debug("CONN2BEFREE_THREAD/TASK:[%d/%d]",temp.curthreads_active,temp.curtasks_pending);
        freeConnMemWait4Free();
        stpool_stat(ThreadPool_ExecuteMsg, &temp);
        log_info("ONLINE_USER:[%d] ONLINE_IOT:[%d] THREAD/TASK:[%d/%d] ",onlineUserHead->OnlineUserNum,onlineIotHead->OnlineUserNum,temp.curthreads_active,temp.curtasks_pending);
        Sleep(1000*5);
    }
}
void StartCheckUserScene()
{
    CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CheckUserSceneCmd,NULL,0,NULL);
}
