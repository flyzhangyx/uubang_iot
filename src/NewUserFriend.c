#include"../head/SERVER.h"
int NewUserFriend(CLN *a,int friendId)
{
    char insert[150] = "";
    if(friendId==0)
    {
        USER Friend_node = FindRegisterUserOrIotNode(0,a->TalktoID,0);
        if(Friend_node==NULL)
        {
            return 0;
        }
        else
            friendId=Friend_node->USERKEY_ID;
    }
    sprintf(insert,"%s%d%s%d%s%s",
            "INSERT INTO `userrelationship` (`userId`, `friendId`, `RelCreateDate`) VALUES ('",
            a->USERKEY_ID,
            "', '",
            friendId,
            "', ",
            "CURRENT_TIMESTAMP)");
    mysql_master_connect_ping();
    if(mysql_real_query(&mysql,insert,strlen(insert)))
    {
        log_error("MySQL ERR (USER FRIEND):%s",mysql_error(&mysql));
        return 0;
    }
    UpdateSqlInfoTimestamp(a->USERKEY_ID,1,0);
    return 1;
}
