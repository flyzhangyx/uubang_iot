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
            printf("\n%d_USERID:%s | Id-Key:%d\n",paixu,Tag->USERID,Tag->USERKEY_ID);
            Tag=Tag->next;
            paixu++;
        }
        Tag = RegistedIotHead->next;
        paixu = 1;
        while(Tag!=NULL)
        {
            printf("\n%d_IOTDeviceID:%s | Id-Key:%d | Class:%d\n",paixu,Tag->USERID,Tag->USERKEY_ID,atoi(Tag->info));
            Tag=Tag->next;
            paixu++;
        }
        printf("\nPrint User And IotDeviceOK!\n");
    }
}

