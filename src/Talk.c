#include"../head/SERVER.h"
#include"../head/md5.h"
#ifdef STPOOL
int talk(struct sttask *ptask)
#else
int talk(LPVOID b)
#endif
{
    int len,signIN=0,creat_check_alive=0;
#ifdef STPOOL
    if(ptask->task_arg==NULL)
    {
        printf("ERR");
        return 0;
    }
    CLN* a=(CLN*)ptask->task_arg;
#else
    CLN* a = (CLN*)b;
#endif
    char logcat[256]="";
    sendbag SendDataStruct;
    char sendbuf[sizeof(sendbag)]= {0};
    char tag[4];
    memset(tag,0,3);
    memset(&SendDataStruct,0,sizeof(sendbag));
    SOCKET c=a->remote_socket;
    strncpy(tag,a->checkcode,3);
    tag[3]='\0';
    memset(logcat,0,100*sizeof(char));
    strcpy(logcat,inet_ntoa(a->ADDR.sin_addr));
    strcat(logcat,"|");
    strcat(logcat,a->checkcode);
    logwrite(logcat);
    if(FindRegisterUserOrIotNode(0,a->USERID,0)!=NULL)
        a->USERKEY_ID = FindRegisterUserOrIotNode(0,a->USERID,0)->USERKEY_ID;
    ///**********************验证是否为合法用户***************************
    signIN = (a->info[0]=='Y');//whether User had Signed in
    ///************************循环接受用户请求******************************
    if(signIN&&a->info[0]!='N'&&!creat_check_alive)
    {
        creat_check_alive=1;
        //CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Check_alive,a,0,NULL);
    }
    switch (DJBHash(tag, 3))
    {
    case 12: //HBA
    {
        memset(&SendDataStruct, 0, sizeof(sendbag));
        memset(sendbuf, 0, sizeof(sendbag));
        strcpy(SendDataStruct.checkcode, "HBA");
        SendDataStruct.save[99] = '\n';
        memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
        len = send(c, sendbuf, sizeof(sendbag), 0);
        if (len == SOCKET_ERROR || len == 0)
        {
            closesocket(c);
            return 0;
        }
    }
    case 69099://RSA
    {
        InitRSA(&(a->key));//Create RSAKey, need srand(time(NULL)) first
        InitRSA(&(a->key));//twice ,it is a bug left to be fixed
        printf("\nPublic Key (%d,%d) | Private Key (%d,%d)\n",a->key.publicKey,a->key.commonKey,a->key.privateKey,a->key.commonKey);
        memset(&SendDataStruct,0,sizeof(sendbag));
        memset(sendbuf,0,sizeof(sendbag));
        strcpy(SendDataStruct.checkcode,"RSA");
        sprintf(SendDataStruct.DATA,"%d|%d|%d",a->key.publicKey,a->key.commonKey,a->key.encryptBlockBytes);
        SendDataStruct.save[99]='\n';
        memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
        len=send(a->remote_socket,sendbuf,sizeof(sendbag),0);
        if(len==SOCKET_ERROR||len==0)
        {
            printf("\n连接%I64d退出\n",c);
            closesocket(c);
            delete_out_user(a);
            return 0;
        }
    }
    break;
    case 66604://PIN
    {
        generateRandIntStr(a->Pin,6);
        memset(&SendDataStruct,0,sizeof(sendbag));
        memset(sendbuf,0,sizeof(sendbag));
        strcpy(SendDataStruct.checkcode,"PIN");
         ///Send Email to inform User
        if(signIN==1)
        {
            int encodedCrypto[100]={0};
            memcpy(encodedCrypto,a->data,sizeof(int)*6*a->key.encryptBlockBytes);
            decodeMessage(6, a->key.encryptBlockBytes, encodedCrypto,a->Pin,a->key.privateKey, a->key.commonKey);
            a->Pin[5]=0;
            char MD5Temp[33]="";
            Compute_string_md5(MD5Temp,32,a->Pin);
            sprintf(SendDataStruct.DATA,"%s",MD5Temp);
        }
        else
        {
            char cmd[100]="";
            sprintf(cmd,"%s %s %s","cmd.exe /c \"PINSend.bat\"",a->info,a->Pin);
            printf("\n%s\n",cmd);
            system(cmd);
        }
        SendDataStruct.save[99]='\n';
        memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
        len=send(a->remote_socket,sendbuf,sizeof(sendbag),0);
        if(len==SOCKET_ERROR||len==0)
        {
            printf("\n连接%I64d退出\n",c);
            closesocket(c);
            delete_out_user(a);
            return 0;
        }
    }
    break;
    case 69858: //SIA
    {
        printf("\n%s/%s/\n", a->USERID, a->USERPASSWORD);
        int encodedCrypto[100]={0};
        memcpy(encodedCrypto,a->data,sizeof(int)*32*a->key.encryptBlockBytes);
        decodeMessage(32, a->key.encryptBlockBytes, encodedCrypto,a->USERPASSWORD,a->key.privateKey, a->key.commonKey);
        a->USERPASSWORD[32]=0;
        if (SIGNIN(a) == 1)
        {
            signIN = 1;
            a->info[0] = 'Y';
            memset(&SendDataStruct, 0, sizeof(sendbag));
            memset(sendbuf, 0, sizeof(sendbag));
            strcpy(SendDataStruct.checkcode, "SIA");
            SendDataStruct.save[99] = '\n';
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = send(c, sendbuf, sizeof(sendbag), 0);
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);
                delete_out_user(a);
                return 0;
            }
        }
        else
        {
            memset(&SendDataStruct, 0, sizeof(sendbag));
            memset(sendbuf, 0, sizeof(sendbag));
            strcpy(SendDataStruct.checkcode, "SiA");
            SendDataStruct.save[99] = '\n';
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = send(c, sendbuf, sizeof(sendbag), 0);
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);
                return 0;
            }
        }
    }
    break;
    case 68637: //REA
    {
        UpdateLocalRegUserAndIotlist();
        char PinMd5[33]="";
        Compute_string_md5((unsigned char *)a->Pin,6,PinMd5);
        if(!strcmp(PinMd5,a->USERPASSWORD))
        {
            memset(&SendDataStruct, 0, sizeof(sendbag));
            memset(sendbuf, 0, sizeof(sendbag));
            strcpy(SendDataStruct.checkcode, "RPE");//Reg PinCode Err
            SendDataStruct.save[99] = '\n';
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = send(c, sendbuf, sizeof(SendDataStruct), 0);
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);
                return 0;
            }
        }
        int encodedCrypto[96]={0};
        memcpy(encodedCrypto,a->data,32*a->key.encryptBlockBytes*sizeof(int));
        decodeMessage(32, a->key.encryptBlockBytes, encodedCrypto,a->USERPASSWORD,a->key.privateKey, a->key.commonKey);//RSA Decrypt Password
        Decrypt(a->USERPASSWORD,32,a->Pin,a->USERPASSWORD);//Kaisa Decrypt
        if (Register(a, 0) == 1)
        {
            memset(&SendDataStruct, 0, sizeof(sendbag));
            memset(sendbuf, 0, sizeof(sendbag));
            strcpy(SendDataStruct.checkcode, "REA");
            SendDataStruct.save[99] = '\n';
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = send(c, sendbuf, sizeof(SendDataStruct), 0);
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);
                return 0;
            }
        }
        else
        {
            memset(&SendDataStruct, 0, sizeof(sendbag));
            memset(sendbuf, 0, sizeof(sendbag));
            strcpy(SendDataStruct.checkcode, "ReA");
            SendDataStruct.save[99] = '\n';
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = send(c, sendbuf, sizeof(sendbag), 0);
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);
                return 0;
            }
        }
    }
    break;
    case 68905: //RME
        break;
    case 70683: //TAA
    {
        if (Check_Id_Pwd(0, a) != -1)
        {
            USER talktouser = FindOnlineUserOrIot(0, SendDataStruct.TalktoID, 0);
            if (talktouser == NULL)
            {
                USER find = FindRegisterUserOrIotNode(0, a->TalktoID, 0);
                if (find == NULL)
                {
                    return 0;
                }
                NewUserMsgStorage(a, find->USERKEY_ID);
                memset(&SendDataStruct, 0, sizeof(sendbag));
                memset(sendbuf, 0, sizeof(sendbag));
                strcpy(SendDataStruct.checkcode, "TAN"); //TA but not online
                SendDataStruct.save[99] = '\n';
                memcpy(sendbuf, &SendDataStruct, sizeof(sendbag));
                len = send(c, sendbuf, sizeof(sendbag), 0);
                if (len == SOCKET_ERROR || len == 0)
                {
                    closesocket(c);
                    delete_out_user(a);
                    return 0;
                }
            }
            else
            {
                memset(sendbuf, 0, sizeof(sendbag));
                strcpy(SendDataStruct.checkcode, "TAT");
                SendDataStruct.save[99] = '\n';
                char temp[12] = {0};
                strcpy(temp, SendDataStruct.USERID);
                strcpy(SendDataStruct.USERID, SendDataStruct.TalktoID);
                strcpy(SendDataStruct.TalktoID, temp);
                memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
                len = send(talktouser->USER_socket, sendbuf, sizeof(sendbag), 0);
                if (len == SOCKET_ERROR || len == 0)
                {
                    closesocket(talktouser->USER_socket);
                    delete_out_user(a);
                }
                USER find = FindRegisterUserOrIotNode(0, a->TalktoID, 0);
                if (find == NULL)
                {
                    return 0;
                }
                NewUserMsgStorage(a, find->USERKEY_ID);
                memset(&SendDataStruct, 0, sizeof(sendbag));
                memset(sendbuf, 0, sizeof(sendbag));
                strcpy(SendDataStruct.checkcode, "TAS");
                strcpy(SendDataStruct.TalktoID, a->TalktoID);
                strcpy(SendDataStruct.USERID, a->USERID);
                SendDataStruct.save[99] = '\n';
                memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
                len = send(c, sendbuf, sizeof(sendbag), 0);
                if (len == SOCKET_ERROR || len == 0)
                {

                    closesocket(c);
                    delete_out_user(a);
                    return 0;
                }
            }
        }
        else
        {
            memset(&SendDataStruct, 0, sizeof(sendbag));
            memset(sendbuf, 0, sizeof(sendbag));
            strcpy(SendDataStruct.checkcode, "Taa"); //TA but not online
            SendDataStruct.save[99] = '\n';
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = send(c, sendbuf, sizeof(sendbag), 0);
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);
                delete_out_user(a);
                return 0;
            }
        }
    }
    break;
    case 70235: //STO
    {
        memset(&SendDataStruct, 0, sizeof(sendbag));
        memset(sendbuf, 0, sizeof(sendbag));
        strcpy(SendDataStruct.checkcode, "STO");
        SendDataStruct.save[99] = '\n';
        memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
        len = send(a->remote_socket, sendbuf, sizeof(sendbag), 0);
        if (len == SOCKET_ERROR || len == 0)
        {

            closesocket(c);
            delete_out_user(a);
            return 0;
        }
        delete_out_user(a);
        printf("\n%s\n", SendDataStruct.checkcode);
        a->info[0] = 'N';
        creat_check_alive = 0;
    }
    break;
    case 72270: //UPD
    {
        memset(&SendDataStruct, 0, sizeof(sendbag));
        memset(sendbuf, 0, sizeof(sendbag));
        strcpy(SendDataStruct.checkcode, "UPD");
        strcpy(SendDataStruct.DATA, app_version);
        SendDataStruct.save[99] = '\n';
        memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
        len = send(c, sendbuf, sizeof(sendbag), 0);
    }
    break;
    case 50094: //ADD
    {
        CLN d;
        char IoTdevice[5] = "";
        int F = 0;
        strcpy(d.USERID, SendDataStruct.TalktoID);
        if (Check_Id_Pwd(1, &d) != -1)
        {
            strncpy(IoTdevice, SendDataStruct.checkcode, 4);
            IoTdevice[4] = '\0';
            if (!strcmp(IoTdevice, "ADDI"))
            {
                F = 1;
            }
            USER talktouser = FindOnlineUserOrIot(0, SendDataStruct.TalktoID, 0);
            if (talktouser == NULL && F != 1)
            {
                Message mes = (Message)malloc(sizeof(struct message));
                memset(mes, 0, sizeof(contact));
                memset(mes, 0, sizeof(contact));
                strcpy(mes->checkcode, "NCA");
                strcpy(mes->TalktoID, a->USERID);
                strcpy(mes->USERID, SendDataStruct.TalktoID);
                strcpy(mes->data, SendDataStruct.DATA);
                char talkto[30] = {'0'};
                strcpy(talkto, SendDataStruct.TalktoID);
                strcat(talkto, "/");
                strcat(talkto, SendDataStruct.TalktoID);
                strcat(talkto, "ME");
                memset(&SendDataStruct, 0, sizeof(sendbag));
                memset(sendbuf, 0, sizeof(sendbag));
                strcpy(SendDataStruct.checkcode, "ADN");
                //TA but not online
                strcpy(SendDataStruct.TalktoID, a->TalktoID);
                strcpy(SendDataStruct.USERID, a->USERID);
                SendDataStruct.save[99] = '\n';
                memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
                FILE *ss = fopen(talkto, "a+"); ///**********dakai
                fwrite(mes, sizeof(struct message), 1, ss);
                fflush(ss);
                fclose(ss);
                free(mes);
                len = send(c, sendbuf, sizeof(sendbag), 0);
                if (len == SOCKET_ERROR || len == 0)
                {

                    closesocket(c);
                    delete_out_user(a);
                    return 0;
                }
            }
            else if (talktouser != NULL && F != 1)
            {

                //memset(&SendDataStruct,0,sizeof(sendbag));
                memset(sendbuf, 0, sizeof(sendbag));
                strcpy(SendDataStruct.checkcode, "ADT");
                SendDataStruct.save[99] = '\n';
                char temp[12] = {0};
                strcpy(temp, SendDataStruct.USERID);
                strcpy(SendDataStruct.USERID, SendDataStruct.TalktoID);
                strcpy(SendDataStruct.TalktoID, temp);
                SendDataStruct.save[99] = '\n';
                memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
                len = send(talktouser->USER_socket, sendbuf, sizeof(sendbag), 0);
                if (len == SOCKET_ERROR || len == 0)
                {
                    closesocket(talktouser->USER_socket);
                    delete_out_user(a);
                    strcpy(temp, SendDataStruct.USERID);
                    strcpy(SendDataStruct.USERID, SendDataStruct.TalktoID);
                    strcpy(SendDataStruct.TalktoID, temp);
                }
                memset(&SendDataStruct, 0, sizeof(sendbag));
                memset(sendbuf, 0, sizeof(sendbag));
                strcpy(SendDataStruct.TalktoID, a->TalktoID);
                strcpy(SendDataStruct.USERID, a->USERID);
                strcpy(SendDataStruct.checkcode, "ADD");
                SendDataStruct.save[99] = '\n';
                memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
                len = send(c, sendbuf, sizeof(sendbag), 0);
                if (len == SOCKET_ERROR || len == 0)
                {

                    closesocket(c);
                    delete_out_user(a);
                    return 0;
                }
                ///********************
            }
            else if (F == 1 && talktouser != NULL)
            {
                char IoTcommand[30] = "";
                strcpy(IoTcommand, "ADD");
                strncat(IoTcommand, a->USERID, 20);
                len = send(talktouser->USER_socket, IoTcommand, 30 * sizeof(char), 0);
                // free(sendbuf);
                if (len == SOCKET_ERROR || len == 0)
                {
                    closesocket(talktouser->USER_socket);
                    return 0;
                }
            }
        }
        else
        {
            //memset(&SendDataStruct,0,sizeof(sendbag));
            memset(sendbuf, 0, sizeof(sendbag));
            strcpy(SendDataStruct.checkcode, "Add"); //TA but not online
            SendDataStruct.save[99] = '\n';
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = send(c, sendbuf, sizeof(sendbag), 0);
            // free(sendbuf);
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);
                delete_out_user(a);
                return 0;
            }
        }
    }
    break;
    case 50109: //ADS
    {
        Contact mes = (Contact)malloc(sizeof(struct contact));
        memset(mes, 0, sizeof(contact));
        memset(sendbuf, 0, sizeof(sendbag));
        strcpy(mes->checkcode, "RCO");
        strcpy(mes->TalktoID, SendDataStruct.TalktoID);
        strcpy(mes->USERID, SendDataStruct.USERID);
        SendDataStruct.save[99] = '\n';
        memset(mes, 0, sizeof(contact));
        strcpy(mes->checkcode, "RCO");
        strcpy(mes->TalktoID, SendDataStruct.USERID);
        strcpy(mes->USERID, SendDataStruct.TalktoID);
        ///**********dakai
        char talkto[30] = {'0'};
        strcpy(talkto, SendDataStruct.TalktoID);
        strcat(talkto, "/");
        strcat(talkto, SendDataStruct.TalktoID);
        strcat(talkto, "CO");
        FILE *ss = fopen(talkto, "a+"); ///**********dakai
        fwrite(mes, sizeof(struct contact), 1, ss);
        fflush(ss);
        fclose(ss);
        strcpy(SendDataStruct.checkcode, "ADS");
        SendDataStruct.save[99] = '\n';
        memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
        len = send(c, sendbuf, sizeof(sendbag), 0);
        if (len == SOCKET_ERROR || len == 0)
        {
            closesocket(c);
            delete_out_user(a);
            return 0;
        }
    }
    break;
    case 68585: //RCO
    {
        UserReqFriendRel(a);
        UserReqIotRel(a);
    }
    break;
    case 69000: //RPA
    {
        UpdateLocalRegUserAndIotlist();
        int encodedCrypto[96]={0};
        memcpy(encodedCrypto,a->data,32*a->key.encryptBlockBytes*sizeof(int));
        decodeMessage(32, a->key.encryptBlockBytes, encodedCrypto,a->REUSERPASSWORD,a->key.privateKey, a->key.commonKey);//RSA Decrypt Password
        Decrypt(a->REUSERPASSWORD,32,a->Pin,a->REUSERPASSWORD);//Kaisa Decrypt
        Decrypt(a->USERPASSWORD,32,a->Pin,a->USERPASSWORD);
        if (UserRePwd(a) == 1)
        {
            memset(&SendDataStruct, 0, sizeof(sendbag));
            memset(sendbuf, 0, sizeof(sendbag));
            strcpy(SendDataStruct.checkcode, "RPA");
            SendDataStruct.save[99] = '\n';
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = send(c, sendbuf, sizeof(SendDataStruct), 0);
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);
                return 0;
            }
        }
        else
        {
            memset(&SendDataStruct, 0, sizeof(sendbag));
            memset(sendbuf, 0, sizeof(sendbag));
            strcpy(SendDataStruct.checkcode, "RPF");//RE PASSWORD FAIL
            SendDataStruct.save[99] = '\n';
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = send(c, sendbuf, sizeof(sendbag), 0);
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);

                return 0;
            }
        }
    }
    break;
    default:
        break;
    }
    return 1;
}

