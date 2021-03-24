#include "../head/SERVER.h"


typedef struct mutexStruct
{
    pthread_mutex_t mutex;
} MemPoolMutex;
void ** MemPoolList;
MemPoolMutex * PoolMutexList;
int MemPoolListSize;
int MemPoolAvailable;
int MallocNodesize;
int InitMemPool(int MemPoolSize,int MallocNodeSize)
{
//    MemPoolList = (void**)malloc(sizeof(void*)*MemPoolSize);
//    PoolMutexList = (MemPoolMutex*)malloc(sizeof(MemPoolMutex)*MemPoolSize);
//    MallocNodesize = MallocNodeSize;
//    int i = 10000;
//    int i = 0;
//    while(i<MemPoolSize)
//    {
//        MemPoolList[i] = (void*) malloc(MallocNodeSize);
//        if(MemPoolList[i] == NULL||pthread_mutex_init(&(PoolMutexList[i].mutex),NULL))
//        {
//            log_error("MemPool Init Fail");
//            return 0;
//        }
//        i++;
//    }
//    MemPoolListSize=i;
    MemPoolAvailable=0;
    return MemPoolSize;
}

void *mallocNode(int *flag/*sup user a index which mem is providing */)
{
//    int start_index = 0, index = 0, i;
//    int j = rand()% (MemPoolListSize/2000);
//    start_index = rand()% 2000+(j)*2000; //访问的开始地址
//    for (i=0; i < MemPoolListSize; i++)
//    {
//        index = (start_index + i) % MemPoolListSize;
//        if (!pthread_mutex_trylock(&(PoolMutexList[index].mutex)))
//        {
//            if(index>=200000||index<=0)
//            {
//                continue;
//                log_error("%d",index);
//            }
//            *flag = index;
//            log_info("%d",index);
//            memset(MemPoolList[index],0,MallocNodesize);
//             log_info("%d",index);
//            MemPoolAvailable--;
//            return MemPoolList[index];
//        }
//    }
    *flag = -1;
    InterlockedIncrement((LPLONG) &(MemPoolAvailable));
    return (void*)malloc(sizeof(CLN));
}

void freeNode(int flag,void *node)
{
//    if(node==NULL&&flag>=MemPoolListSize&&flag<-1)
//    {
//        log_error("Free Overflow MemPool");
//        return;
//    }
    //if(flag==-1)
//    {
        free(node);
        InterlockedDecrement((LPLONG) &(MemPoolAvailable));
        //   }
//    else
//    {
//        pthread_mutex_unlock(&(PoolMutexList[flag].mutex));
//        MemPoolAvailable++;
//    }
}

void freeMemPool()
{
//    int i = 0;
//    while(i++<MemPoolListSize)
//    {
//        free(MemPoolList[i-1]);
//        pthread_mutex_destroy(&(PoolMutexList[i-1].mutex));
//    }
//    free(MemPoolList);
//    free(PoolMutexList);
}
