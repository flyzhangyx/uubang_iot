#include "../head/SERVER.h"
int UpdateSqlInfoTimestamp(int UserId,int index,int flag)
{
    char update[200] = "";
    switch(index)
    {
    case 1:
        sprintf(update,"%s%s%s%d"
                ,"UPDATE `iotserverinfo` SET `FriendRelUpdateTime` = "
                ,"CURRENT_TIME()"
                ," WHERE `iotserverinfo`.`userId` = "
                ,UserId);
        break;
    case 2:
        sprintf(update,"%s%s%s%d"
                ,"UPDATE `iotserverinfo` SET `IotRelUpdateTime` = "
                ,"CURRENT_TIME()"
                ," WHERE `iotserverinfo`.`userId` = "
                ,UserId);
        break;
    case 3:
        sprintf(update,"%s%s%s%d"
                ,"UPDATE `iotserverinfo` SET `MsgRecUpdateTime` = "
                ,"CURRENT_TIME()"
                ," WHERE `iotserverinfo`.`userId` = "
                ,UserId);
        break;
    case 4:
        sprintf(update,"%s%d%s%d"
                ,"UPDATE `iotserverinfo` SET `IsRegisterRecord` = "
                ,flag
                ," WHERE `iotserverinfo`.`userId` = "
                ,UserId);
        break;
    }
    mysql_master_connect_ping();
    if(mysql_real_query(&mysql, update,strlen(update)))
    {
        log_error("MySQL ERR :%s",mysql_error(&mysql));
        return -1;
    }
    return 1;
}
