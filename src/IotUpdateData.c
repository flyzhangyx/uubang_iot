#include"../head/SERVER.h"
/** \brief
 *
 * \param a CLN
 * \param EvtClass int
 * \param status int //0 Off,1 On
 * \return int
 *
 */
int IotUpdateStatus(CLN *a,int EvtClass,int status)
{
    char insert[800]="";
    char update[800]="";
    sprintf(insert,"%s%d%s%d%s%d%s%s%s",
            "INSERT INTO `iotevtcache` (`iotId`, `evtClass`, `status`, `content`, `updateDate`) VALUES ('",
            a->USERKEY_ID,
            "', '",
            EvtClass,
            "', '",
            status,
            "', '",
            a->data,
            "', CURRENT_TIMESTAMP)");
    sprintf(update,"%s%d%s%s%s%d%s%d",
            "UPDATE `iotevtcache` SET `status` = '",
            status,
            "', `content` = '",
            a->data,
            "', `updateDate` = CURRENT_TIME() WHERE `iotevtcache`.`iotId` = ",
            a->USERKEY_ID,
            " AND `iotevtcache`.`evtClass` =  ",
            EvtClass);
    SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        log_error("SQL NODE NULL");
        return 0;
    }
    MYSQL *mysql=&(temmp->fd);
    if(mysql_real_query(mysql,insert,strlen(insert)))
    {
        if(strstr(mysql_error(mysql),"Duplicate")==NULL)
        {
            log_error("SQL ERR(IOT UPDATE ERR1): %s",mysql_error(mysql));
            release_node(MySqlConnPool, temmp);
            return 0;
        }
        else
        {
            release_node(MySqlConnPool, temmp);
            if((temmp=get_db_connect(MySqlConnPool))==NULL)
            {
                log_error("SQL NODE NULL");
                return 0;
            }
            MYSQL *mysql=&(temmp->fd);
            mysql_real_query(mysql,update,strlen(update));
            if(mysql_affected_rows(mysql)==0)
            {
                release_node(MySqlConnPool, temmp);
                log_error("SQL ERR(IOT UPDATE ERR2): %s",mysql_error(mysql));
                return 0;
            }
            else
            {
                release_node(MySqlConnPool, temmp);
                return 1;
            }
        }
    }
    else
    {
        release_node(MySqlConnPool, temmp);
        return 1;
    }
}
