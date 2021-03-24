#include "../head/SERVER.h"
void CreateServerInfoInSQL()
{
    static int isCreated = 0;
    if(isCreated)
    {
        return;
    }
    char create[500] = "";
    sprintf(create,"%s%s%s%s%s%s%s%s%s%s%s%s",
            "INSERT INTO `serverruninfo` (`ServerId`, `name`, `os`, `ip`, `vendor`, `UpdateTime`) VALUES ('",
            ServerID,
            "', '",
            "TEST",
            "', '",
            "WinServer",
            "', '",
            NetIP,
            "', '",
            "aliyun",
            "', ",
            " CURRENT_TIMESTAMP)");
    SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        log_error("SQL_NODE NULL UPDATE SERVER RUNINFO");
        return;
    }
    MYSQL *mysql=&(temmp->fd);
    if(mysql_real_query(mysql, create,strlen(create)))
    {
        if(strstr(mysql_error(mysql),"Duplicate"))
        {
            release_node(MySqlConnPool, temmp);
            return;
        }
        log_error("MySQL ERR :%s",mysql_error(mysql));
        release_node(MySqlConnPool, temmp);
        return ;
    }
    release_node(MySqlConnPool, temmp);
    return;
}

int UpdateServerRunInfo(int CpuRate,int Mem,int TaskNum,int OnlineUserNum,int OnlineIotNum)
{
    char update[500] = "";
    sprintf(update,"%s%d%s%s%s%d%s%d%s%d%s%d%s%s",
            "UPDATE `serverruninfo` SET `cpu` = '",
            CpuRate,
            "', `ip` = '",
            NetIP,
            "', `memory` = '",
            Mem,
            "', `onlineUserNum` = '",
            OnlineUserNum,
            "', `onlineIotNum` = '",
            OnlineIotNum,
            "', `TaskNum` = '",
            TaskNum,
            "', `UpdateTime` = CURRENT_TIME() WHERE `serverruninfo`.`ServerId` = ",
            ServerID);
    SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        return 0;
    }
    MYSQL *mysql=&(temmp->fd);
    if(mysql_real_query(mysql, update,strlen(update)))
    {
        log_error("MySQL ERR (UPDATE SERVER RUNINFO):%s",mysql_error(mysql));
        release_node(MySqlConnPool, temmp);
        return 0;
    }
    release_node(MySqlConnPool, temmp);
    return 1;
}
