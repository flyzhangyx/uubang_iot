#include "../head/SERVER.h"
void CopyUserPacketInterface2Cln(UserPacketInterface UserPacketInterface,CLN *a)
{
    UserPacketInterface.checkcode[17]='\0';
    UserPacketInterface.DATA[512]='\0';
    UserPacketInterface.USERPASSWORD[32]='\0';
    UserPacketInterface.USERID[11]='\0';
    UserPacketInterface.REUSERPASSWORD[32]='\0';
    UserPacketInterface.TalktoID[11]='\0';
    UserPacketInterface.save[99]='\0';
    memcpy(a->checkcode,UserPacketInterface.checkcode,sizeof(char)*17);
    memcpy(a->data,UserPacketInterface.DATA,sizeof(char)*512);
    memcpy(a->USERPASSWORD,UserPacketInterface.USERPASSWORD,sizeof(char)*32);
    memcpy(a->USERID,UserPacketInterface.USERID,sizeof(char)*11);;
    memcpy(a->REUSERPASSWORD,UserPacketInterface.REUSERPASSWORD,sizeof(char)*32);
    memcpy(a->TalktoID,UserPacketInterface.TalktoID,sizeof(char)*11);
    memcpy(a->DATE,UserPacketInterface.save,sizeof(char)*99);
}
