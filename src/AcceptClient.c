#include"../head/SERVER.h"
int AcceptClient()
{

    HANDLE completionPort  = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (NULL == completionPort) 	// 创建IO内核对象失败
    {
        printf("\nCompleteIO ERR");
        return -1;
    }
    SYSTEM_INFO mySysInfo;
    GetSystemInfo(&mySysInfo);
    for(DWORD i = 0; i < (mySysInfo.dwNumberOfProcessors * 2+2); ++i)
    {
        HANDLE ThreadHandle = CreateThread(NULL, 0, ServerWorkThread, completionPort, 0, NULL);
        if(NULL == ThreadHandle)
        {
            printf("\nCreate Work Thread Err");
            return -1;
        }
        CloseHandle(ThreadHandle);
    }

    listen(server_sockfd,10);///等待客户端连接请求到达
    while(1)
    {
        CLN* CONNHANDLE = (CLN*)malloc(sizeof(CLN));
        if(CONNHANDLE==NULL)
        {
            continue;
        }
        memset(CONNHANDLE,0,sizeof(CLN));
        int RemoteLen = sizeof(struct sockaddr_in);
        CONNHANDLE->info[1] = 'N';
        CONNHANDLE->remote_socket = accept(server_sockfd, (struct sockaddr*)&(CONNHANDLE->ADDR), &RemoteLen);
        if(SOCKET_ERROR == CONNHANDLE->remote_socket) 	// 接收客户端失败
        {
            printf("Accept Socket Error" );
            free(CONNHANDLE);
            system("pause");
            continue;
        }
        else
        {
            CLN_num++;
            //printf("\naccept num %d client %s:%u\n",CLN_num,inet_ntoa(CONNHANDLE->ADDR.sin_addr),CONNHANDLE->ADDR.sin_port);
        }
        CreateIoCompletionPort((HANDLE)(CONNHANDLE ->remote_socket), completionPort, (ULONG_PTR)CONNHANDLE, 0);//Create relation between CONNHANDLE and COmpletionPort
        LPPER_IO_OPERATION_DATA PerIoData = (LPPER_IO_OPERATION_DATA)malloc(sizeof(PER_IO_OPERATEION_DATA));//Per IO operation exchange data use this struct,put it in the WSARecv func to Rec something
        if(PerIoData==NULL)
        {
            closesocket(CONNHANDLE ->remote_socket);
            free(CONNHANDLE);
            printf("\nMalloc PerIOData Fail");
            continue;
        }
        memset(PerIoData,0, sizeof(PER_IO_OPERATEION_DATA));
        memset(&(PerIoData -> overlapped),0, sizeof(OVERLAPPED));
        PerIoData->WSADATABUF.len = sizeof(sendbag);
        PerIoData->WSADATABUF.buf = PerIoData->RECBUFFER;
        PerIoData->OpCode= 0;	// read
        DWORD RecvBytes;
        DWORD Flags = 0;
        InitRSA(&(CONNHANDLE->key));//Create RSAKey, need srand(time(NULL)) first
        printf("Public Key (%d,%d)\nPrivate Key (%d,%d)\n",CONNHANDLE->key.publicKey,CONNHANDLE->key.commonKey,CONNHANDLE->key.privateKey,CONNHANDLE->key.commonKey);
        WSARecv(CONNHANDLE->remote_socket, &(PerIoData->WSADATABUF), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);
    }
}
