#include"../head/SERVER.h"
void PrintAllUserAndIotDevice()
{
    USER Tag= RegistedUserHead->next;
    if(Tag==NULL)
    {
        return;
    }
    else
    {
        int paixu=1;
        while(Tag!=NULL)
        {
            printf("%d_USERID:%s\t| Id-Key:%d\n",paixu,Tag->USERID,Tag->USERKEY_ID);
            Tag=Tag->next;
            paixu++;
        }
        Tag = RegistedIotHead->next;
        paixu = 1;
        while(Tag!=NULL)
        {
            printf("%d_IOTDeviceID:%s \t| Id-Key:%d\t| Class:%d\n",paixu,Tag->USERID,Tag->USERKEY_ID,atoi(Tag->info));
            Tag=Tag->next;
            paixu++;
        }
        printf("Print User And IotDeviceOK!\n");
    }
}

