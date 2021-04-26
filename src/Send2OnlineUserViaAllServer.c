#include"../head/SERVER.h"
#define SEND_STO() do{memset(&SendDataStruct, 0, sizeof(UserPacketInterface));\
        memset(sendbuf, 0, sizeof(UserPacketInterface));\
        strcpy(SendDataStruct.checkcode, "STO");\
        SendDataStruct.save[99] = _HC_;\
        memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));\
        len = send(temp->USER_SOCKET, sendbuf, sizeof(UserPacketInterface), 0);\
        if (len == SOCKET_ERROR || len == 0)\
        {\
            closesocket(temp->USER_SOCKET);\
            return;\
        }\
        }while(0)

SOCKET LocalSocket;
int isConnected;
#ifdef STPOOL
void task_err_handler1(struct sttask *ptask, long reasons)
{
    log_error("**ERR: '%s' (%lx)",ptask->task_name, reasons);
}


#ifdef STPOOL
void HandleTopServerMsg(struct sttask *ptask)
#else
int talk(LPVOID b)
#endif
{
    int len;
    UserPacketInterface SendDataStruct;
    char sendbuf[sizeof(UserPacketInterface)]="";
    IotPacketInterface IotSendDataStruct;
    char Iotsendbuf[sizeof(IotPacketInterface)]="";
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
    int opCode = 0;
    char tag[4];
    if(strlen(a->checkcode)==2)
    {
        opCode = atoi(a->checkcode);
    }
    else
    {
        memset(tag,0,4);
        memcpy(tag,a->checkcode,3);
        opCode = DJBHash(tag, 3);
    }
    CLN ReUseCLN;
    /*******************************/
    switch (opCode)
    {
    case 70235://STO
    {
        USER temp = FindOnlineUserOrIot(0,a->TalktoID,0);
        if(temp==NULL)
            return;
        SEND_STO();
        ReUseCLN.SOCKET = temp->USER_SOCKET;//delete user by ID&SOCKET
        sprintf(ReUseCLN.USERID,"%s",temp->USERID);
        delete_out_user(&ReUseCLN);
        break;
    }
    case 0:
    {
        USER tmp = FindOnlineUserOrIot(10,a->TalktoID,0.);
        //log_debug("%x",tmp);
        if(tmp == NULL)
            return;
        memset(&IotSendDataStruct,0,sizeof(IotSendDataStruct));
        sprintf(IotSendDataStruct.opCode,"%s","00");
        IotSendDataStruct.SeqNum[0] = 0;//For other server ctrl ,not local
        Stringcut(a->data,0,199,IotSendDataStruct.payLoad);
        Encrypt(IotSendDataStruct.payLoad,strlen(IotSendDataStruct.payLoad),tmp->CONNHANDLE->Pin,IotSendDataStruct.payLoad);
        memcpy(Iotsendbuf,&IotSendDataStruct,sizeof(IotPacketInterface));
        Iotsendbuf[sizeof(IotPacketInterface)-1] = _HC_;
        len=SyncSend(tmp->USER_SOCKET,Iotsendbuf,sizeof(IotPacketInterface),&(tmp->CONNHANDLE->t));
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(tmp->USER_SOCKET);
        }
    }
    break;
    }
    free(a);
}
#endif // STPOOL
DWORD WINAPI ReceiveMsgFromTopServer()
{
    char buf[sizeof(UserPacketInterface)] = "";
    int len;
    UserPacketInterface pack;
    CLN *CONNHANDLE;
    while(!isShutDown&&isConnected)
    {
        CONNHANDLE = (CLN*)malloc(sizeof(CLN));
        memset(buf,0,sizeof(UserPacketInterface));
        len = recv(LocalSocket,buf,sizeof(UserPacketInterface),0);
        if(len<0)
        {
            log_error("Receive From TopServer Error!");
            len = send(LocalSocket,"HBA",3,0);
            if(len==0||len==SOCKET_ERROR)
            {
                closesocket(LocalSocket);
                isConnected = 0;
            }
            else
            {
                Sleep(1000);
                continue;
            }
        }
        else
        {
            log_debug("%s",buf);
            memcpy(&pack,buf,sizeof(UserPacketInterface));
            memset(CONNHANDLE,0,sizeof(CLN));
            CopyUserPacketInterface2Cln(pack,CONNHANDLE);
#ifdef STPOOL
            stpool_add_routine(ThreadPool_ExecuteTask,"TopServerMsg",(void*)(struct sttask*)HandleTopServerMsg,task_err_handler1,CONNHANDLE,NULL);
#else
            libThreadPoolaskAdd(ThreadPool_ExecuteTask, HandleTopServerMsg, (void*)CONNHANDLE);//put into task queue
#endif
        }
    }
    return 1;
}
void ConnectToTopServer()
{
    closesocket(LocalSocket);
    isConnected = 0;
    if(isConnected)
        return;
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
    addr.sin_addr.s_addr = inet_addr("47.106.207.241");// 你连接需要服务端的ip
    int addrlen = sizeof(addr);
    int ret = connect(LocalSocket, (struct sockaddr *)&addr, addrlen);//实现连接服务端
    if(ret<0)
    {
        log_error("Connect To TopServer Error!");
        return;
    }
    CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ReceiveMsgFromTopServer,NULL,0,NULL));
    isConnected = 1;
}
int Send2TopServerTest()
{
    return send(LocalSocket,"HBA",4,0);
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
    SendBuf.save[99]=_HC_;
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