//    if(!strcmp(tag,REGISTER))
//    {
//        if (Register(a,0)==1)
//        {
//            printf("\n注册成功\n");
//            memset(&SendDataStruct,0,sizeof(sendbag));
//            memset(sendbuf,0,sizeof(sendbag));
//            strcpy(SendDataStruct.checkcode,"RE");
//            SendDataStruct.save[99]='\n';
//            memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//            len=send(c,sendbuf,sizeof(SendDataStruct),0);
//            if(len==SOCKET_ERROR||len==0)
//            {
//                printf("\n连接%I64d退出\n",c);
//                closesocket(c);
//                return 0;
//            }
//        }
//        else
//        {
//            printf("\n注册失败\n");
//            memset(&SendDataStruct,0,sizeof(sendbag));
//            memset(sendbuf,0,sizeof(sendbag));
//            strcpy(SendDataStruct.checkcode,"Re");
//            SendDataStruct.save[99]='\n';
//            memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//            len=send(c,sendbuf,sizeof(sendbag),0);
//            if(len==SOCKET_ERROR||len==0)
//            {
//                printf("\n连接%I64d退出\n",c);
//                closesocket(c);
//                return 0;
//            }
//        }
//    }
//    ///****************************登录请求***********************************
//    else if(!strcmp(tag,"SI"))
//    {
//        printf("\n%s/%s/\n",a->USERID,a->USERPASSWORD);
//        if (SIGNIN(a)==1)
//        {
//            printf("\n登陆成功\n");
//            signIN=1;
//            a->info[0]='Y';
//            memset(&SendDataStruct,0,sizeof(sendbag));
//            memset(sendbuf,0,sizeof(sendbag));
//            strcpy(SendDataStruct.checkcode,SIGN_IN);
//            SendDataStruct.save[99]='\n';
//            memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//            len=send(c,sendbuf,sizeof(sendbag),0);
//            if(len==SOCKET_ERROR||len==0)
//            {
//                printf("\n连接%I64d退出\n",c);
//                closesocket(c);
//                delete_out_user(a);
//                return 0;
//            }
//            //
//        }
//        else
//        {
//            printf("\n登录失败\n");
//            memset(&SendDataStruct,0,sizeof(sendbag));
//            memset(sendbuf,0,sizeof(sendbag));
//            strcpy(SendDataStruct.checkcode,"Si");
//            SendDataStruct.save[99]='\n';
//            memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//            len=send(c,sendbuf,sizeof(sendbag),0);
//            if(len==SOCKET_ERROR||len==0)
//            {
//                printf("\n连接%I64d退出\n",c);
//                closesocket(c);
//                return 0;
//            }
//        }
//    }
//    ///****************************************************
//    else if(!strcmp(tag,"RCO"))
//    {
//        UserReqFriendRel(a);
//        UserReqIotRel(a);
//        Sleep(1000);
//    }
//    else if(!strcmp(tag,"UPD"))
//    {
//        memset(&SendDataStruct,0,sizeof(sendbag));
//        memset(sendbuf,0,sizeof(sendbag));
//        strcpy(SendDataStruct.checkcode,"UPD");
//        strcpy(SendDataStruct.DATA,app_version);
//        SendDataStruct.save[99]='\n';
//        memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//        len=send(c,sendbuf,sizeof(sendbag),0);
//    }
//    else if(!strcmp(tag,"RME"))
//    {
//        /**REQUEST MESSAGE**/;
//    }
//    ///****************************************************
//    else if(!strcmp(tag,REPWD))
//    {
//        if (UserRePwd(a)==1)
//        {
//            printf("\n改密成功\n");
//            memset(&SendDataStruct,0,sizeof(sendbag));
//            memset(sendbuf,0,sizeof(sendbag));
//            strcpy(SendDataStruct.checkcode,"RP");
//            SendDataStruct.save[99]='\n';
//            memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//            len=send(c,sendbuf,sizeof(SendDataStruct),0);
//            if(len==SOCKET_ERROR||len==0)
//            {
//                printf("\n连接%I64d退出\n",c);
//                closesocket(c);
//                free(a);
//                return 0;
//            }
//        }
//        else
//        {
//            printf("\n改密失败\n");
//            memset(&SendDataStruct,0,sizeof(sendbag));
//            memset(sendbuf,0,sizeof(sendbag));
//            strcpy(SendDataStruct.checkcode,"Rp");
//            SendDataStruct.save[99]='\n';
//            memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//            len=send(c,sendbuf,sizeof(sendbag),0);
//            if(len==SOCKET_ERROR||len==0)
//            {
//                printf("\n连接%I64d退出\n",c);
//                closesocket(c);
//                free(a);
//                return 0;
//            }
//        }
//    }
//    ///*****************************讨论to******************************
//    else if(!strcmp(tag,"TAI")&&signIN)
//    {
//        if(Check_Id_Pwd(0,a)!=-1)
//        {
//            USER talktouser=FindOnlineUserOrIot(0,SendDataStruct.TalktoID,0);
//            if(talktouser==NULL)
//            {
//                memset(&SendDataStruct,0,sizeof(sendbag));
//                memset(sendbuf,0,sizeof(sendbag));
//                strcpy(SendDataStruct.checkcode,"TNI");//TA but not online
//                SendDataStruct.save[99]='\n';
//                memcpy(sendbuf,&SendDataStruct,sizeof(sendbag));
//                len=send(c,sendbuf,sizeof(sendbag),0);
//                if(len==SOCKET_ERROR||len==0)
//                {
//                    printf("\n连接%I64d退出\n",c);
//                    closesocket(c);
//                    delete_out_user(a);
//                    free(a);
//                    return 0;
//                }
//            }
//            else
//            {
//                //strcpy(talktouser.info,SendDataStruct.TalktoID);
//                ///******如果是对物联设备CD=TA123457///******************************************************
//                strcpy(IoTdata,"CMD");
//                //strncat(IoTdata,a->USERID,11);
//                strncat(IoTdata,SendDataStruct.DATA,26);
//                //printf("%s",SendDataStruct.DATA);
//                /*memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));*/
//                len=send(talktouser->USER_socket,IoTdata,30*sizeof(char),0);
//                // free(sendbuf);
//
//                if(len==SOCKET_ERROR||len==0)
//                {
//                    printf("\n连接%I64d退出\n",c);
//                    closesocket(talktouser->USER_socket);
//                    return 0;
//                }
//                //memset(&SendDataStruct,0,sizeof(sendbag));
//                memset(sendbuf,0,sizeof(sendbag));
//                strcpy(SendDataStruct.checkcode,"TAi");
//                strcpy(SendDataStruct.DATA,talktouser->info);
//                SendDataStruct.save[99]='\n';
//                memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//                /*len=send(c,sendbuf,sizeof(sendbag),0);
//                if(len==SOCKET_ERROR||len==0)
//                {
//                    printf("\n连接%I64d退出\n",c);
//                    closesocket(c);
//                    delete_out_user(a);
//                    return 0;
//                }*/
//            }
//        }
//    }
//    else if(!strcmp(tag,TALK_TO)&&signIN)
//    {
//        if(Check_Id_Pwd(0,a)!=-1)
//        {
//            USER talktouser=FindOnlineUserOrIot(0,SendDataStruct.TalktoID,0);
//            if(talktouser==NULL)
//            {
//                USER find = FindRegisterUserOrIotNode(0,a->TalktoID,0);
//                if(find==NULL)
//                {
//                    return 0;
//                }
//                NewUserMsgStorage(a,find->USERKEY_ID);
//                memset(&SendDataStruct,0,sizeof(sendbag));
//                memset(sendbuf,0,sizeof(sendbag));
//                strcpy(SendDataStruct.checkcode,"TAN");//TA but not online
//                SendDataStruct.save[99]='\n';
//                memcpy(sendbuf,&SendDataStruct,sizeof(sendbag));
//                len=send(c,sendbuf,sizeof(sendbag),0);
//                if(len==SOCKET_ERROR||len==0)
//                {
//                    printf("\n连接%I64d退出\n",c);
//                    closesocket(c);
//                    delete_out_user(a);
//                    free(a);
//                    return 0;
//                }
//            }
//            else
//            {
//                memset(sendbuf,0,sizeof(sendbag));
//                strcpy(SendDataStruct.checkcode,"TAT");
//                SendDataStruct.save[99]='\n';
//                char temp[12]= {0};
//                strcpy(temp,SendDataStruct.USERID);
//                strcpy(SendDataStruct.USERID,SendDataStruct.TalktoID);
//                strcpy(SendDataStruct.TalktoID,temp);
//                memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//                len=send(talktouser->USER_socket,sendbuf,sizeof(sendbag),0);
//                if(len==SOCKET_ERROR||len==0)
//                {
//                    printf("\n连接%I64d退出\n",c);
//                    closesocket(talktouser->USER_socket);
//                    CLN d;
//                    strcpy(d.USERID,a->TalktoID);
//                    delete_out_user(&d);
//                }
//                USER find = FindRegisterUserOrIotNode(0,a->TalktoID,0);
//                if(find==NULL)
//                {
//                    return 0;
//                }
//                NewUserMsgStorage(a,find->USERKEY_ID);
//                memset(&SendDataStruct,0,sizeof(sendbag));
//                memset(sendbuf,0,sizeof(sendbag));
//                strcpy(SendDataStruct.checkcode,"TAS");
//                strcpy(SendDataStruct.TalktoID,a->TalktoID);
//                strcpy(SendDataStruct.USERID,a->USERID);
//                SendDataStruct.save[99]='\n';
//                memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//                len=send(c,sendbuf,sizeof(sendbag),0);
//                if(len==SOCKET_ERROR||len==0)
//                {
//                    printf("\n连接%I64d退出\n",c);
//                    closesocket(c);
//                    delete_out_user(a);
//                    free(a);
//                    return 0;
//                }
//            }
//        }
//        else
//        {
//            printf("用户%s不存在",SendDataStruct.TalktoID);
//            memset(&SendDataStruct,0,sizeof(sendbag));
//            memset(sendbuf,0,sizeof(sendbag));
//            strcpy(SendDataStruct.checkcode,"Taa");//TA but not online
//            SendDataStruct.save[99]='\n';
//            memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//            len=send(c,sendbuf,sizeof(sendbag),0);
//            if(len==SOCKET_ERROR||len==0)
//            {
//                printf("\n连接%I64d退出\n",c);
//                closesocket(c);
//                delete_out_user(a);
//                free(a);
//                return 0;
//            }
//        }
//    }
//
//
//
//
//
//
//
//    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
//    else if(!strcmp(tag,"ADD")&&signIN)
//    {
//        CLN d;
//        char IoTdevice[5]="";
//        int F=0;
//        strcpy(d.USERID,SendDataStruct.TalktoID);
//        if(Check_Id_Pwd(1,&d)!=-1)
//        {
//            strncpy(IoTdevice,SendDataStruct.checkcode,4);
//            IoTdevice[4]='\0';
//            if(!strcmp(IoTdevice,"ADDI"))
//            {
//                F=1;
//            }
//            USER talktouser=FindOnlineUserOrIot(0,SendDataStruct.TalktoID,0);
//            if(talktouser==NULL&&F!=1)
//            {
//                Message mes=(Message)malloc(sizeof(struct message));
//                memset(mes,0,sizeof(contact));
//                memset(mes,0,sizeof(contact));
//                strcpy(mes->checkcode,"NCA");
//                strcpy(mes->TalktoID,a->USERID);
//                strcpy(mes->USERID,SendDataStruct.TalktoID);
//                strcpy(mes->data,SendDataStruct.DATA);
//                printf("用户%s不在线ADD",SendDataStruct.TalktoID);
//                char talkto[30]= {'0'};
//                strcpy(talkto,SendDataStruct.TalktoID);
//                strcat(talkto,"/");
//                strcat(talkto,SendDataStruct.TalktoID);
//                strcat(talkto,"ME");
//                memset(&SendDataStruct,0,sizeof(sendbag));
//                memset(sendbuf,0,sizeof(sendbag));
//                strcpy(SendDataStruct.checkcode,"ADN");
//                //TA but not online
//                strcpy(SendDataStruct.TalktoID,a->TalktoID);
//                strcpy(SendDataStruct.USERID,a->USERID);
//                SendDataStruct.save[99]='\n';
//                memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//                FILE* ss=fopen(talkto,"a+");///**********dakai
//                fwrite(mes,sizeof(struct message),1,ss);
//                fflush(ss);
//                fclose(ss);
//                free(mes);
//                len=send(c,sendbuf,sizeof(sendbag),0);
//                if(len==SOCKET_ERROR||len==0)
//                {
//                    printf("\n连接%I64d退出\n",c);
//                    closesocket(c);
//                    delete_out_user(a);
//                    return 0;
//                }
//            }
//            else if(talktouser!=NULL&&F!=1)
//            {
//
//                //memset(&SendDataStruct,0,sizeof(sendbag));
//                memset(sendbuf,0,sizeof(sendbag));
//                strcpy(SendDataStruct.checkcode,"ADT");
//                SendDataStruct.save[99]='\n';
//                char temp[12]= {0};
//                strcpy(temp,SendDataStruct.USERID);
//                strcpy(SendDataStruct.USERID,SendDataStruct.TalktoID);
//                strcpy(SendDataStruct.TalktoID,temp);
//                SendDataStruct.save[99]='\n';
//                memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//                len=send(talktouser->USER_socket,sendbuf,sizeof(sendbag),0);
//                if(len==SOCKET_ERROR||len==0)
//                {
//                    printf("\n连接%I64d退出\n",c);
//                    closesocket(talktouser->USER_socket);
//                    delete_out_user(&d);
//                    isRedo=1;
//                    strcpy(temp,SendDataStruct.USERID);
//                    strcpy(SendDataStruct.USERID,SendDataStruct.TalktoID);
//                    strcpy(SendDataStruct.TalktoID,temp);
//                }
//                memset(&SendDataStruct,0,sizeof(sendbag));
//                memset(sendbuf,0,sizeof(sendbag));
//                strcpy(SendDataStruct.TalktoID,a->TalktoID);
//                strcpy(SendDataStruct.USERID,a->USERID);
//                strcpy(SendDataStruct.checkcode,"ADD");
//                SendDataStruct.save[99]='\n';
//                memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//                len=send(c,sendbuf,sizeof(sendbag),0);
//                if(len==SOCKET_ERROR||len==0)
//                {
//                    printf("\n连接%I64d退出\n",c);
//                    closesocket(c);
//                    delete_out_user(a);
//                    return 0;
//                }
//                ///********************
//
//            }
//            else if(F==1&&talktouser!=NULL)
//            {
//                char IoTcommand[30]="";
//                strcpy(IoTcommand,"ADD");
//                strncat(IoTcommand,a->USERID,20);
//                len=send(talktouser->USER_socket,IoTcommand,30*sizeof(char),0);
//                // free(sendbuf);
//                if(len==SOCKET_ERROR||len==0)
//                {
//                    printf("\n连接%I64d退出\n",c);
//                    closesocket(talktouser->USER_socket);
//                    return 0;
//                }
//            }
//        }
//        else
//        {
//            printf("用户%s不存在ADD",SendDataStruct.TalktoID);
//            //memset(&SendDataStruct,0,sizeof(sendbag));
//            memset(sendbuf,0,sizeof(sendbag));
//            strcpy(SendDataStruct.checkcode,"Add");//TA but not online
//            SendDataStruct.save[99]='\n';
//            memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//            len=send(c,sendbuf,sizeof(sendbag),0);
//            // free(sendbuf);
//            if(len==SOCKET_ERROR||len==0)
//            {
//                printf("\n连接%I64d退出\n",c);
//                closesocket(c);
//                delete_out_user(a);
//                return 0;
//            }
//        }
//    }
//    ///*******************************************
//    else if(!strcmp(tag,"ADS"))///****only reply****
//    {
//
//        Contact mes=(Contact)malloc(sizeof(struct contact));
//        memset(mes,0,sizeof(contact));
//        memset(sendbuf,0,sizeof(sendbag));
//        strcpy(mes->checkcode,"RCO");
//        strcpy(mes->TalktoID,SendDataStruct.TalktoID);
//        strcpy(mes->USERID,SendDataStruct.USERID);
//        SendDataStruct.save[99]='\n';
//        memset(mes,0,sizeof(contact));
//        strcpy(mes->checkcode,"RCO");
//        strcpy(mes->TalktoID,SendDataStruct.USERID);
//        strcpy(mes->USERID,SendDataStruct.TalktoID);
//        ///**********dakai
//
//        char talkto[30]= {'0'};
//        strcpy(talkto,SendDataStruct.TalktoID);
//        strcat(talkto,"/");
//        strcat(talkto,SendDataStruct.TalktoID);
//        strcat(talkto,"CO");
//        FILE* ss=fopen(talkto,"a+");///**********dakai
//        fwrite(mes,sizeof(struct contact),1,ss);
//        fflush(ss);
//        fclose(ss);
//        strcpy(SendDataStruct.checkcode,"ADS");
//        SendDataStruct.save[99]='\n';
//        memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//        len=send(c,sendbuf,sizeof(sendbag),0);
//        if(len==SOCKET_ERROR||len==0)
//        {
//            printf("\n连接%I64d退出\n",c);
//            closesocket(c);
//            delete_out_user(a);
//            return 0;
//        }
//    }
//    ///*********************************************
//    else if(!strcmp(tag,"HB"))
//    {
//        memset(&SendDataStruct,0,sizeof(sendbag));
//        memset(sendbuf,0,sizeof(sendbag));
//        strcpy(SendDataStruct.checkcode,"HB");
//        SendDataStruct.save[99]='\n';
//        memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//        len=send(c,sendbuf,sizeof(sendbag),0);
//        // free(sendbuf);
//        if(len==SOCKET_ERROR||len==0)
//        {
//            printf("\n连接%I64d退出\n",c);
//            closesocket(c);
//            return 0;
//        }
//    }
//    else if(!strcmp(tag,"STO"))
//    {
//        memset(&SendDataStruct,0,sizeof(sendbag));
//        memset(sendbuf,0,sizeof(sendbag));
//        strcpy(SendDataStruct.checkcode,"STO");
//        SendDataStruct.save[99]='\n';
//        memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
//        len=send(a->remote_socket,sendbuf,sizeof(sendbag),0);
//        if(len==SOCKET_ERROR||len==0)
//        {
//            printf("\n连接%I64d退出\n",c);
//            closesocket(c);
//            delete_out_user(a);
//            return 0;
//        }
//        delete_out_user(a);
//        printf("\n%s\n",SendDataStruct.checkcode);
//        a->info[0]='N';
//        creat_check_alive=0;
//    }
/////***************************************IOT**************************************
//
////    else
////    {
////        //printf("%s",SendDataStruct.USERID);
////        closesocket(c);
////        printf("\nIllegal user\n ");
////        return 0;
////    }
//
////    else
////    {
////        closesocket(c);
////        printf("\nIllegal user\n ");
////        return 0;
////    }
//    return 0;
//}
//
///*
//else if(!strcmp("ZYXX1227",SendDataStruct.checkcode))
//{
//    ///************************************
//    printf("\nNOMAL IoT_CLIENT:|%s\n",inet_ntoa(a->ADDR.sin_addr));///改
//    //len=recv(c,rec,1*sizeof(char),0);
//    //send(c,"123456789012345678901234567890",30*sizeof(char),0);
//    recv(c,rec,12*sizeof(char),0);
//    strncpy(a->USERID,rec,12);
//    printf("%s",rec);
//    a->USERID[11] ='\0';
//    printf("--%s",a->USERID);
//    recv(c,rec,10*sizeof(char),0);
//    strncpy(a->USERPASSWORD,rec,10);
//    a->USERPASSWORD[9]='\0';
//    printf("/%s\n",a->USERPASSWORD);
//    printf("\nUnecessary Regist\n");
//    ///*************************************
//    Register(a,1);
//    if (SIGNIN(a)==1)
//    {
//        printf("\n登陆成功\n");
//        signIN=1;
//        strcpy(IoTdata,"SII");
//        strcat(IoTdata,a->USERID);
//        len=send(c,IoTdata,30*sizeof(char),0);
//        if(len==SOCKET_ERROR||len==0)
//        {
//            printf("\n连接%I64d退出\n",c);
//            closesocket(c);
//            delete_out_user(a);
//            return 0;
//        }
//        strcpy(Contacta,a->USERID);
//        strcat(Contacta,"CO");
//        //usercontact=fopen(Contact,"a+");
//        strcpy(Messagea,a->USERID);
//        strcat(Messagea,"ME");
//        ///*************进入
//        IoTtalk(Contacta,Messagea,a);
//        //
//    }
//    else
//    {
//        printf("\n登录失败\n");
//        memset(&SendDataStruct,0,sizeof(sendbag));
//        memset(sendbuf,0,sizeof(sendbag));
//        strcpy(IoTdata,"Sii");
//        strcat(IoTdata,a->USERID);
//        len=send(c,IoTdata,30*sizeof(char),0);
//        if(len==SOCKET_ERROR||len==0)
//        {
//            printf("\n连接%I64d退出\n",c);
//            closesocket(c);
//            return 0;
//        }
//        */

