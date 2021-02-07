#include"../head/SERVER.h"
void RequestIotDevices(CLN *a)
{
    sendbag RecDataStruct;
    int len=0;
    USER talktouser=NULL;
    char sendbuf[sizeof(sendbag)]= {0};
    Contact d;
    memset(&d,0,sizeof(Contact));
    printf("\nContact OK:");
    char find[50] = "";
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(find, "%s%s", "SELECT * FROM ", a->USERID);
    if (mysql_real_query(&mysql, find, strlen(find)))//No devices bonded
    {
        return;
    }
    res = mysql_store_result(&mysql);
    while ((row = mysql_fetch_row(res)))
    {
        memset(&RecDataStruct,0,sizeof(sendbag));
        memset(sendbuf,0,sizeof(sendbag));
        memset(find,0,50*sizeof(char));
        MYSQL_RES *restemp;
        MYSQL_ROW rowtemp;
        sprintf(find, "%s%s", "SELECT * FROM `iotnode` WHERE `id` = ", row[1]);
        if (mysql_real_query(&mysql, find, strlen(find)))
        {
            printf("Failed to query: %s\n", mysql_error(&mysql));
            return;
        }
        restemp =  mysql_store_result(&mysql);
        rowtemp = mysql_fetch_row(restemp);
        ///**************如果是RCI物联设备则对Ckcode ——> OUT或者ONL
        strcpy(d->TalktoID,rowtemp[1]);
        strcpy(d->info,rowtemp[2]);
        mysql_free_result(restemp);
        talktouser=FindOnlineUserOrIot(10,d->TalktoID,0);
        if(talktouser==NULL)
        {
            strcpy(RecDataStruct.checkcode,"OUT");
        }
        else
        {
            char IoTdata[30]="";
            strcpy(IoTdata,"SSS00000000000000000000000000");
            len=send(talktouser->USER_socket,IoTdata,30*sizeof(char),0);
            if(len==SOCKET_ERROR||len==0)
            {
                ///********************************
                CLN g;
                strcpy(g.USERID,talktouser->USERID);
                closesocket(talktouser->USER_socket);
                delete_out_user(&g);
                ///********************************
                strcpy(RecDataStruct.checkcode,"OUT");
                strcpy(RecDataStruct.DATA,talktouser->info);
            }
            else
            {
                strcpy(RecDataStruct.checkcode,"ONL");
                strcpy(RecDataStruct.DATA,talktouser->info);
            }
        }
        strcpy(RecDataStruct.TalktoID,d->TalktoID);
        strcpy(RecDataStruct.USERID,a->USERID);
        RecDataStruct.save[99]='\n';
        memcpy(sendbuf,&RecDataStruct,sizeof(RecDataStruct));
        len=send(a->remote_socket,sendbuf,sizeof(sendbag),0);
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(a->remote_socket);
            delete_out_user(a);
            return ;
        }
        Sleep(100);
    }
    mysql_free_result(res);
}
