#include"../head/SERVER.h"
int UserDeleteSce(CLN* a,int iotid)
{
    char del[100]="";
    sprintf(del,"%s%d","DELETE FROM `userscenecmd` WHERE `userscenecmd`.`iotid`  = ",iotid);
    log_debug("%s",del);
    SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        log_error("SQL NODE NULL");
        return 0;
    }
    MYSQL *mysql=&(temmp->fd);
    if(mysql_real_query(mysql,del,strlen(del)))
    {
        release_node(MySqlConnPool, temmp);
        log_error("MySQL ERR(IOT DEL CMD) :%s",mysql_error(mysql));
        return 0;
    }
    release_node(MySqlConnPool, temmp);
    return 1;
}
