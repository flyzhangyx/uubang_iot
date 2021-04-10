#include"../head/SERVER.h"
#define SEND_STO_2_ALL_SERVER() do{CLN arg;\
            sprintf(arg.checkcode,"%s","STO");\
            arg.ADDR = a->ADDR;\
            sprintf(arg.TalktoID,"%s",a->USERID);\
            sprintf(arg.USERID,"%s",a->USERID);\
            sprintf(arg.data,"%s",inet_ntoa((a->ADDR.sin_addr)));\
            arg.USERKEY_ID = a->USERKEY_ID;\
            Send2OnlineUserViaTopServer(arg);}while(0)

#define SEND_HBA() do{UserPacketInterface as;\
                char sendbuf[sizeof(UserPacketInterface)]= {0};\
                memset(&as,0,sizeof(UserPacketInterface));\
                memset(sendbuf,0,sizeof(UserPacketInterface));\
                strcpy(as.checkcode,"STO");\
                as.save[99]=_HC_;\
                memcpy(sendbuf,&as,sizeof(UserPacketInterface));\
                SyncSend(checkuser->USER_SOCKET,sendbuf,sizeof(UserPacketInterface),&(checkuser->CONNHANDLE->t));}while(0)

int SIGNIN(CLN *a)
{
    if(strlen(a->USERID)>=11)
    {
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
                a->conn->USERKEY_ID = temp->USERKEY_ID;
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
            CLN b;
            a->USERKEY_ID = checkuser->USERKEY_ID;
            memcpy(&b,a,sizeof(CLN));
            b.SOCKET = checkuser->USER_SOCKET;
            delete_out_user(&b);
            SEND_HBA();
            newOnlineUserOrIotDev(a);
            return 1;
        }
    }
    else
    {
        int check=Check_Id_Pwd(0,a);
        USER checkuser= FindOnlineUserOrIot(10,a->USERID,0);
        if(check==-1)
        {
            return -1;
        }
        if(checkuser == NULL )
        {
            USER temp = FindRegisterUserOrIotNode(10,a->USERID,0);
            if(temp!=NULL)
            {
                a->USERKEY_ID=temp->USERKEY_ID;
                a->conn->USERKEY_ID = temp->USERKEY_ID;
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
            CLN b;
            USER temp = FindRegisterUserOrIotNode(10,a->USERID,0);
            if(temp!=NULL)
            {
                a->USERKEY_ID=temp->USERKEY_ID;
                a->conn->USERKEY_ID = temp->USERKEY_ID;
            }
            memcpy(&b,a,sizeof(CLN));
            b.SOCKET = checkuser->USER_SOCKET;
            delete_out_user(&b);
            newOnlineUserOrIotDev(a);
            return 1;
        }
    }
}
