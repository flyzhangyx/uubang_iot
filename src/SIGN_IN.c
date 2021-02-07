#include"../head/SERVER.h"
int SIGNIN(CLN *a)
{
    if(strlen(a->USERID)>=11)
    {
        SOCKET asocket=a->remote_socket;
        int check=Check_Id_Pwd(0,a);
        USER checkuser= FindOnlineUserOrIot(0,a->USERID,0);
        if(check==-1)
        {
            return -1;
        }
        else if(check!=-1&&(checkuser==NULL))
        {
            USER temp = FindRegisterUserOrIotNode(0,a->USERID,0);
            if(temp!=NULL)
            {
                a->USERKEY_ID=temp->USERKEY_ID;
            }
            newOnlineUserOrIotDev(a);
            return 1;
        }
        else if(check!=-1&&(checkuser!=NULL)&&strcmp("CH123457",a->checkcode))
        {
            printf("%s|%s",a->checkcode,a->USERID);
            perror("online");
            sendbag as;
            char sendbuf[sizeof(sendbag)]= {0};
            memset(&as,0,sizeof(sendbag));
            memset(sendbuf,0,sizeof(sendbag));
            strcpy(as.checkcode,"STO");
            as.save[99]='\n';
            memcpy(sendbuf,&as,sizeof(sendbag));
            send(checkuser->USER_socket,sendbuf,sizeof(sendbag),0);
            closesocket(checkuser->USER_socket);
            checkuser->USER_socket=asocket;
            checkuser->USER_socket_udp=a->ADDR;
            return 1;
        }
        else if(check!=-1&&(checkuser!=NULL)&&!strcmp("CH123457",a->checkcode))
        {
            if(!strncmp(inet_ntoa(checkuser->USER_socket_udp.sin_addr),inet_ntoa(a->ADDR.sin_addr),8))
            {
                closesocket(checkuser->USER_socket);
                USER temp = FindRegisterUserOrIotNode(0,a->USERID,0);
                if(temp!=NULL)
                {
                    a->USERKEY_ID=temp->USERKEY_ID;
                }
                newOnlineUserOrIotDev(a);
            }
            else
            {
                USER temp = FindRegisterUserOrIotNode(0,a->USERID,0);
                if(temp!=NULL)
                {
                    a->USERKEY_ID=temp->USERKEY_ID;
                }
                delete_out_user(a);
                newOnlineUserOrIotDev(a);
            }
            return 1;
        }
        return 5;
    }
    else
    {
        SOCKET asocket=a->remote_socket;
        int check=Check_Id_Pwd(0,a);
        USER checkuser= FindOnlineUserOrIot(10,a->USERID,0);
        if(check==-1)
        {
            return -1;
        }
        else if(check!=-1&&(checkuser==NULL))
        {
            USER temp = FindRegisterUserOrIotNode(0,a->USERID,0);
            if(temp!=NULL)
            {
                a->USERKEY_ID=temp->USERKEY_ID;
            }
            newOnlineUserOrIotDev(a);
            return 1;
        }
        else if(check!=-1&&(checkuser!=NULL)&&strcmp("CH123457",a->checkcode))
        {
            printf("%s|%s",a->checkcode,a->USERID);
            perror("online");
            sendbag as;
            char sendbuf[sizeof(sendbag)]= {0};
            memset(&as,0,sizeof(sendbag));
            memset(sendbuf,0,sizeof(sendbag));
            strcpy(as.checkcode,"STO");
            as.save[99]='\n';
            memcpy(sendbuf,&as,sizeof(sendbag));
            send(checkuser->USER_socket,sendbuf,sizeof(sendbag),0);
            closesocket(checkuser->USER_socket);
            checkuser->USER_socket=asocket;
            checkuser->USER_socket_udp=a->ADDR;
            return 1;
        }
        else if(check!=-1&&(checkuser!=NULL)&&!strcmp("CH123457",a->checkcode))
        {
            if(!strncmp(inet_ntoa(checkuser->USER_socket_udp.sin_addr),inet_ntoa(a->ADDR.sin_addr),8))
            {
                closesocket(checkuser->USER_socket);
                USER temp = FindRegisterUserOrIotNode(0,a->USERID,0);
                if(temp!=NULL)
                {
                    a->USERKEY_ID=temp->USERKEY_ID;
                }
                newOnlineUserOrIotDev(a);
            }
            else
            {
                USER temp = FindRegisterUserOrIotNode(0,a->USERID,0);
                if(temp!=NULL)
                {
                    a->USERKEY_ID=temp->USERKEY_ID;
                }
                delete_out_user(a);
                newOnlineUserOrIotDev(a);
            }
            return 1;
        }
        return 5;
    }
}
