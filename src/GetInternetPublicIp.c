#include "../head/SERVER.h"
void getNetIp(char *ip)
{
    int flag=0;
    SOCKET sock=socket(AF_INET,SOCK_STREAM,0);;
    char **pptr=NULL;
    struct hostent    *ptr = NULL;
    char destIP[128];
    ptr = gethostbyname("www.flyzhangyx.com");
    if(NULL == ptr)
    {
        perror("gethostbyname error");
        exit(0);
    }
    for(flag=0,pptr=ptr->h_addr_list ; NULL != *pptr ; ++pptr)
    {
        inet_ntop(ptr->h_addrtype,*pptr,destIP,sizeof(destIP));
        SOCKADDR_IN hostAddr;
        hostAddr.sin_addr.S_un.S_addr = inet_addr(destIP);
        hostAddr.sin_family = AF_INET;
        hostAddr.sin_port = htons(81);
        //连接服务器
        if(connect(sock,(SOCKADDR*)&hostAddr,sizeof(SOCKADDR)) == SOCKET_ERROR)
        {
            continue;
        }
        else
        {
            flag=1;
            break;
        }
    }
    if(0 == flag)
    {
        perror("connect failed");
        exit(-13);
    }
    //char szBuffer[1024] = {"GET /getip.php HTTP/1.1\r\nHost:www.flyzhangyx.com\r\nConnection:Close\r\n\r\n"};
    char szBuffer[1024]="GET /getip.php HTTP/1.1\r\nHost: www.flyzhangyx.com \r\nConnection: keep-alive\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n\r\n";

    //向服务器发送数据
    if(send(sock,szBuffer,strlen (szBuffer),0) == SOCKET_ERROR)
        log_info("send error\n");
    //从服务器获得数据
    if(recv(sock,szBuffer,1024,0) == SOCKET_ERROR)
        log_info("recv error\n");
    //output
    char *p = szBuffer;
    flag = 0;
    char temp[20]="";
    int i=0;
    while(*p != ']')
    {
        if(*(p-1) == '[') flag = 1;
        if(flag)
        {
            temp[i]=*p;
            i++;
        }
        p++;
    }
    temp[i]=0;
    log_info("Net Ip address is:%s",temp);
    closesocket(sock);
    if(ip!=NULL)
    {
        memcpy(ip,temp,strlen(temp)+1);
    }
}
