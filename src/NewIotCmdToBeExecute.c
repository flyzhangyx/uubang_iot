#include "../head/SERVER.h"
int NewIotCmdToBeExecute(CLN *a,char *cmd,int Devclass,int status,char * TimeToBeExe)
{
    char insert[200]="";
    sprintf(insert,"%s%d%s%d%s%d%s%s%s%d%s%s%s",
            "INSERT INTO `iotcmd` (`iotid`, `class`, `status`, `cmd`, `fromid`, `cmdDate`, `cmdStatus`) VALUES ('",
            FindRegisterUserOrIotNode(10,a->TalktoID,0)->USERKEY_ID,
            "', '",
            Devclass,
            "', '",
            status,
            "', '",
            cmd,
            "', '",
            a->USERKEY_ID,
            "','",
             TimeToBeExe,
             "', '0')");
    mysql_master_connect_ping();
    if(mysql_real_query(&mysql,insert,strlen(insert)))
    {
        printf("\nMySQL ERR :%s",mysql_error(&mysql));
        return 0;
    }
    return 1;
}
