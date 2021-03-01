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
            log_info("%d_USERID:%s | Id-Key:%d",paixu,Tag->USERID,Tag->USERKEY_ID);
            Tag=Tag->next;
            paixu++;
        }
        Tag = RegistedIotHead->next;
        paixu = 1;
        while(Tag!=NULL)
        {
            log_info("%d_IOTDeviceID:%s | Id-Key:%d | Class:%d",paixu,Tag->USERID,Tag->USERKEY_ID,atoi(Tag->info));
            Tag=Tag->next;
            paixu++;
        }
        log_info("Print User And IotDeviceOK!");
    }
}

