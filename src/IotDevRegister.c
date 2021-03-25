#include"../head/SERVER.h"
int IotRegister(CLN* a,int DevClass)
{
    char insert[200] = "";
    sprintf(insert,"%s%s%s%d%s%s%s%s",
            "INSERT INTO `iotnode` (`id`, `pinCode`, `deviceClass`, `openId`, `RegistDate`) VALUES (NULL, '",
            a->USERID,
            "', '",
            DevClass,
            "', '",
            a->USERPASSWORD,
            "', ",
            "CURRENT_TIMESTAMP)");
    SQL_NODE *temmp;
    while((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        Sleep(50);
        continue;
    }
    MYSQL *mysql=&(temmp->fd);
    if(mysql_real_query(mysql,insert,strlen(insert)))
    {
        if(!strstr(mysql_error(mysql),"Duplicate"))
        {
            release_node(MySqlConnPool, temmp);
            log_error("MySQL ERR :%s",mysql_error(mysql));
            return -1;
        }
    }
    release_node(MySqlConnPool, temmp);
    return 1;
}
