#include"../head/SERVER.h"
USER FindOnlineUserOrIot(int n,char *USERID,int id)///n为检查方法，0为查找USERID用户并返回该用户节点；10 查找IOT，其他为打印全部用户。
{
    USER Tag= onlineUserHead->next;
    USER Tag_iot=onlineIotHead->next;
    if(n==10)
    {
        if(Tag_iot==NULL)
        {
            //printf("1.Printonlineuser:在线用户列表中无此用户%s",USERID);
            return NULL;
        }
        if(USERID!=NULL)
        {
            if(!strcmp(USERID,Tag_iot->USERID))
            {
                return Tag_iot;
            }
            else
            {
                while(Tag_iot->next!=NULL)
                {
                    if(!strcmp(USERID,Tag_iot->next->USERID))
                    {
                        return Tag_iot->next;
                    }
                    else
                    {
                        Tag=Tag_iot->next;
                    }
                }
                // printf("2.Printonlineuser:在线用户列表中无此用户%s",USERID);
                return NULL;
            }
        }
        else
        {
            if(id==Tag_iot->USERKEY_ID)
            {
                return Tag_iot;
            }
            else
            {
                while(Tag_iot->next!=NULL)
                {
                    if(id==Tag_iot->next->USERKEY_ID)
                    {
                        return Tag_iot->next;
                    }
                    else
                    {
                        Tag=Tag_iot->next;
                    }
                }
                // printf("2.Printonlineuser:在线用户列表中无此用户%s",USERID);
                return NULL;
            }
        }
    }
    else if(n==0)
    {
        if(Tag==NULL)
        {
            //printf("1.Printonlineuser:在线用户列表中无此用户%s",USERID);
            return NULL;
        }
        if(USERID!=NULL)
        {
            if(!strcmp(USERID,Tag->USERID))
            {
                return Tag;
            }
            else
            {
                while(Tag->next!=NULL)
                {
                    if(!strcmp(USERID,Tag->next->USERID))
                    {
                        return Tag->next;
                    }
                    else
                    {
                        Tag=Tag->next;
                    }
                }
                return NULL;
            }
        }
        else
        {
            if(id==Tag->USERKEY_ID)
            {
                return Tag;
            }
            else
            {
                while(Tag->next!=NULL)
                {
                    if(id==Tag->next->USERKEY_ID)
                    {
                        return Tag->next;
                    }
                    else
                    {
                        Tag=Tag->next;
                    }
                }
                return NULL;
            }
        }
    }
    else
    {
        USER Tag= onlineUserHead->next;
        USER Tag_iot = onlineIotHead->next;
        if(Tag==NULL&&Tag_iot==NULL)
        {
            return NULL;
        }
        else
        {
            int paixu=1;
            while(Tag!=NULL)
            {
                printf("|%d_Online_USERID:%s\t| Id-Key:%d\t|\n",paixu,Tag->USERID,Tag->USERKEY_ID);
                Tag=Tag->next;
                paixu++;
            }
            printf("\n");
            paixu = 1;
            while(Tag_iot!=NULL)
            {
                printf("%d_Online_IotDevID:%s\t| Id-Key:%d\t|\n",paixu,Tag_iot->USERID,Tag_iot->USERKEY_ID);
                Tag_iot=Tag_iot->next;
                paixu++;
            }
            printf("\nPrint User And IotDeviceOK!\n");
        }
    }
    return NULL;
}

