#include"../head/SERVER.h"
int ReadOrDeleteUserScene(CLN *a,char* UserId,int CmdGroupId,int del)
{
    if(del==1)
    {
        char dele[150]="";
        sprintf(dele,"%s%d%s%d%s",
                "DELETE FROM `userscenecmd` WHERE `userscenecmd`.`CmdGroup` = '",
                CmdGroupId,
                "'AND `userscenecmd`.`fromUserId` = '",
                FindRegisterUserOrIotNode(0,UserId,0)->USERKEY_ID,
                "'");
                mysql_master_connect_ping();
        mysql_master_connect_ping();
        if(mysql_real_query(&mysql,dele,strlen(dele)))
        {
            printf("\nMySQL ERR(DEL SCENE CMD) :%s",mysql_error(&mysql));
            return 0;
        }
        return 1;
    }
    else
    {
        UserPacketInterface RecDataStruct;
        int len=0;
        char sendbuf[sizeof(UserPacketInterface)]= {0};
        char find[100] = "";
        MYSQL_RES *res;
        MYSQL_ROW row;
        sprintf(find, "%s%d", "SELECT * FROM `userscenecmd` WHERE `fromUserId` = ", FindRegisterUserOrIotNode(0,UserId,0)->USERKEY_ID);
        mysql_master_connect_ping();
        if (mysql_real_query(&mysql, find, strlen(find)))//No
        {
            printf("\n SQL ERR (REQ USER SCENE):%s",mysql_error(&mysql));
            return 0;
        }
        res = mysql_store_result(&mysql);
        while ((row = mysql_fetch_row(res)))
        {
            memset(&RecDataStruct,0,sizeof(UserPacketInterface));
            memset(sendbuf,0,sizeof(UserPacketInterface));
            USER Temp = FindRegisterUserOrIotNode(10,NULL,atoi(row[1]));
            if(Temp==NULL)
            {
                mysql_free_result(res);
                return 0;
            }
            printf("111");
            strcpy(RecDataStruct.checkcode,"SCENE DATA");
            strcpy(RecDataStruct.TalktoID,Temp->USERID);
            strcpy(RecDataStruct.USERID,a->USERID);
            sprintf(RecDataStruct.DATA,"%s|%s|%s|%s|%s|%s|%s",row[2],row[3],row[4],row[5],row[6],row[7],row[8]);
            RecDataStruct.save[99]='\n';
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
}
