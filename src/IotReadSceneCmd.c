#include"../head/SERVER.h"
int IotReadSelfSceneCmd(CLN *a,int Iot_key)
{
    IotPacketInterface RecDataStruct;
    int len=0;
    char sendbuf[sizeof(IotPacketInterface)]= {0};
    char find[100] = "";
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(find, "%s%d", "SELECT * FROM `userscenecmd` WHERE `iotid` = ", Iot_key);
    SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        log_error("SQL NODE NULL");
        return 0;
    }
    MYSQL *mysql=&(temmp->fd);
    if (mysql_real_query(mysql, find, strlen(find)))//No
    {
        log_error(" SQL ERR (REQ USER SCENE):%s",mysql_error(mysql));
        release_node(MySqlConnPool, temmp);
        return 0;
    }
    res = mysql_store_result(mysql);
    while ((row = mysql_fetch_row(res)))
    {
        memset(&RecDataStruct,0,sizeof(IotPacketInterface));
        memset(sendbuf,0,sizeof(IotPacketInterface));
        strcpy(RecDataStruct.opCode,"06");//SCE
        sprintf(RecDataStruct.payLoad,"%s_%s_%s_%s_%s_%s_%s_",row[2],row[3],row[4],row[5],row[6],row[7],row[8]);//devclass_cmdcontent_status_cmdTime_Exedate_cmdgroup_fromUserid
        Encrypt(RecDataStruct.payLoad,strlen(RecDataStruct.payLoad),a->Pin,RecDataStruct.payLoad);//Encrypt with Pin
        InterlockedIncrement((LPLONG) &(a->conn->SeqNum));
        RecDataStruct.SeqNum[0] = a->conn->SeqNum;
        if(a->conn->SeqNum==127)
        {
            a->conn->SeqNum = 1;
        }
        memcpy(sendbuf,&RecDataStruct,sizeof(RecDataStruct));
        sendbuf[sizeof(IotPacketInterface)-1] = _HC_;
        len=SyncSend(a->SOCKET,sendbuf,sizeof(IotPacketInterface),&(a->conn->t));
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(a->SOCKET);
            release_node(MySqlConnPool, temmp);
            mysql_free_result(res);
            return 0;
        }
    }
    mysql_free_result(res);
    release_node(MySqlConnPool, temmp);
    return 1;
}
