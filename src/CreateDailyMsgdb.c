#include"../head/SERVER.h"
DWORD WINAPI CreateDailyMsgdbThread()
{
    int flag = 0;
    while(!isShutDown)
    {
        struct tm *p;
        time_t t,t1;
        t =time(NULL);//Now Time
        p=localtime(&t);
        p->tm_hour=0;
        p->tm_mday=p->tm_mday+1;
        p->tm_min=0;
        p->tm_sec=0;
        t1=mktime(p);
        if((t1-t)/60>0&&(t1-t)/60<=5*60)
        {
            int i=0;
            while(i++<100&&!flag)
            {
                int ret = NewUserMsgTableInSQL();
                if(ret==0)
                {
                    Sleep(1000);
                    continue;
                }
                else if(ret==-1)
                {
                    flag = 1;
                    break;
                }
                else
                {
                    log_info("New Daily MSGDB Create SUCCESS!");
                    flag = 1;
                    break;
                }
            }
            if(i>=99)
            {
                exit(-12);
            }
        }
        else
        {
             flag = 0;
        }
        Sleep(1000*60);
    }
    return 1;
}
HANDLE CreateDailyMsgdb()
{
    return CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CreateDailyMsgdbThread,NULL,0,NULL);
}
