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
    mysql_master_connect_ping();
    if(mysql_real_query(&mysql,insert,strlen(insert)))
    {
        log_error("MySQL ERR :%s",mysql_error(&mysql));
        return -1;
    }
    char insert1[800] = "";
    sprintf(insert1,"%s%s%s%s%s%s%s%d%s",
            "INSERT INTO `iotserverinfo` (`userId`, `FriendRelUpdateTime`, `IotRelUpdateTime`, `MsgRecUpdateTime`, `IsRegisterRecord`) VALUES ('",
            a->USERID,
            "', '",
            "CURRENT_TIMESTAMP",
            "', '",
            "CURRENT_TIMESTAMP",
            "', '",
            0,
            "')");
    mysql_master_connect_ping();
    if(mysql_real_query(&mysql,insert1,strlen(insert1)))
    {
        log_error("MySQL ERR :%s",mysql_error(&mysql));
        return -1;
    }
    return 1;
}
