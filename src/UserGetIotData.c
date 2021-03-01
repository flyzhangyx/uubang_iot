#include "../head/SERVER.h"
int UserGetIotData(CLN *a)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    UserPacketInterface RecDataStruct;
    char sendbuf[sizeof(UserPacketInterface)]= {0};
    char find[100]="";
    sprintf(find,"%s%d","SELECT * FROM `iotevtcache` WHERE `iotId` =",FindRegisterUserOrIotNode(10,a->TalktoID,0)->USERKEY_ID);
    mysql_master_connect_ping();
    if(mysql_real_query(&mysql,find,strlen(find)))
    {
        log_error(" SQL ERR (USERGETIOTDATA):%s",mysql_error(&mysql));
        return 0;
    }
    memset(&RecDataStruct,0,sizeof(UserPacketInterface));
    res = mysql_store_result(&mysql);
    strcpy(RecDataStruct.checkcode,"IOT");
    strcpy(RecDataStruct.TalktoID,a->TalktoID);
    strcpy(RecDataStruct.USERID,a->USERID);
    while ((row = mysql_fetch_row(res)))
    {
        memset(sendbuf,0,sizeof(UserPacketInterface));
        strcpy(RecDataStruct.save,row[4]);//UpdateDate
        RecDataStruct.save[99]='\n';
        sprintf(RecDataStruct.DATA,"%d+%d+%s",atoi(row[1]),atoi(row[2]),row[3]);
        memcpy(sendbuf,&RecDataStruct,sizeof(RecDataStruct));
        int len=send(a->remote_socket,sendbuf,sizeof(UserPacketInterface),0);
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(a->remote_socket);
            mysql_free_result(res);
            return 0;
        }
        Sleep(100);
    }
    mysql_free_result(res);
    return 1;
}
