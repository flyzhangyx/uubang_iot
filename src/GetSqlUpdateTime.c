#include"../head/SERVER.h"
///Index ::::  userId+FriendRelUpdateTime+IotRelUpdateTime+MsgRecUpdateTime+IsRegisterRecord
char* GetUpdateTimeStamp(int UserId,int index,char *ret_row)
{
    char find[100] = "";
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(find, "%s%d", "SELECT * FROM `iotserverinfo` WHERE `userId` = ", UserId);
    SQL_NODE *temmp;while((temmp=get_db_connect(MySqlConnPool))==NULL){Sleep(50);continue;}MYSQL *mysql=&(temmp->fd);
    if (mysql_real_query(&(temmp->fd), find, strlen(find)+1))
    {
        log_error("SQL ERR (REQ IOTSERVER INFO):%s",mysql_error(&(temmp->fd)));
        release_node(MySqlConnPool, temmp);
        return 0;
    }
    res = mysql_store_result(&(temmp->fd));
    row = mysql_fetch_row(res);//only a record
    release_node(MySqlConnPool, temmp);
    mysql_free_result(res);
    if(ret_row!=NULL)
    {
        if(row[index]!=NULL);
        memcpy(ret_row,row[index],strlen(row[index])+1);
    }
    return row[index];
}
