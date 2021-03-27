#include "../head/SERVER.h"
#define MAX_PING_TIME 1
void task_err_handler2(struct sttask *ptask, long reasons)
{
    log_error("**ERR: '%s' (%lx)",ptask->task_name, reasons);
}
typedef struct cln_link
{
    CLN* CONNHANDLE;
    LPPER_IO_DATA PERIODATA;
    int time;
    int left_time;
    struct cln_link* next;
} CLN_LINK;
extern int AcceptClientNum;
static CLN_LINK Head;
static CLN_LINK MallocHead;

int addConnMemWait4Free(struct sttask *ptask)
{
    WaitForSingleObject(Con2FreeLink_mutex, INFINITE);
    if(ptask==NULL)
    {
        log_error("ThreadPool Err");
        return 0;
    }
    log_debug("Add 2 Thread Pool Reason: %s",ptask->task_name);
    Con2FreeArg* arg =(Con2FreeArg*) (ptask->task_arg);
    CLN* Conn = arg->Conn;
    log_debug("MemAddr: 0x%x",Conn);
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
    ConnMemNode->left_time=0;
    ConnMemNode->next=NULL;
    cursor->next=ConnMemNode;
    Head.time++;
    free(arg);
    ReleaseMutex(Con2FreeLink_mutex);
    return 1;
}

int addConnMemWait4Ping(Con2FreeArg* arg)
{
    if(arg==NULL)
    {
        log_error("Add Conn 4 Ping Err");
        return 0;
    }
    CLN* Conn = arg->Conn;
    LPPER_IO_DATA PerIoData=arg->PerIoData;
    CLN_LINK *cursor=&MallocHead;
    while(cursor->next!=NULL)
    {
        cursor=cursor->next;
    }
    CLN_LINK *ConnMemNode = (CLN_LINK*)malloc(sizeof(CLN_LINK));
    if(ConnMemNode==NULL)
    {
        log_error("Malloc Error");
        free(arg);
        return 0;
    }
    ConnMemNode->CONNHANDLE=Conn;
    ConnMemNode->PERIODATA=PerIoData;
    ConnMemNode->time=0;
    ConnMemNode->left_time=0;
    ConnMemNode->next=NULL;
    cursor->next=ConnMemNode;
    MallocHead.time++;
    free(arg);
    return 1;
}

void PingMallocConnList()
{
    char HBA[100]="HBA";
    time_t now_time;
    time(&now_time);
    char time_now[20];
    strftime(time_now,80,"%Y-%m-%d %X",localtime(&now_time));
    strcat(HBA,time_now);
    int HBAlen = strlen(HBA);
    HBA[HBAlen+1]=_HC_;
    CLN_LINK* cursor=&MallocHead;
    //log_info("CONN2BEPING %d",cursor->time);
    int len = 0;
    while(cursor->next!=NULL)
    {
        if(cursor->next->CONNHANDLE!=NULL)
        {
            if(!pthread_mutex_trylock(&(cursor->next->CONNHANDLE->t)))
            {
                len = send(cursor->next->CONNHANDLE->SOCKET,HBA,HBAlen+2,0);
                if(len==SOCKET_ERROR||len==0)
                {
                    cursor->next->time++;
                }
                pthread_mutex_unlock(&(cursor->next->CONNHANDLE->t));
            }
        }
        else
        {
            CLN_LINK *temp = cursor->next;
            cursor->next=cursor->next->next;
            free(temp);
            temp=NULL;
            continue;
        }
        if(cursor->next->time>MAX_PING_TIME)
        {
            Con2FreeArg *Autofree = (Con2FreeArg*)malloc(sizeof(Con2FreeArg));
            Autofree->Conn = cursor->next->CONNHANDLE;
            Autofree->PerIoData = cursor->next->PERIODATA;
#ifdef STPOOL
            stpool_add_routine(ThreadPool_ExecuteTask,"ADDCON2FREE_PING",(void*)(struct sttask*)addConnMemWait4Free,task_err_handler2,Autofree,NULL);
#else
            libThreadPoolaskAdd(ThreadPool_ExecuteTask, addConnMemWait4Free, (void*)Autofree);//put into task queue
#endif
            CLN_LINK *temp = cursor->next;
            cursor->next=cursor->next->next;
            free(temp);
            temp=NULL;
            MallocHead.time--;
        }
        else
            cursor=cursor->next;
    }
}

void freeConnMemWait4Free()
{
    if(WAIT_TIMEOUT==WaitForSingleObject(Con2FreeLink_mutex, 0))
    {
        //log_info("LOCK CONN2BEFREE_LINK FAIL");
        return;
    }
    CLN_LINK* cursor=&Head;
    //log_info("CONN2BEFREE %d",cursor->time);
    while(cursor->next!=NULL)
    {
        if(cursor->next->CONNHANDLE!=NULL)
        {
            if((cursor->next->time==1&&cursor->next->CONNHANDLE->info[2]==0)||(cursor->next->left_time>=100))
            {
                log_debug("Free Conn :%I64d  MemAddr :0x%x",cursor->next->CONNHANDLE->SOCKET,cursor->next->CONNHANDLE);
                delete_out_user(cursor->next->CONNHANDLE);
                closesocket(cursor->next->CONNHANDLE->SOCKET);
                pthread_mutex_destroy(&(cursor->next->CONNHANDLE->t));
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
                AcceptClientNum--;
                if(Head.time>0)
                    Head.time--;
                continue;
            }
            else
            {
                if(cursor->next->CONNHANDLE->info[2]!=0)
                {
                    cursor->next->time=0;
                    cursor->next->left_time++;
                }
                else
                    cursor->next->time++;
                cursor=cursor->next;
                continue;
            }
        }
        else
        {
            log_debug("CON2BEFREE_NULL");
            CLN_LINK *temp = cursor->next;
            cursor->next=cursor->next->next;
            free(temp);
            temp=NULL;
        }
    }
    ReleaseMutex(Con2FreeLink_mutex);
}
