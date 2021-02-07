#include "../head/SERVER.h"
void CopyCln2Sendbag(CLN a,sendbag *Sendbag)
{
    strcpy(Sendbag->checkcode,a.checkcode);//,sizeof(char)*17);
    Sendbag->checkcode[17]='\0';
    strcpy(Sendbag->DATA,a.data);//,sizeof(char)*512);
    Sendbag->DATA[512]='\0';
    strcpy(Sendbag->USERPASSWORD,a.USERPASSWORD);//,sizeof(char)*32);
    Sendbag->USERPASSWORD[32]='\0';
    strcpy(Sendbag->USERID,a.USERID);//,sizeof(char)*11);
    Sendbag->USERID[11]='\0';
    strcpy(Sendbag->REUSERPASSWORD,a.REUSERPASSWORD);//,sizeof(char)*32);
    Sendbag->REUSERPASSWORD[32]='\0';
    strcpy(Sendbag->TalktoID,a.TalktoID);//,sizeof(char)*11);
    Sendbag->TalktoID[11]='\0';
    strcpy(Sendbag->save,a.DATE);//,sizeof(char)*99);
    Sendbag->save[99]='\0';
}

void CopySendbag2Cln(sendbag Sendbag,CLN *a)
{
    Sendbag.checkcode[17]='\0';
    Sendbag.DATA[512]='\0';
    Sendbag.USERPASSWORD[32]='\0';
    Sendbag.USERID[11]='\0';
    Sendbag.REUSERPASSWORD[32]='\0';
    Sendbag.TalktoID[11]='\0';
    Sendbag.save[99]='\0';
    strcpy(a->checkcode,Sendbag.checkcode);//,sizeof(char)*17);
    strcpy(a->data,Sendbag.DATA);//,sizeof(char)*512);
    strcpy(a->USERPASSWORD,Sendbag.USERPASSWORD);//,sizeof(char)*32);
    strcpy(a->USERID,Sendbag.USERID);//,sizeof(char)*11);;
    strcpy(a->REUSERPASSWORD,Sendbag.REUSERPASSWORD);//,sizeof(char)*32);
    strcpy(a->TalktoID,Sendbag.TalktoID);//,sizeof(char)*11);
    strcpy(a->DATE,Sendbag.save);//,sizeof(char)*99);
}
