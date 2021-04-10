#include "../head/SERVER.h"
int IotGetIotData(CLN *a,int ReadIotId)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    IotPacketInterface RecDataStruct;
    char sendbuf[sizeof(IotPacketInterface)]= {0};
    char find[100]="";
    sprintf(find,"%s%d","SELECT * FROM `iotevtcache` WHERE `iotId` =",ReadIotId);
    SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        return 0;
    }
    MYSQL *mysql=&(temmp->fd);
    if(mysql_real_query(mysql,find,strlen(find)))
    {
        release_node(MySqlConnPool, temmp);
        log_error(" SQL ERR (IOTGETIOTDATA):%s",mysql_error(mysql));
        return 0;
    }
    memset(&RecDataStruct,0,sizeof(IotPacketInterface));
    res = mysql_store_result(mysql);
    int len = 0;
    while ((row = mysql_fetch_row(res)))
    {
        memset(&RecDataStruct,0,sizeof(IotPacketInterface));
        memset(sendbuf,0,sizeof(IotPacketInterface));
        strcpy(RecDataStruct.opCode,"07");
        sprintf(RecDataStruct.payLoad,"%d_%d_%d_%s_%s_",atoi(row[0]),atoi(row[1]),atoi(row[2]),row[3],row[4]);//
        Encrypt(RecDataStruct.payLoad,strlen(RecDataStruct.payLoad),a->Pin,RecDataStruct.payLoad);//Encrypt
        InterlockedIncrement((LPLONG) &(a->conn->SeqNum));
        RecDataStruct.SeqNum[0] = a->conn->SeqNum;
        if(a->conn->SeqNum==127)
        {
            a->conn->SeqNum = 1;
        }
        memcpy(sendbuf,&RecDataStruct,sizeof(RecDataStruct));
        sendbuf[sizeof(IotPacketInterface)-1] = _HC_;
        len = SyncSend(a->SOCKET,sendbuf,sizeof(IotPacketInterface),&(a->conn->t));
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
