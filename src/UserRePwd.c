#include"../head/SERVER.h"
int UserRePwd(CLN *a)
{
    USER Tag= RegistedUserHead->next;
    if(Check_Id_Pwd(0,a)==1)
    {
        if(Tag==NULL)
        {
            log_info("注册用户列表%s不匹配,请检查服务器",a->USERID);
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
                        SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        log_error("SQL NODE NULL");
        return 0;
    }
                        MYSQL *mysql=&(temmp->fd);
                        if(mysql_real_query(mysql, update,strlen(update)))
                        {
                            release_node(MySqlConnPool, temmp);
                            log_error("MySQL ERR :%s",mysql_error(mysql));
                            return -1;
                        }
                        strcpy(Tag->next->USERPASSWORD,a->REUSERPASSWORD) ;
                        release_node(MySqlConnPool, temmp);
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
