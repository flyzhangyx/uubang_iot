#include"../head/SERVER.h"
DWORD WINAPI fun(LPVOID qn)
{
    char sendbuf[sizeof(sendbag)]= {0};
    SOCKADDR_IN remoteAddr;
    sendbag RecDataStruct_udp,rec;
    int nAddrLen = sizeof(remoteAddr);
    while(1)
    {
        int ret = recvfrom(server_sockfd_udp,(char*)&RecDataStruct_udp, sizeof(RecDataStruct_udp), 0, (SOCKADDR*)&remoteAddr, &nAddrLen);
        if (ret > 0)
        {
            if(strcmp(RecDataStruct_udp.checkcode,"UD1"))///其他情况
            {
                rec=RecDataStruct_udp;
                printf("%s:%d",inet_ntoa(remoteAddr.sin_addr),remoteAddr.sin_port);
                CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)UdpPackResolve,&rec,0,NULL);
            }
            else if(!strcmp(RecDataStruct_udp.checkcode,"UD1"))///表示首次连接
            {
                USER talktouser=FindOnlineUserOrIot(0,RecDataStruct_udp.USERID,0);
                if(talktouser!=NULL)
                {
                    printf("%s:%d",inet_ntoa(remoteAddr.sin_addr),remoteAddr.sin_port);
                    talktouser->USER_socket_udp=remoteAddr;///存入相应链表
                    strcpy(RecDataStruct_udp.checkcode,"UDP");///
                    memset(sendbuf,0,sizeof(sendbag));
                    RecDataStruct_udp.save[99]='\n';
                    memcpy(sendbuf,&RecDataStruct_udp,sizeof(RecDataStruct_udp));
                    //printf("qwqw");
                    sendto(server_sockfd_udp, sendbuf, sizeof(RecDataStruct_udp), 0, (SOCKADDR *)&remoteAddr, nAddrLen);
                }
                else
                {
                    strcpy(RecDataStruct_udp.checkcode,"udp");///小写表示连接失败
                    memset(sendbuf,0,sizeof(sendbag));
                    RecDataStruct_udp.save[99]='\n';
                    memcpy(sendbuf,&RecDataStruct_udp,sizeof(RecDataStruct_udp));
                    sendto(server_sockfd_udp, sendbuf, sizeof(RecDataStruct_udp), 0, (SOCKADDR *)&remoteAddr, nAddrLen);
                }
            }
            else
            {
                printf("\nUDP 异常:%s,用户:%s\n",RecDataStruct_udp.checkcode,RecDataStruct_udp.USERID);
            }
        }
    }
    /*FILE *f=fopen("pic.png","r");
                    strcpy(a->TalktoID,"12345678908");
                    printf("|%d",bitmapfigure(a,f,"PNG"));
                    fclose(f);*/
    return 0;
}

