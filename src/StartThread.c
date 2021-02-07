#include"../head/SERVER.h"
int StartThread(CLN *a)
{
    CLN_thread[CLN_num]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)talk,a,0,NULL);
    printf("用户%d已接入,线程已开启",CLN_num);
    return AcceptClient();
}
