#include "../head/SERVER.h"
int NewUserMsgStorage(CLN *a,int ToId)
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
    if (mysql_real_query(&mysql, insert, strlen(insert)))
    {
        printf("\nSQL ERR (INSERT MSG):%s\n",mysql_error(&mysql));
        return 0;
    }
    UpdateSqlInfoTimestamp(a->USERKEY_ID,3,0);
    return 1;
}
