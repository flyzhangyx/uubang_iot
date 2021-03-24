#include "../head/SERVER.h"
int getAppVersion()
{
    char AppVersionQuery[100] = "";
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(AppVersionQuery,"%s","SELECT * FROM `appinfo`");
    SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        log_error("SQL NODE NULL WHEN REQ APPVERSION");
        return 0;
    }
    MYSQL *mysql=temmp->mysql_sock;
    if (mysql_real_query(mysql, AppVersionQuery, strlen(AppVersionQuery)+1))
    {
        log_error("SQL ERR (REQ APPVERSION INFO):%s",mysql_error(temmp->mysql_sock));
        release_node(MySqlConnPool, temmp);
        log_error("更新文件读取失败!");
        return 0;
    }
    res = mysql_store_result(temmp->mysql_sock);
    row = mysql_fetch_row(res);//only a record
    release_node(MySqlConnPool, temmp);
    mysql_free_result(res);
    if(row[0]==NULL)
    {
        log_error("更新文件读取失败!");
        return 0;
    }
    else
    {
        sprintf(app_version,"%s",row[0]);
        app_version[3]=0;
        log_info("APP版本-%s",app_version);
        return 1;
    }
}
