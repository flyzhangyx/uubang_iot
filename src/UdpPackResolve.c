#include"../head/SERVER.h"
DWORD WINAPI UdpPackResolve(LPVOID qn)
{
    UserPacketInterface *rec=(UserPacketInterface*)qn;
    char sendbuf[sizeof(UserPacketInterface)]= {0};
    char tag[4];
    ///��ʼ����UDP�����ͣ�������������Ҫ���ͻذ���UDP��
    strncpy(tag,rec->checkcode,4);
    tag[3]='\0';
    if(!strcmp(tag,"PNG"))
    {
        log_info("ddd");///

        ///һ����ɺ�ذ���֪������һ��
        strcpy(rec->checkcode,"UDP");
        memset(sendbuf,0,sizeof(UserPacketInterface));
        rec->save[99]=_HC_;
        memcpy(sendbuf,rec,sizeof(*rec));
        USER talkuser=FindOnlineUserOrIot(0,rec->USERID,0);
        if(talkuser!=NULL)
        {
            log_info("ssss:%s:%d",inet_ntoa(talkuser->USER_socket_udp.sin_addr),talkuser->USER_socket_udp.sin_port);
            sendto(server_sockfd_udp, sendbuf, sizeof(*rec),0, (SOCKADDR*)&(talkuser->USER_socket_udp), sizeof(talkuser->USER_socket_udp));
        }
        else
        {
            return -1;
        }
    }
    else if(!strcmp(tag,"JPG"))
    {
        ///
    }
    return 0;
}
