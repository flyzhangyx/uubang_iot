#include"../head/SERVER.h"
int delete_out_user(CLN *a)
{
    if(strlen(a->USERID)<6)
    {
        return 0;
    }
    if(strlen(a->USERID)>=11)
    {
        pthread_mutex_lock(&(onlineUserHead->mute));
        USER Tag= onlineUserHead->next;
        if(Tag==NULL)
        {
            pthread_mutex_unlock(&(onlineUserHead->mute));
            return 0;
        }
        else
        {
            if(!strcmp(a->USERID,Tag->USERID)&&(a->SOCKET==Tag->USER_SOCKET))
            {
                onlineUserHead->next=Tag->next;
                free(Tag);
                onlineUserHead->OnlineUserNum--;
                pthread_mutex_unlock(&(onlineUserHead->mute));
                return 1;
            }
            else
            {
                while(Tag->next!=NULL)
                {
                    if(!strcmp(a->USERID,Tag->next->USERID)&&(a->SOCKET==Tag->next->USER_SOCKET))
                    {
                        Tag->next=Tag->next->next;
                        free(Tag->next);
                        onlineUserHead->OnlineUserNum--;
                        pthread_mutex_unlock(&(onlineUserHead->mute));
                        return 1;
                    }
                    else
                    {
                        Tag=Tag->next;
                    }
                }
                pthread_mutex_unlock(&(onlineUserHead->mute));
                return 0;
            }
        }
    }
    else
    {
        return -1;/////////////////For alpha test
        pthread_mutex_lock(&(onlineIotHead->mute));
        USER Tag= onlineIotHead->next;
        if(Tag==NULL)
        {
            pthread_mutex_unlock(&(onlineIotHead->mute));
            return 0;
        }
        else
        {
            if(!strcmp(a->USERID,Tag->USERID))
            {
                onlineIotHead->next=Tag->next;
                free(Tag);
                onlineIotHead->OnlineUserNum--;
                pthread_mutex_unlock(&(onlineIotHead->mute));
                return 1;
            }
            else
            {
                while(Tag->next!=NULL)
                {
                    if(!strcmp(a->USERID,Tag->next->USERID))
                    {
                        Tag->next=Tag->next->next;
                        free(Tag->next);
                        onlineIotHead->OnlineUserNum--;
                        pthread_mutex_unlock(&(onlineIotHead->mute));
                        return 1;
                    }
                    else
                    {
                        Tag=Tag->next;
                    }
                }
                pthread_mutex_unlock(&(onlineIotHead->mute));
                return 0;
            }
        }
    }
}
