#include"../head/SERVER.h"
int UserRePwd(CLN *a)
{
    USER Tag= RegistedUserHead->next;
    if(Check_Id_Pwd(0,a)==1)
    {
        if(Tag==NULL)
        {
            printf("\n注册用户列表%s不匹配,请检查服务器\n",a->USERID);
            return -1;
        }
        else
        {
            if(!strcmp(a->USERID,Tag->USERID))
            {
                strcpy(Tag->USERPASSWORD,a->REUSERPASSWORD) ;
                return 1;
            }
            else
            {
                while(Tag->next!=NULL)
                {
                    if(!strcmp(a->USERID,Tag->next->USERID))
                    {
                        char update[200] = "";
                        sprintf(update,"%s%s%s%d","UPDATE `user` SET `passWord` = '",a->REUSERPASSWORD,"' WHERE `user`.`id` = ",a->USERKEY_ID);
                        if(mysql_real_query(&mysql, update,strlen(update)))
                        {
                            printf("\nMySQL ERR :%s",mysql_error(&mysql));
                            return -1;
                        }
                        strcpy(Tag->next->USERPASSWORD,a->REUSERPASSWORD) ;
                        return 1;
                    }
                    else
                    {
                        Tag=Tag->next;
                    }
                }
            }
        }
    }
    else
    {
        return -1;
    }
    return 0;
}
