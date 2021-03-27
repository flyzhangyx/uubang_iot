#include"../head/SERVER.h"
int UpdateLocalRegUserAndIotlist()
{
    pthread_mutex_lock(&(RegistedUserHead->mute));
    CLN a;
    char query[200]="";
    sprintf(query,"%s%d%s"
            ,"SELECT * FROM `user` WHERE `id` > '"
            ,RegistedUserHead->next->USERKEY_ID
            ,"' ORDER BY `id` ASC"
           );
    MYSQL_RES *res;
    MYSQL_ROW row;
    SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        log_error("SQL NODE NULL");
        return 0;
    }
    MYSQL *mysql=&(temmp->fd);
    if (mysql_real_query(mysql, query, strlen(query)))
    {
        log_error("Failed to Get NewlyUserInfo: %s", mysql_error(mysql));
        release_node(MySqlConnPool, temmp);
        return -1;
    }
    res = mysql_store_result(mysql);
    while ((row = mysql_fetch_row(res)))
    {
        strcpy(a.USERID,row[1]);
        strcpy(a.USERPASSWORD, row[3]);
        a.USERKEY_ID=atoi(row[0]);
        strcpy(a.info,row[4]) ;
        AddtoLocal(&a);
    }
    pthread_mutex_unlock(&(RegistedUserHead->mute));
    release_node(MySqlConnPool, temmp);
    mysql_free_result(res);
    ///...................................................
    memset(query,0,200*sizeof(char));
    if(RegistedIotHead->next==NULL)
    {
        return 1;
    }
    pthread_mutex_lock(&(RegistedIotHead->mute));
    sprintf(query,"%s%d%s"
            ,"SELECT * FROM `iotnode` WHERE `id` > '"
            ,RegistedIotHead->next->USERKEY_ID
            ,"' ORDER BY `id` ASC"
           );
    MYSQL_RES *res_iot;
    MYSQL_ROW row_iot;
    if (mysql_real_query(mysql, query, strlen(query)))
    {
        log_error("Failed to Get NewlyIotInfo: %s", mysql_error(mysql));
        release_node(MySqlConnPool, temmp);
        return -1;
    }
    res_iot = mysql_store_result(mysql);
    while ((row_iot = mysql_fetch_row(res_iot)))
    {
        strcpy(a.USERID,row_iot[1]);
        strcpy(a.USERPASSWORD, row_iot[3]);
        a.USERKEY_ID=atoi(row_iot[0]);
        strcpy(a.info,row_iot[4]) ;
        strcpy(a.checkcode,row_iot[2]);
        AddtoLocal(&a);
    }
    pthread_mutex_unlock(&(RegistedIotHead->mute));
    release_node(MySqlConnPool, temmp);
    mysql_free_result(res_iot);
    return 1;
}
