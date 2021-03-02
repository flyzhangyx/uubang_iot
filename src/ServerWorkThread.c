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
                    //Client close (MITM)
                    closesocket(CONNHANDLE->remote_socket);
                    delete_out_user(CONNHANDLE);
                    if(CONNHANDLE!=NULL)free(CONNHANDLE);
                    if(lpOverlapped!=NULL)
                    {
                        PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, overlapped);
                        free(PerIoData);//free source
                    }
                    continue;
                }
            }
        }
        else
        {
            PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, overlapped);
            if(0 == BytesTransferred||BytesTransferred>721)
            {
                log_debug("%d Byte Received",(int)BytesTransferred);
                if(CONNHANDLE!=NULL)//Continue to receiving data
                {
                    /*memset(&(PerIoData->overlapped), 0,sizeof(OVERLAPPED));
                    PerIoData->WSADATABUF.len = sizeof(UserPacketInterface);
                    PerIoData->WSADATABUF.buf = PerIoData->RECBUFFER;
                    PerIoData->OpCode= 0;	// read
                    DWORD RecvBytes;
                    DWORD Flags = 0;
                    WSARecv(CONNHANDLE->remote_socket, &(PerIoData->WSADATABUF), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);*/
                    closesocket(CONNHANDLE->remote_socket);
                    free(CONNHANDLE);
                    if(lpOverlapped!=NULL)
                    {
                        PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, overlapped);
                        free(PerIoData);//free source
                    }
                }
                continue;
            }
            else if(CONNHANDLE!=NULL&&PerIoData!=NULL)
            {
                if(BytesTransferred>0&&BytesTransferred<721)
                {
                    log_debug("%ld",BytesTransferred);
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
                    {   //OLD VERSION
                        char tempBuf[721]="UPD";
                        send(CONNHANDLE->remote_socket,tempBuf,721,0);
                    }
                }
                if(CONNHANDLE->info[1]!='Y')
                {
                    closesocket(CONNHANDLE->remote_socket);
                    free(CONNHANDLE);
                    if(lpOverlapped!=NULL)
                    {
                        PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, overlapped);
                        free(PerIoData);//free source
                    }
                    continue;
                }
#ifdef STPOOL
                stpool_add_routine(ThreadPool,"IN",(void*)(struct sttask*)talk,task_err_handler,CONNHANDLE,NULL);
#else
                libThreadPool_TaskAdd(ThreadPool, talk, (void*)CONNHANDLE);//put into task queue
#endif
                memset(&(PerIoData->overlapped), 0,sizeof(OVERLAPPED)); //
                PerIoData->WSADATABUF.len = sizeof(UserPacketInterface);
                PerIoData->WSADATABUF.buf = PerIoData->RECBUFFER;
                PerIoData->OpCode= 0;// read
                DWORD RecvBytes;
                DWORD Flags = 0;
                WSARecv(CONNHANDLE->remote_socket, &(PerIoData->WSADATABUF), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);
            }
            else
            {
                ;
            }
        }
    }
    return 0;
}
