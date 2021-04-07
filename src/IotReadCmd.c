#include "../head/SERVER.h"
int IotReadCmd(CLN *a/*key_id*/,int Devclass,int del)
{
    if(del==0)
    {
        char read[200]="";
        sprintf(read,"%s%d%s%d%s",
                "SELECT * FROM `iotcmd` WHERE `iotid` ='",
                a->USERKEY_ID,
                "'AND `class` = '",
                Devclass,
                "'");
        SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        log_error("SQL NODE NULL");
        return 0;
    }
        MYSQL *mysql=&(temmp->fd);
        if(mysql_real_query(mysql,read,strlen(read)))
        {
            log_error("MySQL ERR(IOT READ CMD) :%s",mysql_error(mysql));
            return 0;
        }
        MYSQL_RES *res;
        MYSQL_ROW row;
        res = mysql_store_result(mysql);
        row = mysql_fetch_row(res);
        sprintf(a->data,"%s_%s_%s_%s_%s_",row[1],row[2],row[3],row[4],row[5]);//class_status_cmd_fromid_cmdDate_//
        return 1;
    }
    else
    {
        char del[200]="";
        sprintf(del,"%s%d%s%d%s",
                "DELETE FROM `iotcmd` WHERE `iotcmd`.`iotid` = '",
                a->USERKEY_ID,
                "'AND `iotcmd`.`class` = '",
                Devclass,
                "'");
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
}
