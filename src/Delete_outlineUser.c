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
            if(!strcmp(a->USERID,Tag->USERID))
            {
                if(Tag->isReconnect==1)//RECONNECT
                {
                    Tag->isReconnect = 0;
                    pthread_mutex_unlock(&(onlineUserHead->mute));
                    return 0;
                }
                onlineUserHead->next=Tag->next;
                pthread_mutex_destroy(&(Tag->mutex));
                free(Tag);
                onlineUserHead->OnlineUserNum--;
                pthread_mutex_unlock(&(onlineUserHead->mute));
                return 1;

            }
            else
            {
                while(Tag->next!=NULL)
                {
                    if(!strcmp(a->USERID,Tag->next->USERID))
                    {
                        if(Tag->next->isReconnect==1)//RECONNECT
                        {
                            Tag->next->isReconnect = 0;
                            pthread_mutex_unlock(&(onlineUserHead->mute));
                            return 0;
                        }
                        Tag->next=Tag->next->next;
                        pthread_mutex_destroy(&(Tag->next->mutex));
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
                pthread_mutex_destroy(&(Tag->mutex));
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
                        pthread_mutex_destroy(&(Tag->next->mutex));
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
