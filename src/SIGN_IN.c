#include"../head/SERVER.h"
#define SEND_STO_2_ALL_SERVER() CLN arg;\
            sprintf(arg.checkcode,"%s","STO");\
            arg.ADDR = a->ADDR;\
            sprintf(arg.TalktoID,"%s",a->USERID);\
            sprintf(arg.USERID,"%s",a->USERID);\
            arg.USERKEY_ID = a->USERKEY_ID;\
            Send2OnlineUserViaTopServer(arg);
extern int checkUserInMemWait4FreeList(char*);
int SIGNIN(CLN *a)
{
    if(strlen(a->USERID)>=11)
    {
        if(checkUserInMemWait4FreeList(a->USERID))
        {
            Sleep(500);
        }
        SOCKET asocket=a->SOCKET;
        int check=Check_Id_Pwd(0,a);
        USER checkuser= FindOnlineUserOrIot(0,a->USERID,0);
        if(check==-1)
        {
            return -1;
        }
        else if(checkuser==NULL)
        {
            SEND_STO_2_ALL_SERVER();//Tell All Server To Logout This User
            USER temp = FindRegisterUserOrIotNode(0,a->USERID,0);
            if(temp!=NULL)
            {
                a->USERKEY_ID=temp->USERKEY_ID;
            }
            else
            {
                return -1;
            }
            newOnlineUserOrIotDev(a);
            return 1;
        }
        else
        {
            if(!strcmp(a->checkcode,"SIB"))//Reconnect
            {
                USER temp = checkuser;
                pthread_mutex_lock(&(temp->mutex));
                temp->isReconnect = 1;
                temp->USER_ADDR = a->ADDR;
                closesocket(temp->USER_SOCKET);
                temp->USER_SOCKET = a->SOCKET;
                pthread_mutex_unlock(&(temp->mutex));
                return 1;
            }
            else
            {
                UserPacketInterface as;
                char sendbuf[sizeof(UserPacketInterface)]= {0};
                memset(&as,0,sizeof(UserPacketInterface));
                memset(sendbuf,0,sizeof(UserPacketInterface));
                strcpy(as.checkcode,"STO");
                as.save[99]=_HC_;
                memcpy(sendbuf,&as,sizeof(UserPacketInterface));
                send(checkuser->USER_SOCKET,sendbuf,sizeof(UserPacketInterface),0);
                checkuser->isReconnect = 1;
                checkuser->USER_SOCKET=asocket;
                checkuser->USER_ADDR=a->ADDR;
                return 1;
            }
        }
    }
    else
    {
        SOCKET asocket=a->SOCKET;
        int check=Check_Id_Pwd(0,a);
        USER checkuser= FindOnlineUserOrIot(10,a->USERID,0);
        if(check==-1)
        {
            return -1;
        }
        else if(checkuser==NULL)
        {
            USER temp = FindRegisterUserOrIotNode(10,a->USERID,0);
            if(temp!=NULL)
            {
                a->USERKEY_ID=temp->USERKEY_ID;
            }
            else
            {
                return -1;
            }
            newOnlineUserOrIotDev(a);
            return 1;
        }
        else//ONLINE
        {
            pthread_mutex_lock(&(checkuser->mutex));
            closesocket(checkuser->USER_SOCKET);
            checkuser->USER_SOCKET=asocket;
            checkuser->USER_ADDR=a->ADDR;
            pthread_mutex_unlock(&(checkuser->mutex));
            return 1;
        }
    }
}
