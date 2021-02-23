#include"../head/SERVER.h"
DWORD WINAPI Check_alive(LPVOID qn)
{
    UserPacketInterface sendCLN;
    int len=0;
    char sendbuf1[sizeof(UserPacketInterface)]= {0};
    CLN* check_a=(CLN*)qn;
    Sleep(10*1000);
    while(check_a->info[0]!='N')
    {
        Sleep(2*1000);
        strcpy(sendCLN.checkcode,"BTA");
        sendCLN.save[99]='\n';
        memcpy(sendbuf1,&sendCLN,sizeof(sendCLN));
        len=send(check_a->remote_socket,sendbuf1,sizeof(sendCLN),0);
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(check_a->remote_socket);
            printf("\nUser %s 's connection was Lost!\n",check_a->USERID);
            delete_out_user(check_a);
            break;
        }
    }
    return 0;
}
