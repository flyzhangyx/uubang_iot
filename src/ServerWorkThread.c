#include "../head/SERVER.h"
extern int AcceptClientNum;
#ifdef STPOOL
void task_err_handler(struct sttask *ptask, long reasons)
{
    log_error("**ERR: '%s' (%lx)",ptask->task_name, reasons);
}
#endif // STPOOL
void AddToFreeThread(CLN* CONNHANDLE,LPPER_IO_DATA PerIoData)
{
    Con2FreeArg *Autofree = (Con2FreeArg*)malloc(sizeof(Con2FreeArg));
    Autofree->Conn = CONNHANDLE;
    Autofree->PerIoData = PerIoData;
#ifdef STPOOL
    stpool_add_routine(ThreadPool_ExecuteTask,"ADDCON2FREE_EMPTY",(void*)(struct sttask*)addConnMemWait4Free,task_err_handler,Autofree,NULL);
#else
    libThreadPoolaskAdd(ThreadPool_ExecuteTask, addConnMemWait4Free, (void*)Autofree);//put into task queue
#endif
}

DWORD WINAPI ServerWorkThread(LPVOID lpParam)
{
    HANDLE CompletionPort = (HANDLE)lpParam;
    DWORD BytesTransferred;
    LPOVERLAPPED lpOverlapped;
    CLN* CONNHANDLE = NULL;//Relation with PerConn is store in this struct
    CLN* ARG_CONN = NULL;
    LPPER_IO_DATA PerIoData = NULL;
    UserPacketInterface RecBuff;
    while(!isShutDown)
    {
        CONNHANDLE = NULL;
        lpOverlapped = NULL;
        ARG_CONN = NULL;
        if(!GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&CONNHANDLE, (LPOVERLAPPED*)&lpOverlapped, INFINITE))
        {
            DWORD dwErr = GetLastError();
            if(CONNHANDLE == NULL||NULL==lpOverlapped)
            {
                continue;
            }
            else
            {
                if(dwErr==WAIT_TIMEOUT)
                {
                    log_info("GETIOCPQUEUE TIMEOUT");
                }
                else
                {
                    log_debug("CLOSE BY PEER");//Client close (MITM)
                    closesocket(CONNHANDLE->SOCKET);
                    continue;
                }
            }
        }
        else
        {
            if(lpOverlapped==NULL||CONNHANDLE==NULL)
            {
                log_error("Recv Success, but Mem is Null");
                continue;
            }
            PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, overlapped);
            if(PerIoData==NULL)
            {
                log_error("Recv Success, but PerIoData is Null");
                closesocket(CONNHANDLE->SOCKET);
                continue;
            }
            if(0 == BytesTransferred||BytesTransferred>721)
            {
                closesocket(CONNHANDLE->SOCKET);
                continue;
            }
            else if(BytesTransferred>10&&BytesTransferred<105)
            {
#ifdef MemPool
                int NodeSeq = 0 ;
                ARG_CONN = (CLN*)mallocNode(&NodeSeq);
#else
                ARG_CONN = (CLN*)MemoryPoolAlloc(mp, sizeof(CLN));
#endif
                if(ARG_CONN==NULL)
                {
                    send(CONNHANDLE->SOCKET,"OOE",4,0);//OUTOFMEM
                    log_error("OOE");
                }
                else
                {
#ifdef MemPool
                    ARG_CONN->MemMark = NodeSeq;
#endif // MemPool
                    memcpy(ARG_CONN,CONNHANDLE,sizeof(CLN));
                    CopyRecIotData2Cln(PerIoData->RECBUFFER,ARG_CONN,BytesTransferred);
                    memcpy(CONNHANDLE->checkcode,ARG_CONN->checkcode,17);
                    ARG_CONN->conn = CONNHANDLE;
                    CONNHANDLE->info[1]='Y';
                }
            }
            else
            {
#ifdef MemPool
                int NodeSeq = 0 ;
                ARG_CONN = (CLN*)mallocNode(&NodeSeq);
#else
                ARG_CONN = (CLN*)MemoryPoolAlloc(mp, sizeof(CLN));
#endif
                if(ARG_CONN==NULL)
                {
                    send(CONNHANDLE->SOCKET,"OOE",4,0);//OUTOFMEM
                    log_error("OOE");
                }
                else
                {
#ifdef MemPool
                    ARG_CONN->MemMark = NodeSeq;
#endif // MemPool
                    memcpy(ARG_CONN,CONNHANDLE,sizeof(CLN));
                    ARG_CONN->conn = CONNHANDLE;
                    memset(&RecBuff,0,sizeof(UserPacketInterface));
                    memcpy(&RecBuff,PerIoData->RECBUFFER,BytesTransferred);
                    CopyUserPacketInterface2Cln(RecBuff,ARG_CONN);
                    memcpy(CONNHANDLE->checkcode,ARG_CONN->checkcode,18);
                }
            }
            if(strstr(CONNHANDLE->checkcode,"ZYX")!=NULL&&CONNHANDLE->info[1]!='Y'&&ARG_CONN!=NULL)
            {
                if(strstr(CONNHANDLE->checkcode,"ZYXX1226")!=NULL)
                {
                    CONNHANDLE->info[1]='Y';
                    ARG_CONN->info[1]='Y';
                }
                else if(strstr(CONNHANDLE->checkcode,"ZYXX1227")!=NULL)
                {
                    CONNHANDLE->info[1]='Y';
                    ARG_CONN->info[1]='Y';
                }
                else
                {
                    //OLD VERSION
                    char tempBuf[721]="UPD";
                    send(CONNHANDLE->SOCKET,tempBuf,721,0);
                }
            }
            if(CONNHANDLE->info[1]!='Y')
            {
                log_info("[Illegal User] %s:%d , Con = %I64d ",inet_ntoa((CONNHANDLE->ADDR.sin_addr)),CONNHANDLE->ADDR.sin_port,CONNHANDLE->SOCKET);
                closesocket(CONNHANDLE->SOCKET);
                //AddToFreeThread(CONNHANDLE,PerIoData);
#ifdef MemPool
                freeNode(ARG_CONN->MemMark,ARG_CONN);
#else
                MemoryPoolFree(mp, ARG_CONN);
#endif
                continue;
            }
            if(CONNHANDLE->info[2]<120&&ARG_CONN!=NULL)
            {
                CONNHANDLE->info[2]++;//Task num ++
#ifdef STPOOL
                stpool_add_routine(ThreadPool_ExecuteMsg,"ExcuteMsg",(void*)(struct sttask*)talk,task_err_handler,ARG_CONN,NULL);
#else
                libThreadPoolaskAdd(ThreadPool_ExecuteMsg, talk, (void*)ARG_CONN);//put into task queue
#endif
            }
            else
            {
                send(CONNHANDLE->SOCKET,"OOM",4,0);//OUTOFMEM
#ifdef MemPool
                freeNode(ARG_CONN->MemMark,ARG_CONN);
#else
                MemoryPoolFree(mp, ARG_CONN);
#endif
            }
            memset(&(PerIoData->overlapped), 0,sizeof(OVERLAPPED)); //
            PerIoData->WSADATABUF.len = sizeof(UserPacketInterface);
            PerIoData->WSADATABUF.buf = PerIoData->RECBUFFER;
            PerIoData->OpCode= 0;// read
            DWORD RecvBytes;
            DWORD Flags = 0;
            WSARecv(CONNHANDLE->SOCKET, &(PerIoData->WSADATABUF), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);
        }
    }
    return 0;
}
