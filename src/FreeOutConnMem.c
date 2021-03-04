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
    if(ptask==NULL)
    {
        log_error("ThreadPool Err");
        return 0;
    }
    log_info("Add 2 Thread Pool :%s",ptask->task_name);
    pthread_mutex_lock(&Con2FreeLink_mutex);
    Con2FreeArg* arg =(Con2FreeArg*) (ptask->task_arg);
    CLN* Conn = arg->Conn;
    LPPER_IO_DATA PerIoData=arg->PerIoData;
    CLN_LINK *cursor=&Head;
    cursor->time++;
    while(cursor->next!=NULL)
    {
        cursor=cursor->next;
    }
    CLN_LINK *ConnMemNode = (CLN_LINK*)malloc(sizeof(CLN_LINK));
    if(ConnMemNode==NULL)
    {
        log_error("Malloc Error!");
        pthread_mutex_unlock(&Con2FreeLink_mutex);
        free(arg);
        return 0;
    }
    ConnMemNode->CONNHANDLE=Conn;
    ConnMemNode->PERIODATA=PerIoData;
    ConnMemNode->time=0;
    cursor->next=ConnMemNode;
    pthread_mutex_unlock(&Con2FreeLink_mutex);
    free(arg);
    return 1;
}

void freeConnMemWait4Free()
{
    CLN_LINK* cursor=&Head;
    log_info("CONN2BEFREE %d",Head.time);
    while(cursor->next!=NULL)
    {
        if(cursor->next->CONNHANDLE!=NULL)
        {
            if(cursor->next->time==1&&cursor->next->CONNHANDLE->info[2]==0)
            {
                pthread_mutex_destroy(&(cursor->next->CONNHANDLE->t));
                log_info("Free Conn :%I64d",cursor->next->CONNHANDLE->remote_socket);
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
            if(cursor->next->PERIODATA!=NULL)
            {
                free(cursor->next->PERIODATA);
                cursor->next->PERIODATA=NULL;
            }
            CLN_LINK *temp = cursor->next;
            cursor->next=cursor->next->next;
            free(temp);
            temp=NULL;
        }
    }
}
