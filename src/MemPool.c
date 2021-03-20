#include "../head/SERVER.h"

void ** MemPoolList;
typedef struct mutexStruct
{
    pthread_mutex_t mutex;
} MemPoolMutex;

MemPoolMutex * PoolMutexList;
int MemPoolListSize;
int MemPoolAvailable;
int MallocNodesize;
int InitMemPool(int MemPoolSize,int MallocNodeSize)
{
    MemPoolList = (void**)malloc(sizeof(void*)*MemPoolSize);
    PoolMutexList = (MemPoolMutex*)malloc(sizeof(MemPoolMutex)*MemPoolSize);
    MallocNodesize = MallocNodeSize;
    int i = 0;
    while(i++<MemPoolSize)
    {
        MemPoolList[i-1] = (void*)malloc(MallocNodeSize);
        pthread_mutex_init(&(PoolMutexList[i-1].mutex),NULL);
        if(MemPoolList[i-1] == NULL)
        {
            log_error("MemPool Init Fail");
            return 0;
        }
    }
    MemPoolListSize=i;
    MemPoolAvailable=i;
    return MemPoolSize;
}

void *mallocNode(int *flag/*sup user a index which mem is providing */)
{
    if(MemPoolAvailable==0)
    {
        *flag = -1;
        return (void*)malloc(MallocNodesize);
    }
    int start_index = 0, index = 0, i;
    int j = rand()% (MemPoolListSize/20000);
    start_index = rand()% 20000+(j-1)*20000; //访问的开始地址
    for (i=0; i < MemPoolListSize; i++)
    {
        index = (start_index + i) % MemPoolListSize;
        if (!pthread_mutex_trylock(&(PoolMutexList[index].mutex)))
        {
            *flag = index;
            memset(MemPoolList[index],0,MallocNodesize);
            MemPoolAvailable--;
            return MemPoolList[index];
        }
        else
        {
            continue;
        }
    }
    return NULL;
}

void freeNode(int flag,void *node)
{
    if(flag==-1)
    {
        free(node);
    }
    else
    {
        pthread_mutex_unlock(&(PoolMutexList[flag].mutex));
        MemPoolAvailable++;
    }
}

void freeMemPool()
{
    int i = 0;
    while(i++<MemPoolListSize)
    {
        free(MemPoolList[i-1]);
        pthread_mutex_destroy(&(PoolMutexList[i-1].mutex));
    }
    free(MemPoolList);
    free(PoolMutexList);
}
