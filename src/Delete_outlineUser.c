#include"../head/SERVER.h"
int delete_out_user(CLN *a)
{
    if(strlen(a->USERID)<6)
    {
        return 0;
    }
    if(strlen(a->USERID)>=11)
    {
        USER Tag= onlineUserHead->next;
        if(Tag==NULL)
        {
            //log_info("�����û��б����޴��û�%s,���������",a->USERID);
            return 0;
        }
        else
        {
            if(!strcmp(a->USERID,Tag->USERID))
            {
                onlineUserHead->next=Tag->next;
                free(Tag);
                //log_info("�Ѵ������б���ɾ�����û�%s",a->USERID);
                onlineUserHead->OnlineUserNum--;
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
                        //log_info("�Ѵ������б���ɾ�����û�%s",a->USERID);
                        onlineUserHead->OnlineUserNum--;
                        return 1;
                    }
                    else
                    {
                        Tag=Tag->next;
                    }
                }
            }
        }
        //log_info("�����û��б����޴��û�%s,���������",a->USERID);
        return 0;
    }
    else
    {
        USER Tag= onlineIotHead->next;
        if(Tag==NULL)
        {
            //log_info("�����û��б����޴��豸%s,���������",a->USERID);
            return 0;
        }
        else
        {
            if(!strcmp(a->USERID,Tag->USERID))
            {
                onlineIotHead->next=Tag->next;
                free(Tag);
                //log_info("�Ѵ������б���ɾ�����豸%s",a->USERID);
                onlineIotHead->OnlineUserNum--;
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
                        //log_info("�Ѵ������б���ɾ�����豸%s",a->USERID);
                        onlineIotHead->OnlineUserNum--;
                        return 1;
                    }
                    else
                    {
                        Tag=Tag->next;
                    }
                }
            }
        }
        //log_info("�����û��б����޴��豸%s,���������",a->USERID);
        return 0;
    }
}
