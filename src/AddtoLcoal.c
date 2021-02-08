#include"../head/SERVER.h"
int AddtoLocal(CLN *a)
{
    USER newuser=NULL;
    int n=0;
    do
    {
        newuser=(USER)malloc(sizeof(struct user));
        memset(newuser,0,sizeof(struct user));
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
    newuser->USERKEY_ID = a->USERKEY_ID;
    if(strlen(a->USERID)<11)
    {
        pthread_mutex_lock(&(RegistedIotHead->mute));
        newuser->next=RegistedIotHead->next;
        strcpy(newuser->info,a->info);
        RegistedIotHead->next=newuser;
        pthread_mutex_unlock(&(RegistedIotHead->mute));
        RegistedIotHead->OnlineUserNum++;
    }
    else
    {
        pthread_mutex_lock(&(RegistedUserHead->mute));
        newuser->next=RegistedUserHead->next;
        RegistedUserHead->next=newuser;
        strcpy(newuser->info,a->info);
        pthread_mutex_unlock(&(RegistedUserHead->mute));
        RegistedUserHead->OnlineUserNum++;
    }
    return 1;

}
