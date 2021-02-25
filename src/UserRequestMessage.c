#include"../head/SERVER.h"
/** \brief
 *
 * \param a CLN* //some figure
 * \param DateFirst char* //From which time
 * \param DateLast char*  //To which time
 * \param SqlTime struct tm* //Time which MSGSQLdb u want to access
 * \return int
 *
 */
int UserRequestMessage(CLN *a,int Direction,char *DateFirst,char *DateLast,struct tm *SqlTime)
{
    UserPacketInterface RecDataStruct;
    int len=0;
    char time_now[20];
    strftime(time_now,20,"%Y-%m-%d",SqlTime);
    //strftime(time_now,80,"%Y-%m-%d %H:%M:%S",localtime(&now_time));
    char sendbuf[sizeof(UserPacketInterface)]= {0};
    memset(&RecDataStruct,0,sizeof(UserPacketInterface));
    memset(sendbuf,0,sizeof(UserPacketInterface));
    char find[200] = "";
    MYSQL_RES *res;
    MYSQL_ROW row;
    if(Direction==0)
    {
        sprintf(find, "%s%s%s%d%s%d%s%s%s%s%s",
                "SELECT * FROM `",
                time_now,
                "` WHERE `userId` = ",
                a->USERKEY_ID,
                " AND `friendId` = ",
                FindRegisterUserOrIotNode(0,a->TalktoID,0)->USERKEY_ID,
                " AND `SendDate` BETWEEN '",
                DateFirst,
                "' AND '",
                DateLast,
                "' ");
    }
    else
    {
        sprintf(find, "%s%s%s%d%s%d%s%s%s%s%s",
                "SELECT * FROM `",
                time_now,
                "` WHERE `userId` = ",
                FindRegisterUserOrIotNode(0,a->TalktoID,0)->USERKEY_ID,
                " AND `friendId` = ",
                a->USERKEY_ID,
                " AND `SendDate` BETWEEN '",
                DateFirst,
                "' AND '",
                DateLast,
                "' ");
    }
    strcpy(RecDataStruct.checkcode,"NME");//NEW MESSAGE
    if(Direction==0)
    {
        strcpy(RecDataStruct.TalktoID,a->TalktoID);
        strcpy(RecDataStruct.USERID,a->USERID);
    }
    else
    {
        strcpy(RecDataStruct.TalktoID,a->USERID);
        strcpy(RecDataStruct.USERID,a->TalktoID);
    }
    mysql_master_connect_ping();
    if (mysql_real_query(&mysql, find, strlen(find)))
    {
        printf("\nSQL ERR (REQUEST MSG):%s\n",mysql_error(&mysql));
        return 0;
    }
    res = mysql_store_result(&mysql);
    while ((row = mysql_fetch_row(res)))
    {
        memset(sendbuf,0,sizeof(UserPacketInterface));
        strcpy(RecDataStruct.DATA,row[2]);//content
        strcpy(RecDataStruct.save,row[3]);//date
        memcpy(sendbuf,&RecDataStruct,sizeof(RecDataStruct));
        len=send(a->remote_socket,sendbuf,sizeof(UserPacketInterface),0);
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(a->remote_socket);
            mysql_free_result(res);
            return 0;
        }
    }
    mysql_free_result(res);
    return 1;
}
