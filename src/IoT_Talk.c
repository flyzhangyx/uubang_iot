#include"../head/SERVER.h"
#include"../libmd5utils/md5.h"
#define SEND_OP_BACK(str) do{strcpy(SendStruct.opCode,str);\
                InterlockedIncrement((LPLONG) &(a->conn->SeqNum));\
                SendStruct.SeqNum[0] = a->conn->SeqNum;\
                if(a->conn->SeqNum==127)\
                {\
                    a->conn->SeqNum = 1;\
                }\
                memcpy(SendBuf,&SendStruct,sizeof(IotPacketInterface));\
                SendBuf[sizeof(IotPacketInterface)-1] = _HC_;\
                len=SyncSend(a->SOCKET,SendBuf,sizeof(IotPacketInterface),&(a->conn->t));\
                if(len==SOCKET_ERROR||len==0)\
                {\
                    closesocket(a->SOCKET);\
                }}while(0)

int IoTtalk(CLN* a)
{
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
        sprintf(SendStruct.payLoad,"%d_%d_%d_",a->key.publicKey,a->key.commonKey,a->key.encryptBlockBytes);
        SEND_OP_BACK("01");
    }
    break;
    case 2://PIN
    {
        if(a->key.encryptBlockBytes==0)
            return 0;
        generateRandIntStr(a->Pin,6);
        a->Pin[6]=0;
        memcpy(a->conn->Pin,a->Pin,6);
        log_debug("%s",a->Pin);
        memset(&SendStruct,0,sizeof(IotPacketInterface));
        memset(SendBuf,0,sizeof(IotPacketInterface));
        int encodedCrypto[30]= {0};
        encodeMessage(6, a->key.encryptBlockBytes,a->Pin,encodedCrypto,a->key.privateKey, a->key.commonKey);
        char encodedCryptoByte[100];
        memcpy(encodedCryptoByte,encodedCrypto,6*a->key.encryptBlockBytes*sizeof(int));
        //char MD5Temp[33]="NULL";
        //Compute_string_md5((unsigned char *)MD5Temp,32,a->Pin);
        //sprintf(SendStruct.payLoad,"%s_%s_",encodedCryptoByte,MD5Temp);
        memcpy(SendStruct.payLoad,encodedCryptoByte,sizeof(int)*a->key.encryptBlockBytes*6);
        SendStruct.payLoad[6*a->key.encryptBlockBytes*sizeof(int)] = '_';
        SEND_OP_BACK("02");
    }
    break;
    case 3://REG_SIGN
    {
        if(a->conn->info[0]!='Y'&&a->key.encryptBlockBytes!=0)
        {
            int OutStrSize = 0;
            char** outStr = StrSplit(a->data,200,&OutStrSize,'_');
            if(OutStrSize<2)
            {
                releaseStr(outStr,OutStrSize);
                return 0;
            }
            int encodedCrypto[100]= {0};
            memcpy(encodedCrypto,outStr[1],sizeof(int)*7*a->key.encryptBlockBytes);
            decodeMessage(7, a->key.encryptBlockBytes, encodedCrypto,a->USERPASSWORD,a->key.privateKey, a->key.commonKey);
            sprintf(a->USERID,"%s", outStr[0]);
            Register(a,1);
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
                SEND_OP_BACK("03");
            }
            else
            {
                SEND_OP_BACK("23");
            }
            releaseStr(outStr,OutStrSize);
        }
        else
        {
            SEND_OP_BACK("23");
        }
    }
    break;
    case 4://UPDATE DATA
    {
        if(a->info[0] != 'Y')
            return 0;
        ///Multi data format to resolve
        Decrypt(a->data,strlen(a->data),a->Pin,a->data);
        int OutStrSize = 0;
        char** outStr = StrSplit(a->data,200,&OutStrSize,'_');
        int i = 0;
        if(OutStrSize%2!=0)
            return 0;
        while(i<OutStrSize)
        {
            switch (atoi(outStr[i]))
            {
            case 0://Open_close
            {
                sprintf(a->data,"%s","status");
                if(IotUpdateStatus(a,0,atoi(outStr[i+1])))
                {
                    SEND_OP_BACK("04");
                }
                else
                {
                    SEND_OP_BACK("24");
                }
            }
            break;
            case 1://temperature
            {
                sprintf(a->data,"%s",outStr[i+1]);
                if(IotUpdateStatus(a,1,0))
                {
                    SEND_OP_BACK("04");
                }
                else
                {
                    SEND_OP_BACK("24");
                }
            }
            break;
            case 2://humidity
            {


            }
            break;
            case 3://brightness
            {
                sprintf(a->data,"%s",outStr[i+1]);
                if(IotUpdateStatus(a,3,0))
                {
                    SEND_OP_BACK("04");
                }
                else
                {
                    SEND_OP_BACK("24");
                }
            }
            break;
            default:
                break;
            }
            i+=2;
        }
        releaseStr(outStr,OutStrSize);
    }
    break;
    case 5://READ CMD
    {
        if(a->info[0] != 'Y')
            return 0;
        Decrypt(a->data,strlen(a->data),a->Pin,a->data);
        int OutStrSize = 0;
        char** outStr = StrSplit(a->data,200,&OutStrSize,'_');
        if(OutStrSize<1)
            break;
        if(!IotReadCmd(a,atoi(outStr[0]),0))
        {
            SEND_OP_BACK("25");
            break;
        }
        IotReadCmd(a,atoi(outStr[0]),1);//After Read  Deletion
        memset(&SendStruct,0,sizeof(IotPacketInterface));
        Encrypt(a->data,strlen(a->data),a->Pin,SendStruct.payLoad);
        SEND_OP_BACK("05");
    }
    break;
    case 6://READ SCENE CMD
    {
        if(a->info[0]!='Y')
            return 0;
        IotReadSelfSceneCmd(a,a->USERKEY_ID);
    }
    break;
    case 7://READ IOT DATA
    {
        if(a->info[0] != 'Y')
            return 0;
        int OutStrSize = 0;
        char** outStr = StrSplit(a->data,200,&OutStrSize,'_');
        int i = 0;
        while(i++<OutStrSize)
        {
            IotGetIotData(a,atoi(outStr[i-1]));
        }
        releaseStr(outStr,OutStrSize);
    }
    break;
    case 8://SEND CMD TO OTHER DEV
    {
        if(a->info[0] != 'Y')
            return 0;
        Decrypt(a->data,strlen(a->data),a->Pin,a->data);
        int OutStrSize = 0;
        char** outStr = StrSplit(a->data,200,&OutStrSize,'_');
        if(OutStrSize<3)
        {
            releaseStr(outStr,OutStrSize);
            return 0;
        }
        USER temp = FindOnlineUserOrIot(10,NULL,atoi(outStr[0]));
        if(temp==NULL)
        {
            CLN tmpw;
            memset(&tmpw,0,sizeof(CLN));
            sprintf(tmpw.checkcode,"%s","00");
            sprintf(tmpw.data,"%s_%s_",outStr[1],outStr[2]);
            tmpw.USERKEY_ID = atoi(outStr[0]);
            sprintf(tmpw.TalktoID,"%s",FindRegisterUserOrIotNode(10,NULL,tmpw.USERKEY_ID)->USERID);
            Send2OnlineUserViaTopServer(tmpw);
            ///***********************************///
            /*USER tmp = FindRegisterUserOrIotNode(10,NULL,atoi(outStr[0]));
            if(tmp == NULL)
            {
                releaseStr(outStr,OutStrSize);
                SEND_OP_BACK("28");//I
                return 0;
            }
            sprintf(a->TalktoID,"%s",tmp->USERID);
            if(NewIotCmdToBeExecute(a,outStr[2],atoi(outStr[1]),atoi(outStr[1])==0?atoi(outStr[2]):0,"00:00:00") != 1)
            {
                releaseStr(outStr,OutStrSize);
                SEND_OP_BACK("28");//I
                return 0;
            }
            SEND_OP_BACK("08");//I*/
        }
        else
        {
            a->SOCKET = temp->USER_SOCKET;
            sprintf(SendStruct.payLoad,"%s_%s_",outStr[1],outStr[2]);
            Encrypt(SendStruct.payLoad,strlen(SendStruct.payLoad),temp->CONNHANDLE->Pin,SendStruct.payLoad);
            SEND_OP_BACK("00");//IOT CMD OPCODE
        }
        releaseStr(outStr,OutStrSize);
    }
    break;
    case 9://STORE CMD 4 OTHER DEV
    {
        if(a->info[0] != 'Y')
            return 0;
        Decrypt(a->data,strlen(a->data),a->Pin,a->data);
        int OutStrSize = 0;
        char** outStr = StrSplit(a->data,200,&OutStrSize,'_');
        if(OutStrSize!=3)
        {
            releaseStr(outStr,OutStrSize);
            return 0;
        }
        USER tmp = FindRegisterUserOrIotNode(10,NULL,atoi(outStr[0]));
        if(tmp == NULL)
        {
            releaseStr(outStr,OutStrSize);
            SEND_OP_BACK("29");//I
            return 0;
        }
        sprintf(a->TalktoID,"%s",tmp->USERID);
        if(NewIotCmdToBeExecute(a,outStr[2],atoi(outStr[1]),atoi(outStr[1])==0?atoi(outStr[2]):0,"00:00:00") != 1)
        {
            releaseStr(outStr,OutStrSize);
            SEND_OP_BACK("29");//I
            return 0;
        }
        SEND_OP_BACK("09");//I
        releaseStr(outStr,OutStrSize);
    }
    break;
    default:
        break;
    }
    return 1;
}
