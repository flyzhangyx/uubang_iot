#include "../head/SERVER.h"
typedef struct cln_link
{
    CLN* CONNHANDLE;
    LPPER_IO_DATA PERIODATA;
    int time;
    struct cln_link* next;
} CLN_LINK;

static CLN_LINK Head;
int addConnMemWait4Free(struct sttask *ptask)
{
    WaitForSingleObject(Con2FreeLink_mutex, INFINITE);
    if(ptask==NULL)
    {
        log_error("ThreadPool Err");
        return 0;
    }
    log_info("Add 2 Thread Pool Reason: %s",ptask->task_name);
    Con2FreeArg* arg =(Con2FreeArg*) (ptask->task_arg);
    CLN* Conn = arg->Conn;
    log_info("MemAddr: 0x%x",Conn);
    LPPER_IO_DATA PerIoData=arg->PerIoData;
    CLN_LINK *cursor=&Head;
    while(cursor->next!=NULL)
    {
        cursor=cursor->next;
    }
    CLN_LINK *ConnMemNode = (CLN_LINK*)malloc(sizeof(CLN_LINK));
    if(ConnMemNode==NULL)
    {
        log_error("Malloc Error!");
        ReleaseMutex(Con2FreeLink_mutex);
        free(arg);
        return 0;
    }
    ConnMemNode->CONNHANDLE=Conn;
    ConnMemNode->PERIODATA=PerIoData;
    ConnMemNode->time=0;
    cursor->next=ConnMemNode;
    Head.time++;
    ReleaseMutex(Con2FreeLink_mutex);
    free(arg);
    return 1;
}

void freeConnMemWait4Free()
{
    if(WAIT_TIMEOUT==WaitForSingleObject(Con2FreeLink_mutex, 0))
    {
        log_info("LOCK CONN2BEFREE_LINK FAIL");
        return;
    }
    CLN_LINK* cursor=&Head;
    log_info("CONN2BEFREE %d",cursor->time);
    while(cursor->next!=NULL)
    {
        if(cursor->next->CONNHANDLE!=NULL)
        {
            if(cursor->next->time==1&&cursor->next->CONNHANDLE->info[2]==0)
            {
                pthread_mutex_destroy(&(cursor->next->CONNHANDLE->t));
                log_debug("Free Conn :%I64d  MemAddr :0x%x",cursor->next->CONNHANDLE->remote_socket,cursor->next->CONNHANDLE);
                free(cursor->next->CONNHANDLE);
                cursor->next->CONNHANDLE=NULL;
                if(cursor->next->PERIODATA!=NULL)
                {
                    free(cursor->next->PERIODATA);
                    cursor->next->PERIODATA=NULL;
                }
                CLN_LINK *temp = cursor->next;
                cursor->next=cursor->next->next;
                free(temp);
                temp=NULL;
                if(Head.time>0)
                    Head.time--;
                continue;
            }
            else
            {
                if(cursor->next->CONNHANDLE->info[2]!=0)
                {
                    cursor->next->time=0;
                }
                else
                    cursor->next->time++;
                cursor=cursor->next;
                continue;
            }
        }
        else
        {
            log_error("CON2BEFREE_NULL");
//            if(cursor->next->PERIODATA!=NULL)
//            {
//                free(cursor->next->PERIODATA);
//                cursor->next->PERIODATA=NULL;
//            }
            CLN_LINK *temp = cursor->next;
            cursor->next=cursor->next->next;
            free(temp);
            temp=NULL;
        }
    }
    ReleaseMutex(Con2FreeLink_mutex);
}
