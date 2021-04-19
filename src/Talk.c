#include"../head/SERVER.h"
#include "../libmd5utils/md5.h"

int SyncSend(SOCKET socket,char *buf,int buflen,pthread_mutex_t *t)
{
    if(t==NULL)
        return -1;
    int len = 0;
    pthread_mutex_lock(t);
    len = send(socket,buf,buflen,0);
    pthread_mutex_unlock(t);
    return len;
}
#ifdef STPOOL
int talk(struct sttask *ptask)
#else
int talk(LPVOID b)
#endif
{
    int len,signIN=0;
#ifdef STPOOL
    if(ptask->task_arg==NULL)
    {
        log_error("ARG NULL ERR");
        return 0;
    }
    CLN* a=(CLN*)ptask->task_arg;
#else
    CLN* a = (CLN*)b;
#endif
    log_debug("%s",a->checkcode);
    if(strlen(a->checkcode)<3)//IotDev
    {
        IoTtalk(a);
        InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
        freeNode(a->MemMark,a);
#else
        MemoryPoolFree(mp, a);
#endif
        return 0;
    }
    /****************************/
    UserPacketInterface SendDataStruct;
    char sendbuf[sizeof(UserPacketInterface)]= {0};
    memset(&SendDataStruct,0,sizeof(UserPacketInterface));
    /********************************/
    char tag[4];
    memset(tag,0,4);
    memcpy(tag,a->checkcode,3);
    /*******************************/
    SOCKET c=a->SOCKET;
    signIN = (a->info[0]=='Y');//whether User had Signed in
    ///************************循环接受用户请求******************************
    switch (DJBHash(tag, 3))
    {
    case 57648: //HBA
    {
        memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
        memset(sendbuf, 0, sizeof(UserPacketInterface));
        strcpy(SendDataStruct.checkcode, "HBA");
        SendDataStruct.save[99] = _HC_;
        memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
        len = SyncSend(c, sendbuf, sizeof(UserPacketInterface), &(a->conn->t));
        if (len == SOCKET_ERROR || len == 0)
        {
            closesocket(c);

            InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
            freeNode(a->MemMark,a);
#else
            MemoryPoolFree(mp, a);
#endif
            return 0;
        }
    }
    case 69099://RSA
    {
        InitRSA(&(a->key));//Create RSAKey, need srand(time(NULL)) first
        InitRSA(&(a->key));//twice ,it is a bug left to be fixed
        a->conn->key = a->key;
        log_debug("Public Key (%d,%d) | Private Key (%d,%d)",a->key.publicKey,a->key.commonKey,a->key.privateKey,a->key.commonKey);
        memset(&SendDataStruct,0,sizeof(UserPacketInterface));
        memset(sendbuf,0,sizeof(UserPacketInterface));
        strcpy(SendDataStruct.checkcode,"RSA");
        sprintf(SendDataStruct.DATA,"%d_%d_%d_",a->key.publicKey,a->key.commonKey,a->key.encryptBlockBytes);
        SendDataStruct.save[99]=_HC_;
        memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
        len=SyncSend(a->SOCKET,sendbuf,sizeof(UserPacketInterface),&(a->conn->t));
        if(len==SOCKET_ERROR||len==0)
        {
            log_debug("连接%I64d退出",c);
            closesocket(c);
            InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
            freeNode(a->MemMark,a);
#else
            MemoryPoolFree(mp, a);
#endif
            return 0;
        }
    }
    break;
    case 66604://PIN
    {
        if(a->key.encryptBlockBytes==0)
            return 0;
        generateRandIntStr(a->Pin,6);
        memcpy(a->conn->Pin,a->Pin,7);
        memset(&SendDataStruct,0,sizeof(UserPacketInterface));
        memset(sendbuf,0,sizeof(UserPacketInterface));
        strcpy(SendDataStruct.checkcode,"PIN");
        ///Send Email to inform User
        if(signIN==1)
        {
            int encodedCrypto[100]= {0};
            encodeMessage(6, a->key.encryptBlockBytes, a->Pin,encodedCrypto,a->key.privateKey, a->key.commonKey);
            char encodedCryptoByte[100]="";
            memcpy(encodedCryptoByte,encodedCrypto,sizeof(INT32)*6*a->key.encryptBlockBytes);
            a->Pin[6]=0;
            char MD5Temp[33]="";
            Compute_string_md5((unsigned char *)MD5Temp,32,a->Pin);
            sprintf(SendDataStruct.DATA,"%s_%s_",encodedCryptoByte,MD5Temp);
        }
        else
        {
            char cmd[100]="";
            char email[50]="";
            memcpy(email,a->data,50);
            sprintf(cmd,"%s %s %s","cmd.exe /c \"PINSend.bat\"",email,a->Pin);
            log_debug("%s",cmd);
            system(cmd);
        }
        SendDataStruct.save[99]=_HC_;
        memcpy(sendbuf,&SendDataStruct,sizeof(SendDataStruct));
        len=SyncSend(a->SOCKET,sendbuf,sizeof(UserPacketInterface),&(a->conn->t));
        if(len==SOCKET_ERROR||len==0)
        {
            log_debug("连接%I64d退出",c);
            closesocket(c);

            InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
            freeNode(a->MemMark,a);
#else
            MemoryPoolFree(mp, a);
#endif
            return 0;
        }
    }
    break;
    case 69859://SIB_RECONNECT
    case 69858: //SIA
    {
        int encodedCrypto[100]= {0};
        memcpy(encodedCrypto,a->data,sizeof(int)*32*a->key.encryptBlockBytes);
        decodeMessage(32, a->key.encryptBlockBytes, encodedCrypto,a->USERPASSWORD,a->key.privateKey, a->key.commonKey);
        a->USERPASSWORD[32]=0;
        if (SIGNIN(a) == 1)
        {
            memcpy(a->conn->USERID,a->USERID,12);
            if(FindRegisterUserOrIotNode(0,a->USERID,0)!=NULL)
            {
                a->USERKEY_ID = FindRegisterUserOrIotNode(0,a->USERID,0)->USERKEY_ID;
                a->conn->USERKEY_ID = a->USERKEY_ID;
            }
            signIN = 1;
            a->info[0] = 'Y';
            a->conn->info[0] = 'Y';
            memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
            memset(sendbuf, 0, sizeof(UserPacketInterface));
            strcpy(SendDataStruct.checkcode, "SIA");
            SendDataStruct.save[99] = _HC_;
            memcpy(sendbuf, &SendDataStruct, sizeof(UserPacketInterface));
            len = SyncSend(c, sendbuf, sizeof(UserPacketInterface), &(a->conn->t));
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);
                InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
                freeNode(a->MemMark,a);
#else
                MemoryPoolFree(mp, a);
#endif
                return 0;
            }
        }
        else
        {
            memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
            memset(sendbuf, 0, sizeof(UserPacketInterface));
            strcpy(SendDataStruct.checkcode, "SiA");
            SendDataStruct.save[99] = _HC_;
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = SyncSend(c, sendbuf, sizeof(UserPacketInterface), &(a->conn->t));
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);
                InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
                freeNode(a->MemMark,a);
#else
                MemoryPoolFree(mp, a);
#endif
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
        if(strcmp(PinMd5,a->USERPASSWORD))
        {
            memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
            memset(sendbuf, 0, sizeof(UserPacketInterface));
            strcpy(SendDataStruct.checkcode, "RPE");//Reg PinCode Err
            SendDataStruct.save[99] = _HC_;
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = SyncSend(c, sendbuf, sizeof(SendDataStruct),&(a->conn->t));
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);

                InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
                freeNode(a->MemMark,a);
#else
                MemoryPoolFree(mp, a);
#endif
                return 0;
            }
        }
        int encodedCrypto[96]= {0};
        memcpy(encodedCrypto,a->data,32*a->key.encryptBlockBytes*sizeof(int));
        decodeMessage(32, a->key.encryptBlockBytes, encodedCrypto,a->USERPASSWORD,a->key.privateKey, a->key.commonKey);//RSA Decrypt Password
        a->USERPASSWORD[32] = 0;
        Decrypt(a->USERPASSWORD,32,a->Pin,a->USERPASSWORD);//Kaisa Decrypt
        if (Register(a, 0) == 1)
        {
            memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
            memset(sendbuf, 0, sizeof(UserPacketInterface));
            strcpy(SendDataStruct.checkcode, "REA");
            SendDataStruct.save[99] = _HC_;
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = SyncSend(c, sendbuf, sizeof(SendDataStruct), &(a->conn->t));
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);

                InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
                freeNode(a->MemMark,a);
