#include"../head/SERVER.h"
int StartThread(CLN *a)
{
    CLN_thread[CLN_num]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)talk,a,0,NULL);
    printf("�û�%d�ѽ���,�߳��ѿ���",CLN_num);
    return AcceptClient();
}
