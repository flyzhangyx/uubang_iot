#include"../head/SERVER.h"
#include"../libmd5utils/md5.h"
int IoTtalk(CLN* a)
{
//    char str[100]="test";
//    //GetUpdateTimeStamp(0,1,str);
//    Sleep(50);
//    sprintf(b->USERID,"%s","123456");
//    USER iot = FindRegisterUserOrIotNode(10,"123456",0);
//    if(iot==NULL)
//        return 0;
//    IotReadSelfSceneCmd(b,iot->USERKEY_ID);
//    //send(b->SOCKET,str,strlen(str)+1,0);
//    //return 1;
    int len = 0;
    IotPacketInterface SendStruct;
    char SendBuf[sizeof(IotPacketInterface)] = "";
    memset(SendBuf,0,sizeof(IotPacketInterface));
    memset(&SendStruct,0,sizeof(IotPacketInterface));
    switch(atoi(a->checkcode))
    {
    case 1://RSA
    {
        InitRSA(&(a->key));//Create RSAKey, need srand(time(NULL)) first
        InitRSA(&(a->key));//twice ,it is a bug left to be fixed
        a->conn->key = a->key;
        log_debug("Public Key (%d,%d) | Private Key (%d,%d)",a->key.publicKey,a->key.commonKey,a->key.privateKey,a->key.commonKey);
        memset(SendBuf,0,sizeof(IotPacketInterface));
        memset(&SendStruct,0,sizeof(IotPacketInterface));
        strcpy(SendStruct.opCode,"01");
        sprintf(SendStruct.payLoad,"%d_%d_%d_",a->key.publicKey,a->key.commonKey,a->key.encryptBlockBytes);
        InterlockedIncrement((LPLONG) &(a->conn->SeqNum));
        SendStruct.SeqNum[0] = a->conn->SeqNum;
        if(a->conn->SeqNum==127)
        {
            a->conn->SeqNum = 1;
        }
        memcpy(SendBuf,&SendStruct,sizeof(IotPacketInterface));
        SendBuf[sizeof(IotPacketInterface)-1] = _HC_;
        len=send(a->SOCKET,SendBuf,sizeof(IotPacketInterface),0);
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(a->SOCKET);
            return 0;
        }
    }
    break;
    case 2://PIN
    {
        if(a->key.encryptBlockBytes==0)
        {
            return 0;
        }
        generateRandIntStr(a->Pin,6);
        log_info("%s",a->Pin);
        memcpy(a->conn->Pin,a->Pin,7);
        memset(&SendStruct,0,sizeof(IotPacketInterface));
        memset(SendBuf,0,sizeof(IotPacketInterface));
        int encodedCrypto[100]= {0};
        memcpy(encodedCrypto,a->data,sizeof(int)*6*a->key.encryptBlockBytes);
        encodeMessage(6, a->key.encryptBlockBytes,a->Pin,encodedCrypto,a->key.privateKey, a->key.commonKey);
        char encodedCryptoByte[100];
        memcpy(encodedCryptoByte,encodedCrypto,6*a->key.encryptBlockBytes*sizeof(INT32));
        a->Pin[6]=0;
        char MD5Temp[33]="";
        Compute_string_md5((unsigned char *)MD5Temp,32,a->Pin);
        strcpy(SendStruct.opCode,"02");
        sprintf(SendStruct.payLoad,"%s_%s_",encodedCryptoByte,MD5Temp);
        InterlockedIncrement((LPLONG) &(a->conn->SeqNum));
        SendStruct.SeqNum[0] = a->conn->SeqNum;
        if(a->conn->SeqNum==127)
        {
            a->conn->SeqNum = 1;
        }
        memcpy(SendBuf,&SendStruct,sizeof(IotPacketInterface));
        SendBuf[sizeof(IotPacketInterface)-1] = _HC_;
        len=send(a->SOCKET,SendBuf,sizeof(IotPacketInterface),0);
        if(len==SOCKET_ERROR||len==0)
        {
            closesocket(a->SOCKET);
            return 0;
        }
    }
    break;
    case 3://REG_SIGN
    {
        int OutStrSize = 0;
        char** outStr = StrSplit(a->data,&OutStrSize,'_');
        sprintf(a->USERID,"%s", outStr[0]);
        sprintf(a->USERPASSWORD,"%s",outStr[1]);
        Register(a,1);
        if(a->conn->info[0]!='Y')
        {
            if(SIGNIN(a)==1)
            {
                a->info[0] = 'Y';
                a->conn->info[0] = 'Y';
                sprintf(a->conn->USERID,"%s", outStr[0]);
                if(FindRegisterUserOrIotNode(0,a->USERID,0)!=NULL)
                {
                    a->USERKEY_ID = FindRegisterUserOrIotNode(0,a->USERID,0)->USERKEY_ID;
                    a->conn->USERKEY_ID = a->USERKEY_ID;
                }
                strcpy(SendStruct.opCode,"03");
                InterlockedIncrement((LPLONG) &(a->conn->SeqNum));
                SendStruct.SeqNum[0] = a->conn->SeqNum;
                if(a->conn->SeqNum==127)
                {
                    a->conn->SeqNum = 1;
                }
                memcpy(SendBuf,&SendStruct,sizeof(IotPacketInterface));
                SendBuf[sizeof(IotPacketInterface)-1] = _HC_;
                len=send(a->SOCKET,SendBuf,sizeof(IotPacketInterface),0);
                if(len==SOCKET_ERROR||len==0)
                {
                    closesocket(a->SOCKET);
                }
            }
            else
            {
                strcpy(SendStruct.opCode,"23");
                InterlockedIncrement((LPLONG) &(a->conn->SeqNum));
                SendStruct.SeqNum[0] = a->conn->SeqNum;
                if(a->conn->SeqNum==127)
                {
                    a->conn->SeqNum = 1;
                }
                memcpy(SendBuf,&SendStruct,sizeof(IotPacketInterface));
                SendBuf[sizeof(IotPacketInterface)-1] = _HC_;
                len=send(a->SOCKET,SendBuf,sizeof(IotPacketInterface),0);
                if(len==SOCKET_ERROR||len==0)
                {
                    closesocket(a->SOCKET);
                }
            }
        }
        else
        {
            strcpy(SendStruct.opCode,"23");
            InterlockedIncrement((LPLONG) &(a->conn->SeqNum));
            SendStruct.SeqNum[0] = a->conn->SeqNum;
            if(a->conn->SeqNum==127)
            {
                a->conn->SeqNum = 1;
            }
            memcpy(SendBuf,&SendStruct,sizeof(IotPacketInterface));
            SendBuf[sizeof(IotPacketInterface)-1] = _HC_;
            len=send(a->SOCKET,SendBuf,sizeof(IotPacketInterface),0);
            if(len==SOCKET_ERROR||len==0)
            {
                closesocket(a->SOCKET);
            }
        }
        releaseStr(outStr,OutStrSize);
    }
    break;
    case 4://UPDATE DATA
        break;
    case 5://READ CMD
        break;
    case 6://READ SCENE CMD
        break;
    case 7:
        break;
    default:
        break;
    }
    return 1;
}
///*
//char *timenow;
//    time_t t;//定义一个时间变量
//   // USER user=NULL;
//    char IoTdata[30]="";
//    int len,signIN=0;
//    CLN* a=(CLN*)b;
//    SOCKET c=a->SOCKET;
//    sendbag RecDataStruct;
//    char SendBuf[sizeof(sendbag)]= {0};
//    char tag[4];
//    ///*************IoT*************
//    while(1)
//    {
//        memset(tag,0,4);
//        memset(&RecDataStruct,0,sizeof(RecDataStruct));
//        len=recv(c,IoTdata,30*sizeof(char),0);///*****************************1
//        if(len==SOCKET_ERROR||len==0)
//        {
//            log_info("连接%I64d退出",c);
//            closesocket(c);
//            if(signIN)
//            {
//                log_info("SIGNIN?2TIMES?");
//                delete_out_user(a);
//            }
//            return 0;
//        }
//        strncpy(tag,IoTdata,3);///************************2
//        if(!strcmp(tag,"RMI"))///******请求任务///*****暂时废弃
//        {
//
//            RequestIotEvent(a);
//
//        }
//        else if(!strcmp(tag,"TAI"))///****请求通知知任务情况
//        {
//
//            Stringcut(IoTdata,3,13,RecDataStruct.TalktoID);
//            Stringcut(IoTdata,14,29,RecDataStruct.DATA);
//            USER iot=FindOnlineUserOrIot(10,a->USERID,0);
//            if(iot!=NULL)
//            {
//                strcpy(iot->info,RecDataStruct.DATA);
//            }
//            if(Check_Id_Pwd(0,a)!=-1)
//            {
//                USER talktouser=FindOnlineUserOrIot(10,RecDataStruct.TalktoID,0);
//                if(talktouser==NULL)
//                {
//                    Message mes=(Message)malloc(sizeof(struct message));
//                    memset(mes,0,sizeof(contact));
//
//                    strcpy(mes->checkcode,"NMI");
//                    strcpy(mes->TalktoID,a->USERID);
//                    strcpy(mes->USERID,RecDataStruct.TalktoID);
//                    strcpy(mes->data,RecDataStruct.DATA);
//                    log_info("用户%s不在线TAN",RecDataStruct.TalktoID);
//                    char talkto[15]= {'0'};
//                    strcpy(talkto,RecDataStruct.TalktoID);
//                    strcat(talkto,"ME");
//                    FILE* ss=fopen(talkto,"a+");///**********dakai
//                    fwrite(mes,sizeof(struct message),1,ss);
//                    fflush(ss);
//                    fclose(ss);
//                    free(mes);
//                    strcpy(IoTdata,"TAN");
//                    strcat(IoTdata,RecDataStruct.TalktoID);
//                    len=send(c,IoTdata,30*sizeof(char),0);
//                    if(len==SOCKET_ERROR||len==0)
//                    {
//                        log_info("连接%I64d退出",c);
//                        closesocket(c);
//                        delete_out_user(a);
//                        return 0;
//                    }
//                }
//                else
//                {
//                    memset(SendBuf,0,sizeof(sendbag));
//                    strcpy(RecDataStruct.checkcode,"TAI");
//                    RecDataStruct.save[99]=_HC_;
//                    char temp[12]= {0};
//                    strcpy(temp,RecDataStruct.USERID);
//                    log_info("%s",temp);
//                    strcpy(RecDataStruct.USERID,RecDataStruct.TalktoID);
//                    strcpy(RecDataStruct.TalktoID,a->USERID);
//                    memcpy(SendBuf,&RecDataStruct,sizeof(RecDataStruct));
//                    len=send(talktouser->USER_SOCKET,SendBuf,sizeof(sendbag),0);
//                    if(len==SOCKET_ERROR||len==0)
//                    {
//                        log_info("连接%I64d退出",c);
//                        closesocket(talktouser->USER_SOCKET);
//                        return 0;
//                    }
//                    strcpy(IoTdata,"TAS");
//                    strcat(IoTdata,RecDataStruct.USERID);
//                    len=send(c,IoTdata,30*sizeof(char),0);
//                    if(len==SOCKET_ERROR||len==0)
//                    {
//                        log_info("连接%I64d退出",c);
//                        closesocket(c);
//                        delete_out_user(a);
//                        return 0;
//                    }
//                }
//            }
//            else
//            {
//                log_info("用户%s不存在",RecDataStruct.TalktoID);
//                strcpy(IoTdata,"Taa");
//                strcat(IoTdata,RecDataStruct.TalktoID);
//                len=send(c,IoTdata,30*sizeof(char),0);
//                if(len==SOCKET_ERROR||len==0)
//                {
//                    log_info("连接%I64d退出",c);
//                    closesocket(c);
//                    delete_out_user(a);
//                    return 0;
//                }
//            }
//        }
//        else if(!strcmp(tag,"ADS"))///****only reply****
//        {
//            Stringcut(IoTdata,3,13,a->TalktoID);
//            a->TalktoID[11]='\0';
//
//            Contact mes=(Contact)malloc(sizeof(struct contact));
//            memset(mes,0,sizeof(contact));
//            memset(SendBuf,0,sizeof(sendbag));
//            strcpy(mes->checkcode,"RCO");
//            strcpy(mes->TalktoID,RecDataStruct.TalktoID);
//            strcpy(mes->USERID,a->USERID);
//
//            RecDataStruct.save[99]=_HC_;
//            memset(mes,0,sizeof(contact));
//            strcpy(mes->checkcode,"RCI");///****物联网设备添加成功
//            strcpy(mes->TalktoID,a->USERID);
//            strcpy(mes->USERID,a->TalktoID);
//            ///**********dakai
//
//            char talkto[15]= "";
//            strcpy(talkto,a->TalktoID);
//            strcat(talkto,"CO");
//            FILE* ss=fopen(talkto,"a+");///****dakai***
//
//            fwrite(mes,sizeof(struct contact),1,ss);
//            fflush(ss);
//            fclose(ss);
//            strcpy(IoTdata,"ADS");
//            strcat(IoTdata,RecDataStruct.TalktoID);
//            len=send(c,IoTdata,30*sizeof(char),0);
//            if(len==SOCKET_ERROR||len==0)
//            {
//                log_info("连接%I64d退出",c);
//                closesocket(c);
//                delete_out_user(a);
//                return 0;
//            }
//        }
//        else if(!strcmp(tag,"HBI"))///**************************************
//        {
//            t=time(NULL);
//            timenow=ctime(&t);
//            strcpy(IoTdata,"HBI");
//            strncat(IoTdata,timenow,24);
//            len=send(c,IoTdata,30*sizeof(char),0);
//            log_info("HBI");
//            if(len==SOCKET_ERROR||len==0)
//            {
//                log_info("连接%I64d退出",c);
//                closesocket(c);
//                delete_out_user(a);
//                return 0;
//            }
//        }
//        else if(!strcmp(tag,"STO"))
//        {
//            closesocket(c);
//            delete_out_user(a);
//        }
//        else
//        {
//            log_info("%s",IoTdata);
//        }
//    }
//    */
