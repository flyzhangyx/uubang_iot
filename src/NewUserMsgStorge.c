#include "../head/SERVER.h"
int NewUserMsgStorage(CLN *a/* key_id,data*/,int ToId)
{
    time_t now_time;
    time(&now_time);
    char time_now[20];
    strftime(time_now,80,"%Y-%m-%d",localtime(&now_time));
    char insert[700]="";
    sprintf(insert,"%s%s%s%d%s%d%s%s%s%s",
            "INSERT INTO `",
            time_now,
            "` (`userId`, `friendId`, `Content`, `SendDate`) VALUES ('",
            a->USERKEY_ID,
            "', '",
            ToId,
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
    if (mysql_real_query(mysql, insert, strlen(insert)))
    {
        log_error("SQL ERR (INSERT MSG):%s",mysql_error(mysql));
        release_node(MySqlConnPool, temmp);
        return 0;
    }
    UpdateSqlInfoTimestamp(a->USERKEY_ID,3,0);
    release_node(MySqlConnPool, temmp);
    return 1;
}
