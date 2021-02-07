#include"../head/SERVER.h"
void RequestIotEvent(CLN *a)
{
    sendbag RecDataStruct;
    int len=0;
    char sendbuf[sizeof(sendbag)]= {0};
    Message d;
    memset(&d,0,sizeof(Message));
    memset(&RecDataStruct,0,sizeof(sendbag));
    memset(sendbuf,0,sizeof(sendbag));
    char find[100] = "";
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(find, "%s%s%s%d", "SELECT * FROM ", a->TalktoID," WHERE `toId` = ",a->USERKEY_ID);
    if (mysql_real_query(&mysql, find, strlen(find)))
    {
        printf("\nSQL ERR\n");
        return;
    }
    res = mysql_store_result(&mysql);
    while ((row = mysql_fetch_row(res)))
    {
        //printf("\n%s/%s/%s/%s",row[1],row[2],row[3],row[4]);
        strcpy(RecDataStruct.checkcode,"NIE");//NEW IOT EVENT
        strcpy(RecDataStruct.TalktoID,a->TalktoID);
        strcpy(RecDataStruct.USERID,a->USERID);
        strcpy(RecDataStruct.DATA,row[1]);//evtclass
        strcpy(RecDataStruct.DATA,row[3]);//content
        strcpy(RecDataStruct.save,row[4]);//date
        memcpy(sendbuf,&RecDataStruct,sizeof(RecDataStruct));
        len=send(a->remote_socket,sendbuf,sizeof(sendbag),0);
        // free(sendbuf);
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(a->remote_socket);
            delete_out_user(a);
            return ;
        }
    }
    mysql_free_result(res);
}
