#include"../head/SERVER.h"
SOCKET LocalSocket;
#ifdef STPOOL
void task_err_handler1(struct sttask *ptask, long reasons)
{
    fprintf(stderr, "**ERR: '%s' (%lx)",
            ptask->task_name, reasons);
}


#ifdef STPOOL
void HandleTopServerMsg(struct sttask *ptask)
#else
int talk(LPVOID b)
#endif
{
    int len;
    UserPacketInterface SendDataStruct;
    char sendbuf[721]="";
    IotPacketInterface IotSendDataStruct;
    char Iotsendbuf[200]="";
#ifdef STPOOL
    if(ptask->task_arg==NULL)
    {
        log_error("ARGUE ERR");
        return ;
    }
    CLN* a=(CLN*)ptask->task_arg;
#else
    CLN* a = (CLN*)b;
#endif
    log_error("%s",a->data);
    if(strlen(a->TalktoID)<11)
    {
        USER Temp = FindOnlineUserOrIot(10,a->TalktoID,0);
        if(Temp!=NULL)
        {

        }
    }
    else
    {
        USER Temp = FindOnlineUserOrIot(0,a->TalktoID,0);
        if(Temp!=NULL)
        {
            memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
            memset(sendbuf, 0, sizeof(UserPacketInterface));
            sprintf(SendDataStruct.DATA, "%s",a->data);
            sprintf(SendDataStruct.REUSERPASSWORD, "%s",a->REUSERPASSWORD);
            sprintf(SendDataStruct.save, "%s",a->info);
            sprintf(SendDataStruct.TalktoID, "%s",a->TalktoID);
            sprintf(SendDataStruct.USERID, "%s",a->USERID);
            sprintf(SendDataStruct.USERPASSWORD, "%s",a->USERPASSWORD);
            sprintf(SendDataStruct.checkcode, "%s",a->checkcode);
            SendDataStruct.save[99] = '\n';
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = send(Temp->USER_socket, sendbuf, sizeof(UserPacketInterface), 0);
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(Temp->USER_socket);
            }
        }
    }

    free(a);
}
#endif // STPOOL
DWORD WINAPI ReceiveMsgFromTopServer()
{
    char buf[sizeof(UserPacketInterface)] = "";
    int len;
    UserPacketInterface pack;
    while(1)
    {
        CLN *CONNHANDLE = (CLN*)malloc(sizeof(CLN));
        memset(buf,0,sizeof(UserPacketInterface));
        len = recv(LocalSocket,buf,sizeof(UserPacketInterface),0);
        if(len<0)
        {
            log_info("Receive From TopServer Error!");
            send(LocalSocket,"123",3,0);
            Sleep(1000);
            continue;
        }
        else
        {
            memcpy(&pack,buf,sizeof(UserPacketInterface));
            memset(CONNHANDLE,0,sizeof(CLN));
            CopyUserPacketInterface2Cln(pack,CONNHANDLE);
#ifdef STPOOL
            stpool_add_routine(ThreadPool_ExecuteMsg,"IN",(void*)(struct sttask*)HandleTopServerMsg,task_err_handler1,CONNHANDLE,NULL);
#else
            libThreadPoolaskAdd(ThreadPool_ExecuteMsg, HandleTopServerMsg, (void*)CONNHANDLE);//put into task queue
#endif
        }
    }
}
void ConnectToTopServer()
{
    struct sockaddr_in addr;
    LocalSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ERROR == LocalSocket)
    {
        log_error("socket failed");
        return ;
    }
    memset(&addr, 0, sizeof(addr)); //初始化addr
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4000);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");// 你连接需要服务端的ip
    int addrlen = sizeof(addr);
    int ret = connect(LocalSocket, (struct sockaddr *)&addr, addrlen);//实现连接服务端
    if(ret<0)
    {
        log_error("Connect To TopServer Error!");
        return;
    }
    CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ReceiveMsgFromTopServer,NULL,0,NULL);
}
int Send2OnlineUserViaTopServer(CLN a)
{
    UserPacketInterface SendBuf;
    char Send[sizeof(UserPacketInterface)]="";
    memset(&SendBuf,0,sizeof(UserPacketInterface));
    sprintf(SendBuf.checkcode,"%s",a.checkcode);
    sprintf(SendBuf.DATA,"%s",a.data);
    sprintf(SendBuf.REUSERPASSWORD,"%s",a.REUSERPASSWORD);
    sprintf(SendBuf.save,"%s",a.info);
    SendBuf.save[99]='\n';
    sprintf(SendBuf.TalktoID,"%s",a.TalktoID);
    sprintf(SendBuf.USERID,"%s",a.USERID);
    sprintf(SendBuf.USERPASSWORD,"%s",a.USERPASSWORD);
    memcpy(Send,(char*)&SendBuf,sizeof(UserPacketInterface));
    int len = send(LocalSocket,Send,sizeof(UserPacketInterface),0);
    if(len==0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
