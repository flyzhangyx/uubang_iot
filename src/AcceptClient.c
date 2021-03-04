#include"../head/SERVER.h"
int AcceptClient()
{
    HANDLE completionPort  = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (NULL == completionPort) 	// ����IO�ں˶���ʧ��
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

    listen(server_sockfd,10);///�ȴ��ͻ����������󵽴�
    while(1)
    {
        CLN* CONNHANDLE = (CLN*)malloc(sizeof(CLN));
        if(CONNHANDLE==NULL)
        {
            sleep(1);
            continue;
        }
        memset(CONNHANDLE,0,sizeof(CLN));
        int RemoteLen = sizeof(struct sockaddr_in);
        CONNHANDLE->info[1] = 'N';
        CONNHANDLE->info[0] = 'N';
        CONNHANDLE->info[2] = 0;
        pthread_mutex_init(&(CONNHANDLE->t),NULL);
        CONNHANDLE->remote_socket = accept(server_sockfd, (struct sockaddr*)&(CONNHANDLE->ADDR), &RemoteLen);
        if(SOCKET_ERROR == CONNHANDLE->remote_socket) 	// ���տͻ���ʧ��
        {
            log_error("Accept Socket Error" );
            pthread_mutex_destroy(&(CONNHANDLE->t));
            free(CONNHANDLE);
            continue;
        }
        log_info("Accept %I64d , MemAddr 0x%x",CONNHANDLE->remote_socket,CONNHANDLE);
        CreateIoCompletionPort((HANDLE)(CONNHANDLE ->remote_socket), completionPort, (ULONG_PTR)CONNHANDLE, 0);//Create relation between CONNHANDLE and COmpletionPort
        LPPER_IO_DATA PerIoData = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));//Per IO operation exchange data use this struct,put it in the WSARecv func to Rec something
        if(PerIoData==NULL)
        {
            closesocket(CONNHANDLE ->remote_socket);
            pthread_mutex_destroy(&(CONNHANDLE->t));
            free(CONNHANDLE);
            log_error("Malloc PerIOData Fail");
            continue;
        }
        memset(PerIoData,0, sizeof(PER_IO_DATA));
        memset(&(PerIoData -> overlapped),0, sizeof(OVERLAPPED));
        PerIoData->WSADATABUF.len = sizeof(UserPacketInterface);
        PerIoData->WSADATABUF.buf = PerIoData->RECBUFFER;
        PerIoData->OpCode= 0;	// read
        DWORD RecvBytes;
        DWORD Flags = 0;
        WSARecv(CONNHANDLE->remote_socket, &(PerIoData->WSADATABUF), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);
    }
}
