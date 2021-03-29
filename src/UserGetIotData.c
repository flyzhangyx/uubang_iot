#include "../head/SERVER.h"
int UserGetIotData(CLN *a)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    UserPacketInterface RecDataStruct;
    char sendbuf[sizeof(UserPacketInterface)]= {0};
    char find[100]="";
    sprintf(find,"%s%d","SELECT * FROM `iotevtcache` WHERE `iotId` =",FindRegisterUserOrIotNode(10,a->TalktoID,0)->USERKEY_ID);
    SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        log_error("SQL NODE NULL");
        return 0;
    }
    MYSQL *mysql=&(temmp->fd);
    if(mysql_real_query(mysql,find,strlen(find)))
    {
        release_node(MySqlConnPool, temmp);
        log_error(" SQL ERR (USERGETIOTDATA):%s",mysql_error(mysql));
        return 0;
    }
    memset(&RecDataStruct,0,sizeof(UserPacketInterface));
    res = mysql_store_result(mysql);
    strcpy(RecDataStruct.checkcode,"IOT");
    strcpy(RecDataStruct.TalktoID,a->TalktoID);
    strcpy(RecDataStruct.USERID,a->USERID);
    while ((row = mysql_fetch_row(res)))
    {
        memset(sendbuf,0,sizeof(UserPacketInterface));
        strcpy(RecDataStruct.save,row[4]);//UpdateDate
        RecDataStruct.save[99]=_HC_;
        sprintf(RecDataStruct.DATA,"%d_%d_%s_%s_",atoi(row[1]),atoi(row[2]),row[3],row[4]);
        memcpy(sendbuf,&RecDataStruct,sizeof(RecDataStruct));
        int len=SyncSend(a->SOCKET,sendbuf,sizeof(UserPacketInterface),&(a->t));
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(a->SOCKET);
            release_node(MySqlConnPool, temmp);
            mysql_free_result(res);
            return 0;
        }
        Sleep(100);
    }
    release_node(MySqlConnPool, temmp);
    mysql_free_result(res);
    return 1;
}
