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
    SQL_NODE *temmp=get_db_connect(MySqlConnPool); MYSQL *mysql=&(temmp->fd);
    if(mysql_real_query(mysql,insert,strlen(insert)))
    {
        log_error("MySQL ERR :%s",mysql_error(mysql));
        release_node(MySqlConnPool, temmp);
        return 0;
    }
    release_node(MySqlConnPool, temmp);
    return 1;
}
