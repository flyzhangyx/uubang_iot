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
    newuser->USERKEY_ID = a->USERKEY_ID;
    if(strlen(a->USERID)<11)
    {
        newuser->next=RegistedIotHead->next;
        strcpy(newuser->info,a->info);
        RegistedIotHead->next=newuser;
        RegistedIotHead->OnlineUserNum++;
    }
    else
    {
        newuser->next=RegistedUserHead->next;
        RegistedUserHead->next=newuser;
        strcpy(newuser->info,a->info);
        RegistedUserHead->OnlineUserNum++;
    }
    return 1;

}
