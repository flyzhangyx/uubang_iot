#include"../head/SERVER.h"
///Index ::::  userId+FriendRelUpdateTime+IotRelUpdateTime+MsgRecUpdateTime+IsRegisterRecord
char* GetUpdateTimeStamp(int UserId,int index,char *ret_row)
{
    char find[100] = "";
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(find, "%s%d", "SELECT * FROM `iotserverinfo` WHERE `userId` = ", UserId);
    SQL_NODE *temmp=get_db_connect(MySqlConnPool); MYSQL *mysql=&(temmp->fd);
    if (mysql_real_query(mysql, find, strlen(find)))
    {
        log_error("SQL ERR (REQ IOTSERVER INFO):%s",mysql_error(mysql));
        release_node(MySqlConnPool, temmp);
        return 0;
    }
    res = mysql_store_result(mysql);
    row = mysql_fetch_row(res);//only a record
    release_node(MySqlConnPool, temmp);
    mysql_free_result(res);
    if(ret_row!=NULL)
    memcpy(ret_row,row[index],strlen(row[index])+1);
    return row[index];
}
