#include"../head/SERVER.h"
int Check_Id_Pwd(int n,CLN *a)
{
    if(!(strlen(a->USERID)<11))
    {
        USER Tag= RegistedUserHead->next;
        if(n==0)
        {
            if(Tag!=NULL)
            {
                if(!strcmp(a->USERID,Tag->USERID)&&!strcmp(a->USERPASSWORD,Tag->USERPASSWORD))
                {
                    return 1;
                }
                else
                {
                    while(Tag->next!=NULL)
                    {
                        if(!strcmp(a->USERID,Tag->next->USERID)&&!strcmp(a->USERPASSWORD,Tag->next->USERPASSWORD))
                        {
                            return 1;
                        }
                        else
                        {
                            Tag=Tag->next;
                        }
                    }
                    log_info("Checkidpwd:��ע���û��б����û�%s�˺Ż����������,�������",a->USERID);
                    return -1;
                }
            }
            else
            {
                log_info("Checkidpwd:��ע���û��б����û�%s�˺Ż����������,�������",a->USERID);
                return -1;
            }
        }
        else
        {
            if(Tag!=NULL)
            {
                if(!strcmp(a->USERID,Tag->USERID))
                {
                    return 1;
                }
                else
                {
                    while(Tag->next!=NULL)
                    {
                        if(!strcmp(a->USERID,Tag->USERID))
                        {
                            //log_info("checkidpwd:��ע���û��б����д��û�%s,��ֹע��",a->USERID);
                            return 1;
                        }
                        else
                        {
                            Tag=Tag->next;
                        }
                    }
                    //log_info("checkidpwd:��ע���û��б����޴��û�%s,׼��ע��",a->USERID);
                    return -1;
                }
            }
            else
            {
                log_info("checkidpwd:��ע���û��б����޴��û�%s,׼��ע��",a->USERID);
                return -1;
            }
        }
        return 5;
    }
    else
    {
        USER Tag= RegistedIotHead->next;
        if(n==0)
        {
            if(Tag!=NULL)
            {
                if(!strcmp(a->USERID,Tag->USERID)&&!strcmp(a->USERPASSWORD,Tag->USERPASSWORD))
                {
                    return 1;
                }
                else
                {
                    while(Tag->next!=NULL)
                    {
                        if(!strcmp(a->USERID,Tag->next->USERID)&&!strcmp(a->USERPASSWORD,Tag->next->USERPASSWORD))
                        {
                            return 1;
                        }
                        else
                        {
                            Tag=Tag->next;
                        }
                    }
                    log_info("Checkidpwd:��ע���û��б����û�%s�˺Ż����������,�������",a->USERID);
                    return -1;
                }
            }
            else
            {
                log_info("Checkidpwd:��ע���û��б����û�%s�˺Ż����������,�������",a->USERID);
                return -1;
            }
        }
        else
        {
            if(Tag!=NULL)
            {
                if(!strcmp(a->USERID,Tag->USERID))
                {
                    return 1;
                }
                else
                {
                    while(Tag->next!=NULL)
                    {
                        if(!strcmp(a->USERID,Tag->USERID))
                        {
                            //log_info("checkidpwd:��ע���û��б����д��û�%s,��ֹע��",a->USERID);
                            return 1;
                        }
                        else
                        {
                            Tag=Tag->next;
                        }
                    }
                    //log_info("checkidpwd:��ע���û��б����޴��û�%s,׼��ע��",a->USERID);
                    return -1;
                }
            }
            else
            {
                log_info("checkidpwd:��ע���û��б����޴��û�%s,׼��ע��",a->USERID);
                return -1;
            }
        }
        return 5;
    }
}
