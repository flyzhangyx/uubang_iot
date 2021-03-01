#include "../head/SERVER.h"
int NewUserSceneCmdStore(CLN *a,char *Cmd,int status,int devclass,int cmdGropu,char *TimeStamp,char *ExeDate)
{
    char insert[300]="";
    sprintf(insert,"%s%d%s%d%s%s%s%d%s%s%s%s%s%d%s%d%s",
            "INSERT INTO `userscenecmd` (`iotid`, `devClass`, `cmdContent`, `status`, `CmdTime`, `ExeDate`, `CmdGroup`, `fromUserId`) VALUES ('",
            FindRegisterUserOrIotNode(10,a->TalktoID,0)->USERKEY_ID,
            "', '",
            devclass,
            "', '",
            Cmd,
            "', '",
            status,
            "', '",
            TimeStamp,
            "', '",
            ExeDate,//Monday to Sunday
            "', '",
            cmdGropu,
            "', '",
            a->USERKEY_ID,
            "')");
    mysql_master_connect_ping();
    if(mysql_real_query(&mysql,insert,strlen(insert)))
    {
        log_error("MySQL ERR (NEW USER SCENE):%s",mysql_error(&mysql));
        return 0;
    }
    return 1;
}
