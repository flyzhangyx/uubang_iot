#include"../head/SERVER.h"
int delete_out_user(CLN *a)
{
    if(strlen(a->USERID)>=11)
    {
        USER Tag= onlineUserHead->next;
        if(Tag==NULL)
        {
            printf("\n�����û��б����޴��û�%s,���������\n",a->USERID);
            return 0;
        }
        else
        {
            if(!strcmp(a->USERID,Tag->USERID))
            {
                onlineUserHead->next=Tag->next;
                free(Tag);
                printf("\n�Ѵ������б���ɾ�����û�%s\n",a->USERID);
                //CLN_num--;
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
                        printf("\n�Ѵ������б���ɾ�����û�%s\n",a->USERID);
                        //CLN_num--;
                        return 1;
                    }
                    else
                    {
                        Tag=Tag->next;
                    }
                }
            }
        }
        printf("\n�����û��б����޴��û�%s,���������\n",a->USERID);
        return 0;
    }
    else
    {
        USER Tag= onlineIotHead->next;
        if(Tag==NULL)
        {
            printf("\n�����û��б����޴��豸%s,���������\n",a->USERID);
            return 0;
        }
        else
        {
            if(!strcmp(a->USERID,Tag->USERID))
            {
                onlineIotHead->next=Tag->next;
                free(Tag);
                printf("\n�Ѵ������б���ɾ�����豸%s\n",a->USERID);
                //CLN_num--;
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
                        printf("\n�Ѵ������б���ɾ�����豸%s\n",a->USERID);
                        //CLN_num--;
                        return 1;
                    }
                    else
                    {
                        Tag=Tag->next;
                    }
                }
            }
        }
        printf("\n�����û��б����޴��豸%s,���������\n",a->USERID);
        return 0;
    }
}
