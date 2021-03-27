#include"../head/SERVER.h"
int UserReqFriendRel(CLN *a)
{
    UserPacketInterface RecDataStruct;
    int len=0;
    char sendbuf[sizeof(UserPacketInterface)]= {0};
    char find[100] = "";
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(find, "%s%d", "SELECT * FROM `userrelationship` WHERE `userId` = ", a->USERKEY_ID);
    SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        log_error("SQL NODE NULL");
        return 0;
    }
    MYSQL *mysql=&(temmp->fd);
    if (mysql_real_query(mysql, find, strlen(find)))//No devices bonded
    {
        log_error(" SQL ERR (REQFRIENDREL):%s",mysql_error(mysql));
        release_node(MySqlConnPool, temmp);
        return 0;
    }
    res = mysql_store_result(mysql);
    while ((row = mysql_fetch_row(res)))
    {
        memset(&RecDataStruct,0,sizeof(UserPacketInterface));
        memset(sendbuf,0,sizeof(UserPacketInterface));
        USER Temp = FindRegisterUserOrIotNode(0,NULL,atoi(row[1]));
        if(Temp==NULL)
        {
            release_node(MySqlConnPool, temmp);
            mysql_free_result(res);
            return 0;
        }
        strcpy(RecDataStruct.checkcode,"RCO");
        strcpy(RecDataStruct.TalktoID,Temp->USERID);
        strcpy(RecDataStruct.USERID,a->USERID);
        strcpy(RecDataStruct.save,row[2]);//RelationCreateDate
        RecDataStruct.save[99]=_HC_;
        memcpy(sendbuf,&RecDataStruct,sizeof(RecDataStruct));
        len=send(a->SOCKET,sendbuf,sizeof(UserPacketInterface),0);
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(a->SOCKET);
            release_node(MySqlConnPool, temmp);
            mysql_free_result(res);
            return 0;
        }
    }
    release_node(MySqlConnPool, temmp);
    mysql_free_result(res);
    return 1;
}
