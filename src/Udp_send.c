#include"../head/SERVER.h"
DWORD WINAPI fun(LPVOID qn)
{
    char sendbuf[sizeof(UserPacketInterface)]= {0};
    SOCKADDR_IN remoteAddr;
    UserPacketInterface RecDataStruct_udp,rec;
    int nAddrLen = sizeof(remoteAddr);
    while(1)
    {
        int ret = recvfrom(server_sockfd_udp,(char*)&RecDataStruct_udp, sizeof(RecDataStruct_udp), 0, (SOCKADDR*)&remoteAddr, &nAddrLen);
        if (ret > 0)
        {
            if(strcmp(RecDataStruct_udp.checkcode,"UD1"))///�������
            {
                rec=RecDataStruct_udp;
                log_info("%s:%d",inet_ntoa(remoteAddr.sin_addr),remoteAddr.sin_port);
                CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)UdpPackResolve,&rec,0,NULL);
            }
            else if(!strcmp(RecDataStruct_udp.checkcode,"UD1"))///��ʾ�״�����
            {
                USER talktouser=FindOnlineUserOrIot(0,RecDataStruct_udp.USERID,0);
                if(talktouser!=NULL)
                {
                    log_info("%s:%d",inet_ntoa(remoteAddr.sin_addr),remoteAddr.sin_port);
                    talktouser->USER_socket_udp=remoteAddr;///������Ӧ����
                    strcpy(RecDataStruct_udp.checkcode,"UDP");///
                    memset(sendbuf,0,sizeof(UserPacketInterface));
                    RecDataStruct_udp.save[99]=_HC_;
                    memcpy(sendbuf,&RecDataStruct_udp,sizeof(RecDataStruct_udp));
                    //log_info("qwqw");
                    sendto(server_sockfd_udp, sendbuf, sizeof(RecDataStruct_udp), 0, (SOCKADDR *)&remoteAddr, nAddrLen);
                }
                else
                {
                    strcpy(RecDataStruct_udp.checkcode,"udp");///Сд��ʾ����ʧ��
                    memset(sendbuf,0,sizeof(UserPacketInterface));
                    RecDataStruct_udp.save[99]=_HC_;
                    memcpy(sendbuf,&RecDataStruct_udp,sizeof(RecDataStruct_udp));
                    sendto(server_sockfd_udp, sendbuf, sizeof(RecDataStruct_udp), 0, (SOCKADDR *)&remoteAddr, nAddrLen);
                }
            }
            else
            {
                log_info("\nUDP �쳣:%s,�û�:%s\n",RecDataStruct_udp.checkcode,RecDataStruct_udp.USERID);
            }
        }
    }
    /*FILE *f=fopen("pic.png","r");
                    strcpy(a->TalktoID,"12345678908");
                    log_info("|%d",bitmapfigure(a,f,"PNG"));
                    fclose(f);*/
    return 0;
}
