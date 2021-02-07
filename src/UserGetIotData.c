#include "../head/SERVER.h"
int UserGetIotData(CLN *a)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    sendbag RecDataStruct;
    char sendbuf[sizeof(sendbag)]= {0};
    char find[100]="";
    sprintf(find,"%s%d","SELECT * FROM `iotevtcache` WHERE `iotId` =",FindRegisterUserOrIotNode(10,a->TalktoID,0)->USERKEY_ID);
    if(mysql_real_query(&mysql,find,strlen(find)))
    {
        printf("\n SQL ERR (USERGETIOTDATA):%s",mysql_error(&mysql));
        return 0;
    }
    memset(&RecDataStruct,0,sizeof(sendbag));
    res = mysql_store_result(&mysql);
    strcpy(RecDataStruct.checkcode,"IOT");
    strcpy(RecDataStruct.TalktoID,a->TalktoID);
    strcpy(RecDataStruct.USERID,a->USERID);
    while ((row = mysql_fetch_row(res)))
    {
        memset(sendbuf,0,sizeof(sendbag));
        strcpy(RecDataStruct.save,row[4]);//RelationCreateDate
        RecDataStruct.save[99]='\n';
        sprintf(RecDataStruct.DATA,"%d+%d+%s",atoi(row[1]),atoi(row[2]),row[3]);
        memcpy(sendbuf,&RecDataStruct,sizeof(RecDataStruct));
        int len=send(a->remote_socket,sendbuf,sizeof(sendbag),0);
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(a->remote_socket);
            delete_out_user(a);
            mysql_free_result(res);
            return 0;
        }
        Sleep(100);
    }
    mysql_free_result(res);
    return 1;
}
