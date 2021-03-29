#include"../head/SERVER.h"
USER FindRegisterUserOrIotNode(int n,char* USERID,int id)///查找用户并返回该用户节点
{
    USER Tag= RegistedUserHead->next;
    USER Tag_iot=RegistedIotHead->next;
    if(n==10)
    {
        if(Tag_iot==NULL)
        {
            //log_debug("1.Printonlineuser:在线用户列表中无此用户%s",USERID);
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
                        Tag_iot=Tag_iot->next;
                    }
                }
                // log_debug("2.Printonlineuser:在线用户列表中无此用户%s",USERID);
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
                        Tag_iot=Tag_iot->next;
                    }
                }
                // log_debug("2.Printonlineuser:在线用户列表中无此用户%s",USERID);
                return NULL;
            }
        }
    }
    else if(n==0)
    {
        if(Tag==NULL)
        {
            //log_debug("1.Printonlineuser:在线用户列表中无此用户%s",USERID);
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
                // log_debug("2.Printonlineuser:在线用户列表中无此用户%s",USERID);
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
                // log_debug("2.Printonlineuser:在线用户列表中无此用户%s",USERID);
                return NULL;
            }
        }
    }
    else
    {
        return NULL;
    }
}


