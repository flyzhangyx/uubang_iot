#include"../head/SERVER.h"
int UserRegiter(CLN* a)
{
    char insert[800] = "";
    sprintf(insert,"%s%s%s%s%s%s%s%s%s%s",
            "INSERT INTO user (id, userId, nickName, passWord, email, registDate) VALUES (NULL, '",
            a->USERID,
            "', '",
            a->info,
            "', '",
            a->USERPASSWORD,
            "', '",
            a->data,
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
        log_error("MySQL ERR :%s",mysql_error(mysql));
        release_node(MySqlConnPool, temmp);
        return -1;
    }
    char insert1[800] = "";
    sprintf(insert1,"%s%s%s%s%s%s%s%d%s",
            "INSERT INTO `iotserverinfo` (`userId`, `FriendRelUpdateTime`, `IotRelUpdateTime`, `MsgRecUpdateTime`, `IsRegisterRecord`) VALUES (NULL,",
            "CURRENT_TIMESTAMP",
            ", ",
            "CURRENT_TIMESTAMP",
            ", ",
            "CURRENT_TIMESTAMP",
            ", '",
            0,
            "')");
    if(mysql_real_query(mysql,insert1,strlen(insert1)))
    {
        release_node(MySqlConnPool, temmp);
        log_error("MySQL ERR :%s",mysql_error(mysql));
        return -1;
    }
    return 1;
}
