#include"../head/SERVER.h"
#define SEND_STO_2_ALL_SERVER() do{CLN arg;\
            sprintf(arg.checkcode,"%s","STO");\
            arg.ADDR = a->ADDR;\
            sprintf(arg.TalktoID,"%s",a->USERID);\
            sprintf(arg.USERID,"%s",a->USERID);\
            arg.USERKEY_ID = a->USERKEY_ID;\
            Send2OnlineUserViaTopServer(arg);}while(0)

#define SEND_HBA() do{UserPacketInterface as;\
                char sendbuf[sizeof(UserPacketInterface)]= {0};\
                memset(&as,0,sizeof(UserPacketInterface));\
                memset(sendbuf,0,sizeof(UserPacketInterface));\
                strcpy(as.checkcode,"STO");\
                as.save[99]=_HC_;\
                memcpy(sendbuf,&as,sizeof(UserPacketInterface));\
                send(checkuser->USER_SOCKET,sendbuf,sizeof(UserPacketInterface),0);}while(0)

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
            SOCKET temp = checkuser->USER_SOCKET;
            checkuser->USER_SOCKET=asocket;
            checkuser->USER_ADDR=a->ADDR;
            closesocket(temp);
            return 1;
        }
    }
}
