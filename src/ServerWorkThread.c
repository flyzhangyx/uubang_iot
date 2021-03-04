#include "../head/SERVER.h"
#ifdef STPOOL
void task_err_handler(struct sttask *ptask, long reasons)
{
    fprintf(stderr, "**ERR: '%s' (%lx)",
            ptask->task_name, reasons);
}
#endif // STPOOL
DWORD WINAPI ServerWorkThread(LPVOID lpParam)
{
    HANDLE CompletionPort = (HANDLE)lpParam;
    DWORD BytesTransferred;
    LPOVERLAPPED lpOverlapped;
    CLN* CONNHANDLE = NULL;//Relation with PerConn is store in this struct
    LPPER_IO_DATA PerIoData = NULL;
    UserPacketInterface RecBuff;
    while(1)
    {
        CONNHANDLE = NULL;
        lpOverlapped = NULL;
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
                    log_error("Cl");//Client close (MITM)
                    closesocket(CONNHANDLE->remote_socket);
                    delete_out_user(CONNHANDLE);
                    if(CONNHANDLE->info[2]==0)
                    {
                        pthread_mutex_destroy(&(CONNHANDLE->t));
                        free(CONNHANDLE);
                        CONNHANDLE=NULL;
                        PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, overlapped);
                        free(PerIoData);//free source
                        PerIoData=NULL;
                    }
                    else
                    {
                        PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, overlapped);
                        Con2FreeArg *Autofree = (Con2FreeArg*)malloc(sizeof(Con2FreeArg));
                        Autofree->Conn = CONNHANDLE;
                        Autofree->PerIoData = PerIoData;
#ifdef STPOOL
                        stpool_add_routine(ThreadPool_ExecuteTask,"CON2FREE",(void*)(struct sttask*)addConnMemWait4Free,task_err_handler,Autofree,NULL);
#else
                        libThreadPoolaskAdd(ThreadPool_ExecuteTask, addConnMemWait4Free, (void*)Autofree);//put into task queue
#endif
                    }
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
            if(0 == BytesTransferred||BytesTransferred>721)
            {
                //log_error("%d Byte Received",(int)BytesTransferred);
                closesocket(CONNHANDLE->remote_socket);
                if(CONNHANDLE->info[2]==0)
                {
                    log_info("CONNHANDLE_INFO[2]:%d",CONNHANDLE->info[2]);
                    pthread_mutex_destroy(&(CONNHANDLE->t));
                    free(CONNHANDLE);
                    CONNHANDLE=NULL;
                    free(PerIoData);//free source
                    PerIoData=NULL;
                }
                else
                {
                    Con2FreeArg *Autofree = (Con2FreeArg*)malloc(sizeof(Con2FreeArg));
                    Autofree->Conn = CONNHANDLE;
                    Autofree->PerIoData = PerIoData;
#ifdef STPOOL
                    stpool_add_routine(ThreadPool_ExecuteTask,"CON2FREE",(void*)(struct sttask*)addConnMemWait4Free,task_err_handler,Autofree,NULL);
#else
                    libThreadPoolaskAdd(ThreadPool_ExecuteTask, addConnMemWait4Free, (void*)Autofree);//put into task queue
#endif
                }
                continue;
            }
            else if(BytesTransferred>0&&BytesTransferred<721)
            {
                CopyRecIotData2Cln(PerIoData->RECBUFFER,CONNHANDLE,BytesTransferred);
                CONNHANDLE->info[1]='Y';
            }
            else
            {
                memset(&RecBuff,0,sizeof(UserPacketInterface));
                memcpy(&RecBuff,PerIoData->RECBUFFER,BytesTransferred);
                CopyUserPacketInterface2Cln(RecBuff,CONNHANDLE);
            }
            if(strstr(CONNHANDLE->checkcode,"ZYX")!=NULL&&CONNHANDLE->info[1]!='Y')
            {
                if(strstr(CONNHANDLE->checkcode,"ZYXX1226")!=NULL)
                {
                    CONNHANDLE->info[1]='Y';
                }
                else if(strstr(CONNHANDLE->checkcode,"ZYXX1227")!=NULL)
                {
                    CONNHANDLE->info[1]='Y';
                }
                else
                {
                    //OLD VERSION
                    char tempBuf[721]="UPD";
                    send(CONNHANDLE->remote_socket,tempBuf,721,0);
                }
            }
            if(CONNHANDLE->info[1]!='Y')
            {
                log_info("[Illegal User] %s:%d",inet_ntoa((CONNHANDLE->ADDR.sin_addr)),CONNHANDLE->ADDR.sin_port);
                closesocket(CONNHANDLE->remote_socket);
                free(CONNHANDLE);
                CONNHANDLE=NULL;
                free(PerIoData);//free source
                PerIoData=NULL;
                continue;
            }
            if(CONNHANDLE->info[2]<100)
            {
                CONNHANDLE->info[2]++;//Task num ++
#ifdef STPOOL
                stpool_add_routine(ThreadPool_ExecuteMsg,"IN",(void*)(struct sttask*)talk,task_err_handler,CONNHANDLE,NULL);
#else
                libThreadPoolaskAdd(ThreadPool_ExecuteMsg, talk, (void*)CONNHANDLE);//put into task queue
#endif
            }
            else
                send(CONNHANDLE->remote_socket,"OOM",4,0);//OUTOFMEM

            memset(&(PerIoData->overlapped), 0,sizeof(OVERLAPPED)); //
            PerIoData->WSADATABUF.len = sizeof(UserPacketInterface);
            PerIoData->WSADATABUF.buf = PerIoData->RECBUFFER;
            PerIoData->OpCode= 0;// read
            DWORD RecvBytes;
            DWORD Flags = 0;
            WSARecv(CONNHANDLE->remote_socket, &(PerIoData->WSADATABUF), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);
        }
    }
    return 0;
}
