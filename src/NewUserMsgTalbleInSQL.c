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
    if(mysql_real_query(&mysql,create,strlen(create)))
    {
        printf("SQL ERR (CREATE TABLE):%s",mysql_error(&mysql));
        return 0;
    }
    return 1;
}
