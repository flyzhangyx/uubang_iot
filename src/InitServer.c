#include"../head/SERVER.h"
extern int isConnected;
int initServer(int port,char *ServerId)
{
    int scan;
    ManualCtrl = 0;
    isShutDown = 0;
    isConnected = 0;
    sprintf(ServerID,"%s", ServerId);
    CLN_num=0;
    strcpy(CHECK,"ZY");
    strcpy(SIGN_IN,"SI");//登陆码
    strcpy(REGISTER,"RE");//注册码
    strcpy(MESSAGE,"ME");//信息发送码
    strcpy(TALK_TO,"TA");//通信目标地址码
    strcpy(SIGN_OUT,"SO");//注销登录码
    strcpy(REPWD,"RP");
    ///FIll the Three CHAR full With A ：TA + A = TAA
#ifdef HASH_CODE
    CHECK_HASH = DJBHash("ZYX",3);///应用进入时登陆检测是否已经注册
    log_debug("ZYX%d",CHECK_HASH);
    SIGN_IN_HASH = DJBHash("SIA",3);///登陆码
    log_debug("SIA%d",SIGN_IN_HASH);
    REGISTER_HASH = DJBHash("REA",3);///注册码
    log_debug("REA%d",REGISTER_HASH);
    MESSAGE_HASH = DJBHash("RME",3);///信息发送码
    log_debug("RME%d",MESSAGE_HASH);
    TALK_TO_HASH = DJBHash("TAA",3);///通信目标地址码
    log_debug("TAA%d",TALK_TO_HASH);
    SIGN_OUT_HASH = DJBHash("STO",3);///注销
    log_debug("STO%d",SIGN_OUT_HASH);
    CONTACT_HASH = DJBHash("RCO",3);///联系人码
    log_debug("RCO%d",CONTACT_HASH);
    REPWD_HASH = DJBHash("RPA",3);
    log_debug("RPA%d",REPWD_HASH);
    VERUPD_HASH = DJBHash("UPD",3);
    log_debug("UPD%d",VERUPD_HASH);
    ADDUSER_HASH = DJBHash("ADD",3);
    log_debug("ADD%d",ADDUSER_HASH);
    READDU_HASH = DJBHash("ADS",3);
    log_debug("ADS%d",READDU_HASH);
    HEARTBEAT_HASH = DJBHash("HBA",3);
    log_debug("HBA%d",HEARTBEAT_HASH);
    PINREQ_HASH = DJBHash("PIN",3);
    log_debug("PIN%d",PINREQ_HASH);
    IOTCFM_HASH = DJBHash("IOC",3);
    log_debug("IOC%d",IOTCFM_HASH);//iot cfm add friend cmd
#endif // HASH_CODE
    ///***********socket初始化***********************
    WSADATA wsaData;
    while(1)
    {
        if(!WSAStartup(MAKEWORD(2,2),&wsaData) )
        {
            log_debug("SOCKET ESTABLISHED SUCCESS!");
            break;
        }
        else
        {
            log_error("socket not established! If continue to establish? Yes press 1;No press 0");
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
    SOCKADDR_IN my_addr; //服务器网络地址结构体
    memset(&my_addr,0,sizeof(my_addr)); //数据初始化--清零
    my_addr.sin_family=AF_INET; //设置为IP通信
    my_addr.sin_addr.s_addr=htonl(INADDR_ANY);//服务器IP地址--允许连接到所有本地地址上
    my_addr.sin_port=htons(port); //服务器端口号 /*创建服务器端套接字--IPv4协议，面向连接通信，TCP协议*/
    if((server_sockfd=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
    {
        perror("socket");    /*将套接字绑定到服务器的网络地址上*/
        return -1;
    }
    if (bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))==SOCKET_ERROR)
    {
        perror("bind");
        return -1;
    }
    ///**********Init Thread Pool************///
#ifdef STPOOL
    long eCAPs = eCAP_F_DYNAMIC|eCAP_F_ROUTINE|eCAP_F_TASK_WAIT_ALL;
    /** 创建线程池 */
    ThreadPool_ExecuteMsg = stpool_create("MSG_EXE_POOL", /** 线程池名                      */
                                          eCAPs,    /** 期望libstpool提供的的功能特性 */
                                          35,	   /** 线程池中运行的最大线程数目    */
                                          20,	   /** 预启动提供服务的的线程数目    */
                                          0,	   /** 保持线程池创建后调度任务状态  */
                                          1		   /** 优先级队列数目                */
                                         );
    ThreadPool_ExecuteTask = stpool_create("EXE_TASK_POOL", /** 线程池名                      */
                                           eCAPs,    /** 期望libstpool提供的的功能特性 */
                                           20,	   /** 线程池中运行的最大线程数目    */
                                           5,	   /** 预启动提供服务的的线程数目    */
                                           0,	   /** 保持线程池创建后调度任务状态  */
                                           1		   /** 优先级队列数目                */
                                          );
    log_debug("ThreadPool Init Success!");
    MySqlConnPool=sql_pool_create(36);
    log_debug("MySQLConnPoll Init Success!");
#ifdef MemPool
    InitMemPool(200000,sizeof(CLN));
#else
    mem_size_t max_mem = 500 * MB;
    mem_size_t mem_pool_size = 300 * MB;
    mp = MemoryPoolInit(max_mem, mem_pool_size);
#endif // MemPool
    log_debug("MemPoll Init Success!");
    /**********************************/
#else
    ThreadPool_ExecuteMsg = libThreadPool_Init(10,30,60);
#endif
    ///*******在线用户链表头初始化********
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
    char log_path[100] ="";
    sprintf(log_path,"Port_%d_%s",port,"iotServer.log");
    loginfo=fopen(log_path,"a+");
    //获取终端当前默认颜色，用于后续所有打印的默认颜色配置
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    GetConsoleScreenBufferInfo(h, &csbiInfo);
    g_default_color = csbiInfo.wAttributes;

    do
    {
        RegistedUserHead=(struct OnlineUserHead *)malloc(sizeof(struct OnlineUserHead));///跟Online同样的方式储存
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
    SQL_NODE *temmp;
    if((temmp=get_db_connect(MySqlConnPool))==NULL)
    {
        log_error("SQL NODE NULL");
        return 0;
    }
    MYSQL *mysql=&(temmp->fd);
    if (mysql_real_query(mysql, query, strlen(query)))
    {
        release_node(MySqlConnPool, temmp);
        log_error("Failed to Get UserInfo: %s", mysql_error(mysql));
        return -1;
    }
    res = mysql_store_result(mysql);
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
    if (mysql_real_query(mysql, query, strlen(query)))
    {
        release_node(MySqlConnPool, temmp);
        log_error("Failed to Get IotInfo: %s", mysql_error(mysql));
        return -1;
    }
    res_iot = mysql_store_result(mysql);
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
    release_node(MySqlConnPool, temmp);
    getAppVersion();
    memset(NetIP,0,20);
    getNetIp(NetIP);
    Initialize();//init CPU Load
    Con2FreeLink_mutex = CreateMutex(NULL, FALSE, NULL);
    CreateServerInfoInSQL();
    ///*************************************
    return 1;
}
