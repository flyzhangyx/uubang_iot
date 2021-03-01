#include"../head/SERVER.h"
int NewUserIot(CLN *a,int iotId)
{
    char insert[150] = "";
    if(iotId==0)
    {
        USER iot_node = FindRegisterUserOrIotNode(10,a->TalktoID,0);
        if(iot_node==NULL)
        {
            return 0;
        }
        else
        {
            iotId=iot_node->USERKEY_ID;
        }

    }
    sprintf(insert,"%s%d%s%d%s%s",
            "INSERT INTO `iotrelationship` (`userId`, `iotId`, `bondDate`) VALUES ('",
            a->USERKEY_ID,
            "', '",
            iotId,
            "', ",
            "CURRENT_TIMESTAMP)");
    mysql_master_connect_ping();
    if(mysql_real_query(&mysql,insert,strlen(insert)))
    {
        log_error("MySQL ERR (USER IOT):%s",mysql_error(&mysql));
        return 0;
    }
    UpdateSqlInfoTimestamp(a->USERKEY_ID,2,0);
    return 1;
}
