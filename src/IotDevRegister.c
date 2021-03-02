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
    SQL_NODE *temmp=get_db_connect(MySqlConnPool); MYSQL *mysql=&(temmp->fd);
    if(mysql_real_query(mysql,insert,strlen(insert)))
    {
        release_node(MySqlConnPool, temmp);
        log_error("MySQL ERR :%s",mysql_error(mysql));
        return -1;
    }
    release_node(MySqlConnPool, temmp);
    return 1;
}
