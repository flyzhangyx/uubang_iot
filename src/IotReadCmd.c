#include "../head/SERVER.h"
int IotReadCmd(CLN *a,int Devclass,int del)
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
        mysql_master_connect_ping();
        if(mysql_real_query(&mysql,read,strlen(read)))
        {
            log_error("MySQL ERR(IOT READ CMD) :%s",mysql_error(&mysql));
            return 0;
        }
        MYSQL_RES *res;
        MYSQL_ROW row;
        res = mysql_store_result(&mysql);
        row = mysql_fetch_row(res);
        sprintf(a->data,"%s|%s|%s|%s",row[2],row[3],row[4],row[5]);
        return 1;
    }else{
        char del[200]="";
        sprintf(del,"%s%d%s%d%s",
                "DELETE FROM `iotcmd` WHERE `iotcmd`.`iotid` = '",
                 a->USERKEY_ID,
                "'AND `iotcmd`.`class` = '",
                 Devclass,
                "'");
        mysql_master_connect_ping();
        if(mysql_real_query(&mysql,del,strlen(del)))
        {
            log_error("MySQL ERR(IOT DEL CMD) :%s",mysql_error(&mysql));
            return 0;
        }
        return 1;
    }
}
