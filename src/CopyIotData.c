#include "../head/SERVER.h"
void CopyRecIotData2Cln(char *recBuff,CLN* CONN,int len)
{
    IotPacketInterface temp;
    memset(&temp,0,sizeof(IotPacketInterface));
    memcpy(&temp,recBuff,len);
    temp.opCode[2] = 0;
    temp.SeqNum[1] = 0;
    sprintf(CONN->checkcode,"%s",temp.opCode);
    sprintf(CONN->DATE,"%s",temp.SeqNum);
    memcpy(CONN->data,temp.payLoad,len-5);
}
