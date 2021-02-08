#include"../head/SERVER.h"
int newOnlineUserOrIotDev(CLN *a)
{
    USER newuser=NULL;
    int n=0;
    do
    {
        newuser=(USER)malloc(sizeof(struct user));
        n++;
    }
    while(newuser==NULL&&n<100);
    if(n>99)
    {
        printf("can't add newuser");
        return -1;
    }
    strcpy(newuser->USERID,a->USERID);
    strcpy(newuser->USERPASSWORD,a->USERPASSWORD);
    newuser->USER_socket_udp=a->ADDR;
    newuser->USER_socket=a->remote_socket;
    newuser->USERKEY_ID=a->USERKEY_ID;
    if(strlen(a->USERID)<11)
    {
        pthread_mutex_lock(&(onlineIotHead->mute));
        newuser->next=onlineIotHead->next;
        strcpy(newuser->info,a->info);
        onlineIotHead->next=newuser;
        pthread_mutex_unlock(&(onlineIotHead->mute));
        onlineIotHead->OnlineUserNum++;
    }
    else
    {
        pthread_mutex_lock(&(onlineUserHead->mute));
        newuser->next=onlineUserHead->next;
        strcpy(newuser->info,a->info);
        onlineUserHead->next=newuser;
         pthread_mutex_unlock(&(onlineUserHead->mute));
        onlineUserHead->OnlineUserNum++;
    }
    return 1;
}
