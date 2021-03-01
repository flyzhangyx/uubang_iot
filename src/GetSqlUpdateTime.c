#include"../head/SERVER.h"
///Index ::::  userId+FriendRelUpdateTime+IotRelUpdateTime+MsgRecUpdateTime+IsRegisterRecord
char* GetUpdateTimeStamp(int UserId,int index)
{
    char find[100] = "";
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(find, "%s%d", "SELECT * FROM `iotserverinfo` WHERE `userId` = ", UserId);
    mysql_master_connect_ping();
    if (mysql_real_query(&mysql, find, strlen(find)))
    {
        log_error("SQL ERR (REQ IOTSERVER INFO):%s",mysql_error(&mysql));
        return 0;
    }
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);//only a record
    mysql_free_result(res);
    return row[index];
}
