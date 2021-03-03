#include "../head/SERVER.h"
typedef struct cln_link
{
    CLN* CONNHANDLE;
    LPPER_IO_DATA PERIODATA;
    struct cln_link* next;
} CLN_LINK;

static CLN_LINK Head;
int addConnMemWait4Free(CLN* Conn,LPPER_IO_DATA PerIoData)
{
    CLN_LINK *cursor=&Head;
    while(cursor->next!=NULL)
    {
        cursor=cursor->next;
    }
    CLN_LINK *ConnMemNode = (CLN_LINK*)malloc(sizeof(CLN_LINK));
    if(ConnMemNode==NULL)
    {
        log_error("Malloc Error!");
        return 0;
    }
    ConnMemNode->CONNHANDLE=Conn,
                 ConnMemNode->PERIODATA=PerIoData;
    cursor->next=ConnMemNode;
    return 1;
}

void freeConnMemWait4Free()
{
    CLN_LINK* cursor=&Head;
    while(cursor->next!=NULL)
    {
        if(cursor->next->CONNHANDLE!=NULL)
        {
            if(!pthread_mutex_trylock(&(cursor->next->CONNHANDLE->t)))
            {
                pthread_mutex_unlock(&(cursor->next->CONNHANDLE->t));
                pthread_mutex_destroy(&(cursor->next->CONNHANDLE->t));
                closesocket(cursor->next->CONNHANDLE->remote_socket);
                free(cursor->next->CONNHANDLE);
                if(cursor->next->PERIODATA!=NULL)
                    free(cursor->next->PERIODATA);
                CLN_LINK *temp = cursor->next;
                cursor->next=cursor->next->next;
                log_info("Free1");
                free(temp);
                log_info("Free2");
                continue;
            }
            else
            {
                cursor=cursor->next;
                continue;
            }
        }
        else
        {
            if(cursor->next->PERIODATA!=NULL)
                free(cursor->next->PERIODATA);
            CLN_LINK *temp = cursor->next;
            cursor->next=cursor->next->next;
            free(temp);
        }
    }
}
