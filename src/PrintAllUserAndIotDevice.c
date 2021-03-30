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
            printf("|%d_USERID:%s\t| Id-Key:%d\t|\n",paixu,Tag->USERID,Tag->USERKEY_ID);
            Tag=Tag->next;
            paixu++;
        }
        Tag = RegistedIotHead->next;
        paixu = 1;
        printf("\n");
        while(Tag!=NULL)
        {
            printf("|%d_IOTDeviceID:%s \t| Id-Key:%d\t|\n",paixu,Tag->USERID,Tag->USERKEY_ID);
            Tag=Tag->next;
            paixu++;
        }
        printf("\nPrint User And IotDeviceOK!\n");
    }
}

