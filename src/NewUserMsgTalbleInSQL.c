#include "../head/SERVER.h"
int NewUserMsgTableInSQL()
{
    time_t now_time;
    time(&now_time);
    char time_now[20];
    strftime(time_now,80,"%Y-%m-%d",localtime(&now_time));
    char create[300]="";
    sprintf(create,"%s%s%s",
            "CREATE TABLE `iotserver`.`",
            time_now,
            "` ( `userId` INT UNSIGNED NOT NULL , `friendId` INT UNSIGNED NOT NULL , `Content` TEXT NOT NULL , `SendDate` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ) ENGINE = InnoDB"
           );
    SQL_NODE *temmp;
    while((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        Sleep(50);
        continue;
    }
    MYSQL *mysql=&(temmp->fd);
    if(mysql_real_query(mysql,create,strlen(create)))
    {
        const char *err = mysql_error(mysql);
        if(strstr(err,"already"))
        {
            release_node(MySqlConnPool, temmp);
            return -1;
        }
        else
        {
            log_error("SQL ERR (CREATE TABLE):%s",err);
            release_node(MySqlConnPool, temmp);
            return 0;
        }
    }
    release_node(MySqlConnPool, temmp);
    return 1;
}
