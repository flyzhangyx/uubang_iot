#include"../head/SERVER.h"
int UserReqIotRel(CLN *a)
{
    UserPacketInterface RecDataStruct;
    int len=0;
    char sendbuf[sizeof(UserPacketInterface)]= {0};
    printf("\nContact Iot OK:");
    char find[100] = "";
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(find, "%s%d", "SELECT * FROM `iotrelationship` WHERE `userId` = ", a->USERKEY_ID);
    mysql_master_connect_ping();
    if (mysql_real_query(&mysql, find, strlen(find)))//No devices bonded
    {
        printf("\n SQL ERR (REQIOTREL):%s",mysql_error(&mysql));
        return 0;
    }
    res = mysql_store_result(&mysql);
    memset(&RecDataStruct,0,sizeof(UserPacketInterface));
    strcpy(RecDataStruct.checkcode,"RCI");
    strcpy(RecDataStruct.USERID,a->USERID);
    while ((row = mysql_fetch_row(res)))
    {
        memset(sendbuf,0,sizeof(UserPacketInterface));
        USER Temp = FindRegisterUserOrIotNode(10,NULL,atoi(row[1]));
        if(Temp==NULL)
        {
            mysql_free_result(res);
            return 0;
        }
        strcpy(RecDataStruct.TalktoID,Temp->USERID);
        strcpy(RecDataStruct.save,row[2]);//RelationCreateDate
        RecDataStruct.save[99]='\n';
        memcpy(sendbuf,&RecDataStruct,sizeof(RecDataStruct));
        len=send(a->remote_socket,sendbuf,sizeof(UserPacketInterface),0);
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(a->remote_socket);
            mysql_free_result(res);
            return 0;
        }
        //Sleep(100);
    }
    mysql_free_result(res);
    return 1;
}
