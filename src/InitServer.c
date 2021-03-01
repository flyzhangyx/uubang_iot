#include"../head/SERVER.h"
int initServer(int port)
{
    int scan;
    CLN_num=0;
    logflag=0;
    strcpy(CHECK,"ZY");
    strcpy(SIGN_IN,"SI");//��½��
    strcpy(REGISTER,"RE");//ע����
    strcpy(MESSAGE,"ME");//��Ϣ������
    strcpy(TALK_TO,"TA");//ͨ��Ŀ���ַ��
    strcpy(SIGN_OUT,"SO");//ע����¼��
    strcpy(REPWD,"RP");
    ///FIll the Three CHAR full With A ��TA + A = TAA
    CHECK_HASH = DJBHash("ZYX",3);///Ӧ�ý���ʱ��½����Ƿ��Ѿ�ע��
    log_info("ZYX%d",CHECK_HASH);
    SIGN_IN_HASH = DJBHash("SIA",3);///��½��
    log_info("SIA%d",SIGN_IN_HASH);
    REGISTER_HASH = DJBHash("REA",3);///ע����
    log_info("REA%d",REGISTER_HASH);
    MESSAGE_HASH = DJBHash("RME",3);///��Ϣ������
    log_info("RME%d",MESSAGE_HASH);
    TALK_TO_HASH = DJBHash("TAA",3);///ͨ��Ŀ���ַ��
    log_info("TAA%d",TALK_TO_HASH);
    SIGN_OUT_HASH = DJBHash("STO",3);///ע��
    log_info("STO%d",SIGN_OUT_HASH);
    CONTACT_HASH = DJBHash("RCO",3);///��ϵ����
    log_info("RCO%d",CONTACT_HASH);
    REPWD_HASH = DJBHash("RPA",3);
    log_info("RPA%d",REPWD_HASH);
    VERUPD_HASH = DJBHash("UPD",3);
    log_info("UPD%d",VERUPD_HASH);
    ADDUSER_HASH = DJBHash("ADD",3);
    log_info("ADD%d",ADDUSER_HASH);
    READDU_HASH = DJBHash("ADS",3);
    log_info("ADS%d",READDU_HASH);
    HEARTBEAT_HASH = DJBHash("HBA",3);
    log_info("HBA%d",HEARTBEAT_HASH);
    PINREQ_HASH = DJBHash("PIN",3);
    log_info("PIN%d",PINREQ_HASH);
    IOTCFM_HASH = DJBHash("IOC",3);
    log_info("IOC%d",IOTCFM_HASH);
    ///***********socket��ʼ��***********************
    WSADATA wsaData;
    while(1)
    {
        if(!WSAStartup(MAKEWORD(2,2),&wsaData) )
        {
            log_info("SOCKET ESTABLISHED SUCCESS!");
            break;
        }
        else
        {
            log_info("socket not established! If continue to establish? Yes press 1;No press 0");
            scanf("%d",&scan);
            if(scan==0)
                exit(0);
            else if(scan==1)
                continue;
            else
            {
                log_error("Input err! Late exit!");
                exit(0);
            }
        }
    }
    ///*************************************************
    SOCKADDR_IN my_addr; //�����������ַ�ṹ��
    memset(&my_addr,0,sizeof(my_addr)); //���ݳ�ʼ��--����
    my_addr.sin_family=AF_INET; //����ΪIPͨ��
    my_addr.sin_addr.s_addr=htonl(INADDR_ANY);//������IP��ַ--�������ӵ����б��ص�ַ��
    my_addr.sin_port=htons(port); //�������˿ں� /*�������������׽���--IPv4Э�飬��������ͨ�ţ�TCPЭ��*/
    if((server_sockfd=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
    {
        perror("socket");    /*���׽��ְ󶨵��������������ַ��*/
        return -1;
    }
    if (bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))==SOCKET_ERROR)
    {
        perror("bind");
        return -1;
    }
    MySqlInit();///MySQL INIT



    ///*******�����û�����ͷ��ʼ��********
    do
    {
        onlineUserHead=(struct OnlineUserHead *)malloc(sizeof(struct OnlineUserHead));
        onlineIotHead=(struct OnlineUserHead *)malloc(sizeof(struct OnlineUserHead));
    }
    while(onlineUserHead==NULL&&onlineIotHead==NULL);
    strcpy(onlineUserHead->DATE,"\0");
    onlineUserHead->OnlineUserNum=0;
    onlineUserHead->next=NULL;
    pthread_mutex_init(&(onlineUserHead->mute),NULL);
    strcpy(onlineIotHead->DATE,"\0");
    onlineIotHead->OnlineUserNum=0;
    onlineIotHead->next=NULL;
    pthread_mutex_init(&(onlineIotHead->mute),NULL);
    ///LogFile
    loginfo=fopen("Loginfo.info","a+");
      //��ȡ�ն˵�ǰĬ����ɫ�����ں������д�ӡ��Ĭ����ɫ����
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    GetConsoleScreenBufferInfo(h, &csbiInfo);
    g_default_color = csbiInfo.wAttributes;

    do
    {
        RegistedUserHead=(struct OnlineUserHead *)malloc(sizeof(struct OnlineUserHead));///��Onlineͬ���ķ�ʽ����
        RegistedIotHead=(struct OnlineUserHead *)malloc(sizeof(struct OnlineUserHead));
    }
    while(RegistedUserHead==NULL&&RegistedIotHead==NULL);
    strcpy(RegistedUserHead->DATE,"\0");
    RegistedUserHead->OnlineUserNum=0;
    RegistedUserHead->next=NULL;
    pthread_mutex_init(&(RegistedUserHead->mute),NULL);
    strcpy(RegistedIotHead->DATE,"\0");
    RegistedIotHead->OnlineUserNum=0;
    RegistedIotHead->next=NULL;
    pthread_mutex_init(&(RegistedIotHead->mute),NULL);
    CLN a;
    memset(&a,0,sizeof(CLN));
    char *head = "SELECT * FROM user";
    char *head_iot = "SELECT * FROM iotnode";
    char query[50] = "";
    MYSQL_RES *res;
    MYSQL_ROW row;
    sprintf(query, "%s", head);
    mysql_master_connect_ping();
    if (mysql_real_query(&mysql, query, strlen(query)))
    {
        log_error("Failed to Get UserInfo: %s", mysql_error(&mysql));
        return -1;
    }
    res = mysql_store_result(&mysql);
    while ((row = mysql_fetch_row(res)))
    {
        strcpy(a.USERID,row[1]);
        strcpy(a.USERPASSWORD, row[3]);
        a.USERKEY_ID=atoi(row[0]);
        strcpy(a.info,row[4]) ;
        AddtoLocal(&a);
    }
    mysql_free_result(res);
    ///...................................................
    memset(query,0,50*sizeof(char));
    sprintf(query, "%s", head_iot);
    MYSQL_RES *res_iot;
    MYSQL_ROW row_iot;
    mysql_master_connect_ping();
    if (mysql_real_query(&mysql, query, strlen(query)))
    {
        log_error("Failed to Get IotInfo: %s", mysql_error(&mysql));
        return -1;
    }
    res_iot = mysql_store_result(&mysql);
    while ((row_iot = mysql_fetch_row(res_iot)))
    {
        strcpy(a.USERID,row_iot[1]);
        strcpy(a.USERPASSWORD, row_iot[3]);
        a.USERKEY_ID=atoi(row_iot[0]);
        strcpy(a.info,row_iot[4]) ;
        strcpy(a.checkcode,row_iot[2]);
        AddtoLocal(&a);
    }
    mysql_free_result(res_iot);
    log_info("Local Database IotDevices Get %d",RegistedIotHead->OnlineUserNum);
    log_info("Local Database Users Get %d",RegistedUserHead->OnlineUserNum);
    FILE* updversion=fopen("update","r");
    if(updversion)
    {
        fgets(app_version,4,updversion);
        fclose(updversion);
        app_version[3]='\0';
        log_info("APP�汾-%s",app_version);
    }
    else
    {
        log_error("�����ļ���ȡʧ��!");
        return -1;
    }
    memset(NetIP,0,20);
    getNetIp(NetIP);
    Initialize();//init CPU Load
    ///*************************************
    return 1;
}
