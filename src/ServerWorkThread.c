#include "../head/SERVER.h"
#ifdef STPOOL
void task_err_handler(struct sttask *ptask, long reasons)
{
    fprintf(stderr, "**ERR: '%s' (%lx)\n",
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
    sendbag RecBuff;
    while(1)
    {
        CONNHANDLE = NULL;
        if(!GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&CONNHANDLE, (LPOVERLAPPED*)&lpOverlapped, INFINITE))
        {
            if(NULL==lpOverlapped)
            {
                continue;
            }
            DWORD dwErr = GetLastError();
            if(CONNHANDLE == NULL)
            {
                continue;
            }
            else
            {
                if(dwErr==WAIT_TIMEOUT)
                {
                    printf("\nGETIOCPQUEUE TIMEOUT\n");
                }
                continue;
            }
        }
        else
        {
            PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(lpOverlapped, PER_IO_DATA, overlapped);
            if(0 == BytesTransferred||BytesTransferred>721)
            {
                continue;
            }
            else if(CONNHANDLE!=NULL&&PerIoData!=NULL)
            {
                memset(&RecBuff,0,sizeof(sendbag));
                memcpy(&RecBuff,PerIoData->RECBUFFER,BytesTransferred);
                CopySendbag2Cln(RecBuff,CONNHANDLE);
                ///*********************
                // 为下一个重叠调用建立单I/O操作数据
                memset(&(PerIoData->overlapped), 0,sizeof(OVERLAPPED)); // 清空内存
                PerIoData->WSADATABUF.len = sizeof(sendbag);
                PerIoData->WSADATABUF.buf = PerIoData->RECBUFFER;
                PerIoData->OpCode= 0;	// read
                DWORD RecvBytes;
                DWORD Flags = 0;
                if(strstr(CONNHANDLE->checkcode,"ZYX")!=NULL&&CONNHANDLE->info[1]!='Y')
                {
                    if(strstr(CONNHANDLE->checkcode,"ZYXX1226")!=NULL)
                    {
                        CONNHANDLE->info[1]='Y';
                    }
                    else
                    {
//                        printf("\nOLD VERSION CLIENT:|%s\n",inet_ntoa(CONNHANDLE->ADDR.sin_addr));
//                        memset(&RecDataStruct,0,sizeof(sendbag));
//                        memset(sendbuf,0,sizeof(sendbag));
//                        strcpy(RecDataStruct.checkcode,"UPD");
//                        strcpy(RecDataStruct.DATA,app_version);
//                        RecDataStruct.save[99]='\n';
//                        memcpy(sendbuf,&RecDataStruct,sizeof(RecDataStruct));
//                        len=send(c,sendbuf,sizeof(sendbag),0);
                        continue;
                    }
                }
                if(CONNHANDLE->info[1]!='Y')
                {
                    closesocket(CONNHANDLE->remote_socket);
                    free(CONNHANDLE);
                    continue;
                }
#ifdef STPOOL
                stpool_add_routine(ThreadPool,"IN",talk,task_err_handler,CONNHANDLE,NULL);
#else
                libThreadPool_TaskAdd(ThreadPool, talk, (void*)CONNHANDLE);//put into task queue
#endif
                WSARecv(CONNHANDLE->remote_socket, &(PerIoData->WSADATABUF), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);
            }
        }
    }
    return 0;
}
