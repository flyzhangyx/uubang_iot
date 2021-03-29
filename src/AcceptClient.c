#include"../head/SERVER.h"
int AcceptClientNum = 0;
void Add2MallocConnList(CLN* CONNHANDLE,LPPER_IO_DATA PerIoData)
{
    Con2FreeArg *ConnMalloc = (Con2FreeArg*)malloc(sizeof(Con2FreeArg));
    ConnMalloc->Conn = CONNHANDLE;
    ConnMalloc->PerIoData = PerIoData;
    addConnMemWait4Ping(ConnMalloc);
}
int AcceptClient()
{
    HANDLE completionPort  = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (NULL == completionPort) 	// 创建IO内核对象失败
    {
        log_error("CompleteIO ERR");
        return -1;
    }
    SYSTEM_INFO mySysInfo;
    GetSystemInfo(&mySysInfo);
    for(DWORD i = 0; i < (mySysInfo.dwNumberOfProcessors * 2+2); ++i)
    {
        HANDLE ThreadHandle = CreateThread(NULL, 0, ServerWorkThread, completionPort, 0, NULL);
        if(NULL == ThreadHandle)
        {
            log_error("Create Work Thread Err");
            return -1;
        }
        CloseHandle(ThreadHandle);
    }

    listen(server_sockfd,10);///等待客户端连接请求到达
    while(!isShutDown)
    {
        CLN* CONNHANDLE = (CLN*)malloc(sizeof(CLN));
        if(CONNHANDLE==NULL)
        {
            Sleep(1000);
            continue;
        }
        memset(CONNHANDLE,0,sizeof(CLN));
        int RemoteLen = sizeof(struct sockaddr_in);
        CONNHANDLE->info[1] = 'N';
        CONNHANDLE->info[0] = 'N';
        CONNHANDLE->info[2] = 0;
        if(AcceptClientNum>=10000)
        {
            log_debug("Accept Num Exceed 10000");
            free(CONNHANDLE);
            continue;
        }
        CONNHANDLE->SOCKET = accept(server_sockfd, (struct sockaddr*)&(CONNHANDLE->ADDR), &RemoteLen);
        pthread_mutex_init(&(CONNHANDLE->t),NULL);
        if(SOCKET_ERROR == CONNHANDLE->SOCKET) 	// 接收客户端失败
        {
            log_error("Accept Socket Error" );
            pthread_mutex_destroy(&(CONNHANDLE->t));
            free(CONNHANDLE);
            continue;
        }
        LPPER_IO_DATA PerIoData = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));//Per IO operation exchange data use this struct,put it in the WSARecv func to Rec something
         if(PerIoData==NULL)
        {
            closesocket(CONNHANDLE ->SOCKET);
            pthread_mutex_destroy(&(CONNHANDLE->t));
            free(CONNHANDLE);
            log_error("Malloc PerIOData Fail");
            continue;
        }
        Add2MallocConnList(CONNHANDLE,PerIoData);
        AcceptClientNum++;
        log_debug("Accept %I64d , MemAddr 0x%p",CONNHANDLE->SOCKET,(void*)CONNHANDLE);
        CreateIoCompletionPort((HANDLE)(CONNHANDLE ->SOCKET), completionPort, (ULONG_PTR)CONNHANDLE, 0);//Create relation between CONNHANDLE and COmpletionPort
        memset(PerIoData,0, sizeof(PER_IO_DATA));
        memset(&(PerIoData -> overlapped),0, sizeof(OVERLAPPED));
        PerIoData->WSADATABUF.len = sizeof(UserPacketInterface);
        PerIoData->WSADATABUF.buf = PerIoData->RECBUFFER;
        PerIoData->OpCode= 0;	// read
        DWORD RecvBytes;
        DWORD Flags = 0;
        WSARecv(CONNHANDLE->SOCKET, &(PerIoData->WSADATABUF), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);
    }
    return 1;
}