#else
                MemoryPoolFree(mp, a);
#endif
                return 0;
            }
        }
        else
        {
            memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
            memset(sendbuf, 0, sizeof(UserPacketInterface));
            strcpy(SendDataStruct.checkcode, "ReA");
            SendDataStruct.save[99] = _HC_;
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = SyncSend(c, sendbuf, sizeof(UserPacketInterface), &(a->conn->t));
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);

                InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
                freeNode(a->MemMark,a);
#else
                MemoryPoolFree(mp, a);
#endif
                return 0;
            }
        }
    }
    break;
    case 68905: //RME
    {
        //UserGetIotData(a);
        //UserRequestMessage(a,)
    }
    break;
    case 69664://SCE
    {
        int OutStrSize = 0;
        char** outStr = StrSplit(a->data,200,&OutStrSize,'-');
        if(OutStrSize<7)
        {
            log_debug("SCE ERR");
            releaseStr(outStr,OutStrSize);
            return 0;
        }
        char Cmd[200];
        memset(Cmd,0,200);
        sprintf(Cmd,"%s-%s-%s-%d-98-%s+%s+%s+-",outStr[0],outStr[1],outStr[2],FindRegisterUserOrIotNode(10,a->TalktoID,0)->USERKEY_ID,outStr[0],outStr[1],outStr[2]);
        CLN tmp;
        tmp.USERKEY_ID = a->USERKEY_ID;
        sprintf(tmp.TalktoID,"%s",FindRegisterUserOrIotNode(10,NULL,atoi(outStr[0]))->USERID);
        NewUserSceneCmdStore(&tmp,Cmd,0,atoi(outStr[1]),0,"00:00:00","1111111");
        NewUserSceneCmdStore(a,a->data,0,99,0,"00:00:00","1111111");
        releaseStr(outStr,OutStrSize);
    }
    break;
    case 59185://IOT
    {
        UserGetIotData(a);
    }
    break;
    case 70683: //TAA
    {
        USER find = FindRegisterUserOrIotNode(0, a->TalktoID, 0);
        if (find == NULL)
        {

            InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
            freeNode(a->MemMark,a);
#else
            MemoryPoolFree(mp, a);
#endif
            return 0;
        }
        NewUserMsgStorage(a, find->USERKEY_ID);
        memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
        memset(sendbuf, 0, sizeof(UserPacketInterface));
        strcpy(SendDataStruct.checkcode, "TAN"); //TA but not online
        SendDataStruct.save[99] = _HC_;
        memcpy(sendbuf, &SendDataStruct, sizeof(UserPacketInterface));
        len = SyncSend(c, sendbuf, sizeof(UserPacketInterface), &(a->conn->t));
        if (len == SOCKET_ERROR || len == 0)
        {
            closesocket(c);

            InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
            freeNode(a->MemMark,a);
#else
            MemoryPoolFree(mp, a);
#endif
            return 0;
        }
    }
    break;
    case 70235: //STO
    {
        memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
        memset(sendbuf, 0, sizeof(UserPacketInterface));
        strcpy(SendDataStruct.checkcode, "STO");
        SendDataStruct.save[99] = _HC_;
        memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
        len = SyncSend(a->SOCKET, sendbuf, sizeof(UserPacketInterface), &(a->conn->t));
        if (len == SOCKET_ERROR || len == 0)
        {
            closesocket(c);

            InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
            freeNode(a->MemMark,a);
#else
            MemoryPoolFree(mp, a);
#endif
            return 0;
        }
        delete_out_user(a);
        a->info[0] = 'N';
    }
    break;
    case 72270: //UPD
    {
        memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
        memset(sendbuf, 0, sizeof(UserPacketInterface));
        strcpy(SendDataStruct.checkcode, "UPD");
        strcpy(SendDataStruct.DATA, app_version);
        SendDataStruct.save[99] = _HC_;
        memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
        len = SyncSend(c, sendbuf, sizeof(UserPacketInterface), &(a->conn->t));
    }
    break;
    case 50094: //ADD
    {
        UpdateLocalRegUserAndIotlist();
        USER find = FindRegisterUserOrIotNode(0, a->TalktoID, 0);
        if (find == NULL)
        {
            InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
            freeNode(a->MemMark,a);
#else
            MemoryPoolFree(mp, a);
#endif
            return 0;
        }
        NewUserMsgStorage(a, find->USERKEY_ID);
        memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
        memset(sendbuf, 0, sizeof(UserPacketInterface));
        strcpy(SendDataStruct.checkcode, "ADN"); //ADD
        SendDataStruct.save[99] = _HC_;
        memcpy(sendbuf, &SendDataStruct, sizeof(UserPacketInterface));
        len = SyncSend(c, sendbuf, sizeof(UserPacketInterface), &(a->conn->t));
        if (len == SOCKET_ERROR || len == 0)
        {
            closesocket(c);
            InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
            freeNode(a->MemMark,a);
#else
            MemoryPoolFree(mp, a);
#endif
            return 0;
        }
    }
    break;
    case 50109://ADS
    {
        memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
        memset(sendbuf, 0, sizeof(UserPacketInterface));
        CLN temp;
        strcpy(temp.USERID,a->TalktoID);
        if(!NewUserFriend(a,FindRegisterUserOrIotNode(0,a->TalktoID,0)->USERKEY_ID)||!NewUserFriend(&temp,FindRegisterUserOrIotNode(0,a->USERID,0)->USERKEY_ID))
        {
            strcpy(SendDataStruct.checkcode, "ADS"); //ADD
        }
        else
        {
            strcpy(SendDataStruct.checkcode, "ADN"); //ADD
        }
        SendDataStruct.save[99] = _HC_;
        memcpy(sendbuf, &SendDataStruct, sizeof(UserPacketInterface));
        len = SyncSend(c, sendbuf, sizeof(UserPacketInterface), &(a->conn->t));
        if (len == SOCKET_ERROR || len == 0)
        {
            closesocket(c);

            InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
            freeNode(a->MemMark,a);
#else
            MemoryPoolFree(mp, a);
#endif
            return 0;
        }
    }
    break;
    case 58782://ICM
    {
        int OutStrSize = 0;
        char** outStr = StrSplit(a->data,strlen(a->data),&OutStrSize,'_');
        if(OutStrSize<2)
        {
            releaseStr(outStr,OutStrSize);
            return 0;
        }
        USER temp = FindOnlineUserOrIot(10,a->TalktoID,0);
        if(temp==NULL)
        {
            temp = FindRegisterUserOrIotNode(10,a->TalktoID,0);
            if(temp==NULL)
            {
                return 0;
            }
            CLN tmp;
            sprintf(tmp.checkcode,"%s","00");
            sprintf(tmp.data,"%s_%s_",outStr[0],outStr[1]);
            tmp.USERKEY_ID = temp->USERKEY_ID;
            Send2OnlineUserViaTopServer(tmp);
        }
        else
        {
            IotPacketInterface SendStruct;
            strcpy(SendStruct.opCode,"00");
            sprintf(SendStruct.payLoad,"%s_%s_",outStr[0],outStr[1]);
            Encrypt(SendStruct.payLoad,strlen(SendStruct.payLoad),temp->CONNHANDLE->Pin,SendStruct.payLoad);
            InterlockedIncrement((LPLONG) &(temp->CONNHANDLE->SeqNum));
            SendStruct.SeqNum[0] = temp->CONNHANDLE->SeqNum;
            if(temp->CONNHANDLE->SeqNum==127)
            {
                temp->CONNHANDLE->SeqNum = 1;
            }
            char SendBuf[sizeof(IotPacketInterface)];
            memcpy(SendBuf,&SendStruct,sizeof(IotPacketInterface));
            SendBuf[sizeof(IotPacketInterface)-1] = _HC_;
            len=SyncSend(temp->USER_SOCKET,SendBuf,sizeof(IotPacketInterface),&(temp->CONNHANDLE->t));
            if(len==SOCKET_ERROR||len==0)
            {
                closesocket(temp->USER_SOCKET);
            }
        }
        releaseStr(outStr,OutStrSize);
    }
    break;
    case 59168://IOC
    {
        CLN temp;
        temp.USERKEY_ID=a->USERKEY_ID;
        sprintf(temp.TalktoID,"%s",a->TalktoID);
        memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
        memset(sendbuf, 0, sizeof(UserPacketInterface));
        if(NewUserIot(&temp,0))
        {
            strcpy(SendDataStruct.checkcode, "IOC");//

        }
        else
        {
            strcpy(SendDataStruct.checkcode, "ioc");//
        }
        SendDataStruct.save[99] = _HC_;
        memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
        len = SyncSend(c, sendbuf, sizeof(UserPacketInterface), &(a->conn->t));
        if (len == SOCKET_ERROR || len == 0)
        {
            closesocket(c);
            InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
            freeNode(a->MemMark,a);
#else
            MemoryPoolFree(mp, a);
#endif
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
        int encodedCrypto[96]= {0};
        memcpy(encodedCrypto,a->data,32*a->key.encryptBlockBytes*sizeof(int));
        decodeMessage(32, a->key.encryptBlockBytes, encodedCrypto,a->REUSERPASSWORD,a->key.privateKey, a->key.commonKey);//RSA Decrypt Password
        Decrypt(a->REUSERPASSWORD,32,a->Pin,a->REUSERPASSWORD);//Kaisa Decrypt
        Decrypt(a->USERPASSWORD,32,a->Pin,a->USERPASSWORD);
        if (UserRePwd(a) == 1)
        {
            memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
            memset(sendbuf, 0, sizeof(UserPacketInterface));
            strcpy(SendDataStruct.checkcode, "RPA");
            SendDataStruct.save[99] = _HC_;
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = SyncSend(c, sendbuf, sizeof(SendDataStruct), &(a->conn->t));
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);

                InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
                freeNode(a->MemMark,a);
#else
                MemoryPoolFree(mp, a);
#endif
                return 0;
            }
        }
        else
        {
            memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
            memset(sendbuf, 0, sizeof(UserPacketInterface));
            strcpy(SendDataStruct.checkcode, "RPF");//RE PASSWORD FAIL
            SendDataStruct.save[99] = _HC_;
            memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
            len = SyncSend(c, sendbuf, sizeof(UserPacketInterface), &(a->conn->t));
            if (len == SOCKET_ERROR || len == 0)
            {
                closesocket(c);

                InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
                freeNode(a->MemMark,a);
#else
                MemoryPoolFree(mp, a);
#endif
                return 0;
            }
        }
    }
    break;
    default:
    {
        memset(&SendDataStruct, 0, sizeof(UserPacketInterface));
        memset(sendbuf, 0, sizeof(UserPacketInterface));
        strcpy(SendDataStruct.checkcode, "ERR");//OPCODE ERR
        SendDataStruct.save[99] = _HC_;
        memcpy(sendbuf, &SendDataStruct, sizeof(SendDataStruct));
        len = SyncSend(c, sendbuf, sizeof(UserPacketInterface), &(a->conn->t));
        if (len == SOCKET_ERROR || len == 0)
        {
            closesocket(c);
            InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
            freeNode(a->MemMark,a);
#else
            MemoryPoolFree(mp, a);
#endif
            return 0;
        }
        break;
    }
    }

    InterlockedDecrement((LPLONG) &(a->conn->info[2]));
#ifdef MemPool
    freeNode(a->MemMark,a);
#else
    MemoryPoolFree(mp, a);
#endif
    return 0;
}
