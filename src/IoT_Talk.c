#include"../head/SERVER.h"
int IoTtalk(CLN* b)
{

}
/*
char *timenow;
    time_t t;//定义一个时间变量
   // USER user=NULL;
    char IoTdata[30]="";
    int len,signIN=0;
    CLN* a=(CLN*)b;
    SOCKET c=a->remote_socket;
    sendbag RecDataStruct;
    char sendbuf[sizeof(sendbag)]= {0};
    char tag[4];
    ///*************IoT*************
    while(1)
    {
        memset(tag,0,4);
        memset(&RecDataStruct,0,sizeof(RecDataStruct));
        len=recv(c,IoTdata,30*sizeof(char),0);///*****************************1
        if(len==SOCKET_ERROR||len==0)
        {
            printf("\n连接%I64d退出\n",c);
            closesocket(c);
            if(signIN)
            {
                printf("SIGNIN?2TIMES?");
                delete_out_user(a);
            }
            return 0;
        }
        strncpy(tag,IoTdata,3);///************************2
        if(!strcmp(tag,"RMI"))///******请求任务///*****暂时废弃
        {

            RequestIotEvent(a);

        }
        else if(!strcmp(tag,"TAI"))///****请求通知知任务情况
        {

            Stringcut(IoTdata,3,13,RecDataStruct.TalktoID);
            Stringcut(IoTdata,14,29,RecDataStruct.DATA);
            USER iot=FindOnlineUserOrIot(10,a->USERID,0);
            if(iot!=NULL)
            {
                strcpy(iot->info,RecDataStruct.DATA);
            }
            if(Check_Id_Pwd(0,a)!=-1)
            {
                USER talktouser=FindOnlineUserOrIot(10,RecDataStruct.TalktoID,0);
                if(talktouser==NULL)
                {
                    Message mes=(Message)malloc(sizeof(struct message));
                    memset(mes,0,sizeof(contact));

                    strcpy(mes->checkcode,"NMI");
                    strcpy(mes->TalktoID,a->USERID);
                    strcpy(mes->USERID,RecDataStruct.TalktoID);
                    strcpy(mes->data,RecDataStruct.DATA);
                    printf("用户%s不在线TAN",RecDataStruct.TalktoID);
                    char talkto[15]= {'0'};
                    strcpy(talkto,RecDataStruct.TalktoID);
                    strcat(talkto,"ME");
                    FILE* ss=fopen(talkto,"a+");///**********dakai
                    fwrite(mes,sizeof(struct message),1,ss);
                    fflush(ss);
                    fclose(ss);
                    free(mes);
                    strcpy(IoTdata,"TAN");
                    strcat(IoTdata,RecDataStruct.TalktoID);
                    len=send(c,IoTdata,30*sizeof(char),0);
                    if(len==SOCKET_ERROR||len==0)
                    {
                        printf("\n连接%I64d退出\n",c);
                        closesocket(c);
                        delete_out_user(a);
                        return 0;
                    }
                }
                else
                {
                    memset(sendbuf,0,sizeof(sendbag));
                    strcpy(RecDataStruct.checkcode,"TAI");
                    RecDataStruct.save[99]='\n';
                    char temp[12]= {0};
                    strcpy(temp,RecDataStruct.USERID);
                    printf("%s",temp);
                    strcpy(RecDataStruct.USERID,RecDataStruct.TalktoID);
                    strcpy(RecDataStruct.TalktoID,a->USERID);
                    memcpy(sendbuf,&RecDataStruct,sizeof(RecDataStruct));
                    len=send(talktouser->USER_socket,sendbuf,sizeof(sendbag),0);
                    if(len==SOCKET_ERROR||len==0)
                    {
                        printf("\n连接%I64d退出\n",c);
                        closesocket(talktouser->USER_socket);
                        return 0;
                    }
                    strcpy(IoTdata,"TAS");
                    strcat(IoTdata,RecDataStruct.USERID);
                    len=send(c,IoTdata,30*sizeof(char),0);
                    if(len==SOCKET_ERROR||len==0)
                    {
                        printf("\n连接%I64d退出\n",c);
                        closesocket(c);
                        delete_out_user(a);
                        return 0;
                    }
                }
            }
            else
            {
                printf("用户%s不存在",RecDataStruct.TalktoID);
                strcpy(IoTdata,"Taa");
                strcat(IoTdata,RecDataStruct.TalktoID);
                len=send(c,IoTdata,30*sizeof(char),0);
                if(len==SOCKET_ERROR||len==0)
                {
                    printf("\n连接%I64d退出\n",c);
                    closesocket(c);
                    delete_out_user(a);
                    return 0;
                }
            }
        }
        else if(!strcmp(tag,"ADS"))///****only reply****
        {
            Stringcut(IoTdata,3,13,a->TalktoID);
            a->TalktoID[11]='\0';

            Contact mes=(Contact)malloc(sizeof(struct contact));
            memset(mes,0,sizeof(contact));
            memset(sendbuf,0,sizeof(sendbag));
            strcpy(mes->checkcode,"RCO");
            strcpy(mes->TalktoID,RecDataStruct.TalktoID);
            strcpy(mes->USERID,a->USERID);

            RecDataStruct.save[99]='\n';
            memset(mes,0,sizeof(contact));
            strcpy(mes->checkcode,"RCI");///****物联网设备添加成功
            strcpy(mes->TalktoID,a->USERID);
            strcpy(mes->USERID,a->TalktoID);
            ///**********dakai

            char talkto[15]= "";
            strcpy(talkto,a->TalktoID);
            strcat(talkto,"CO");
            FILE* ss=fopen(talkto,"a+");///****dakai***

            fwrite(mes,sizeof(struct contact),1,ss);
            fflush(ss);
            fclose(ss);
            strcpy(IoTdata,"ADS");
            strcat(IoTdata,RecDataStruct.TalktoID);
            len=send(c,IoTdata,30*sizeof(char),0);
            if(len==SOCKET_ERROR||len==0)
            {
                printf("\n连接%I64d退出\n",c);
                closesocket(c);
                delete_out_user(a);
                return 0;
            }
        }
        else if(!strcmp(tag,"HBI"))///**************************************
        {
            t=time(NULL);
            timenow=ctime(&t);
            strcpy(IoTdata,"HBI");
            strncat(IoTdata,timenow,24);
            len=send(c,IoTdata,30*sizeof(char),0);
            printf("\nHBI\n");
            if(len==SOCKET_ERROR||len==0)
            {
                printf("\n连接%I64d退出\n",c);
                closesocket(c);
                delete_out_user(a);
                return 0;
            }
        }
        else if(!strcmp(tag,"STO"))
        {
            closesocket(c);
            delete_out_user(a);
        }
        else
        {
            printf("%s",IoTdata);
        }
    }
    */
