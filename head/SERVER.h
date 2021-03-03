#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED
#include <stdio.h>
#include <WinSock2.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <direct.h>
#include <time.h>
#include "mysql.h"
#include <malloc.h>
#include "stpool.h"
#include "../head/libThreadPool.h"
#include "sqlpool.h"
#define msleep Sleep
#define BUFSIZE 512
//封装带颜色打印接口
#ifdef DEBUG_PRINT
    #define log_debug(format, args...)     do{ SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_GREEN);\
                                            time_t now_time;\
                                            time(&now_time);\
                                            char time_now[50];\
                                            strftime(time_now,50,"%Y-%m-%d %X",localtime(&now_time));\
                                            printf("[%s][DBG][%s:%d] " #format "\n",time_now,__func__,__LINE__,##args) ;\
                                            char log[1000];\
                                            sprintf(log,"[%s][DBG][%s:%d] " #format "\n", time_now,__func__,__LINE__,##args);\
                                            logwrite(log);\
                                            }while(0)
#else
#define log_debug(format, args...) //Nothing
#endif // DEBUG_PRINT

#ifdef INFO_PRINT
#define log_info(format, args...)       do{ SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE|BACKGROUND_RED|BACKGROUND_GREEN);\
                                            time_t now_time;\
                                            time(&now_time);\
                                            char time_now[50];\
                                            strftime(time_now,50,"%Y-%m-%d %X",localtime(&now_time));\
                                            printf("[%s][INF][%s:%d] " #format "\n", time_now,__func__,__LINE__,##args);\
                                            char log[1000];\
                                            sprintf(log,"[%s][INF][%s:%d] " #format "\n", time_now,__func__,__LINE__,##args);\
                                            logwrite(log);\
                                            }while(0)
#else
#define log_info(format, args...)
#endif // INFO_PRINT

#define log_error(format, args...)       do{ SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_RED);\
                                            time_t now_time;\
                                            time(&now_time);\
                                            char time_now[50];\
                                            strftime(time_now,50,"%Y-%m-%d %X",localtime(&now_time));\
                                            printf("[%s][ERR][%s:%d] " #format "\n", time_now,__func__,__LINE__,##args);\
                                            char log[1000];\
                                            sprintf(log,"[%s][ERR][%s:%d] " #format "\n", time_now,__func__,__LINE__,##args);\
                                            logwrite(log);\
                                            }while(0)
//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), g_default_color);
///**************在线用户节点****************
struct user
{
    SOCKET USER_socket;
    SOCKADDR_IN USER_socket_udp;
    char USERID[12];
    int USERKEY_ID;
    char USERPASSWORD[33];
    char info[100];
    struct user *next;
};
typedef struct user * USER;

struct OnlineUserHead
{
    char DATE[100];
    char TIME[18];
    char info[100];
    pthread_mutex_t mute;
    int OnlineUserNum;
    USER  next;
};
///*******************************************
struct OnlineUserHead * onlineUserHead;
struct OnlineUserHead * RegistedUserHead;
struct OnlineUserHead * onlineIotHead;
struct OnlineUserHead * RegistedIotHead;
///*****************Send接口**********************
typedef struct
{
    char checkcode[18];
    char USERID[12];
    char USERPASSWORD[33];
    char TalktoID[12];
    char REUSERPASSWORD[33];
    char DATA[513];
    char save[100];
} UserPacketInterface;


typedef struct
{
    int publicKey;
    int privateKey;
    int commonKey;
    int encryptBlockBytes;
} RSAKey;
///********函数接口*************
typedef struct
{
    SOCKET remote_socket;
    SOCKADDR_IN ADDR;
    char USERID[12];
    int USERKEY_ID;
    char USERPASSWORD[33];
    char checkcode[18];
    char DATE[100];
    char TalktoID[12];
    char REUSERPASSWORD[33];
    char data[513];
    char info[100];//[0]是否注销登录,N_Y;
    char Pin[7];
    RSAKey key;
    pthread_mutex_t t;
} CLN;
typedef struct
{
    OVERLAPPED overlapped;
    WSABUF WSADATABUF;
    char RECBUFFER[sizeof(UserPacketInterface) ];
    int BufferLen;
    int OpCode;
} *LPPER_IO_DATA, PER_IO_DATA;

typedef struct
{
    char opCode[3];
    char SeqNum[2];
    char payLoad[100];//Max
} IotPacketInterface;

///*****************************
///*******函数声明**************
#ifdef STPOOL
int talk(struct sttask*);
#else
int talk(LPVOID);
#endif // STPOOL
DWORD WINAPI fun(LPVOID);
DWORD WINAPI UdpPackResolve(LPVOID);
DWORD WINAPI file_tcp_thread(LPVOID);
DWORD WINAPI CreateDailyMsgdbThread();
DWORD WINAPI ServerWorkThread(LPVOID lpParam);
int StartThread(CLN* );
int AcceptClient();
int initServer(int);
int newOnlineUserOrIotDev(CLN*);
int delete_out_user(CLN*);
USER FindOnlineUserOrIot(int,char*,int);
int SIGNIN(CLN*);
int AddtoLocal(CLN*);
int Check_Id_Pwd(int,CLN*);
int UserRegiter(CLN*);
int Register(CLN*,int);
int UserRePwd(CLN*);
void RequestIotDevices(CLN*);
void RequestIotEvent(CLN*);
void logwrite(char*);
int IoTtalk(CLN*);
int Stringcut(char* str,int m,int n,char *des);
MYSQL* MySqlInit(MYSQL*);
USER FindRegisterUserOrIotNode(int,char*,int);
void generateRandString(char*dest,unsigned int len);
void generateRandIntStr(char *dest,unsigned int len);
int IotRegister(CLN*,int);
void PrintAllUserAndIotDevice();
int NewUserFriend(CLN*,int);
int NewUserIot (CLN*,int);
int CmpDate(int year, int month, int day);
int NewUserMsgTableInSQL();
int UserRequestMessage(CLN*,int, char*,char*,struct tm*);
int UserReqFriendRel(CLN*);
int NewUserMsgStorage(CLN*,int);
int IotUpdateStatus(CLN*,int,int );
int UserGetIotData(CLN*);
int UserReqIotRel(CLN*);
void CreateDailyMsgdb();
void CopyUserPacketInterface2Cln(UserPacketInterface UserPacketInterface,CLN *a);
unsigned int DJBHash(char* str, unsigned int len);
void InitRSA(RSAKey*);
void encodeMessage(int len, int bytes, char* message,int* outCrypto, int exponent, int modulus);
void decodeMessage(int len, int bytes, int* cryptogram,char *outSource, int exponent, int modulus);
char* GetUpdateTimeStamp(int UserId,int index,char*);
int UpdateSqlInfoTimestamp(int UserId,int index,int flag);
int UpdateLocalRegUserAndIotlist();
void Decrypt(char *source_in,int len,char *PinCode,char *source_out);
void Encrypt(char *source_in,int len,char *PinCode,char *source_out);
int NewIotCmdToBeExecute(CLN *a,char *cmd,int Devclass,int status,char *CmdTimeStamp);
int IotReadCmd(CLN *a,int Devclass,int del);
int mysql_master_connect_ping();
void CopyRecIotData2Cln(char *recBuff,CLN* CONN,int len);
void StartCheckUserScene();
int NewUserSceneCmdStore(CLN*,char*,int,int,int,char*,char*);
int ReadOrDeleteUserScene(CLN*,char*,int,int);
int Send2OnlineUserViaTopServer(CLN a);
void ConnectToTopServer();
void getNetIp(char *ip);
int Initialize();
int GetCPUUseRate();
void StartCheckUserScene();
int GetRamUse();
int addConnMemWait4Free(CLN* Conn,LPPER_IO_DATA PerIoData);
void freeConnMemWait4Free();
///*****************************
///***************各类标志码**********************
char CHECK[3];///应用进入时登陆检测是否已经注册
char SIGN_IN[3];///登陆码
char REGISTER[3];///注册码
char MESSAGE[3];///信息发送码
char TALK_TO[3];///通信目标地址码
char SIGN_OUT[3];///注销
char CONTACT[3];///联系人码
char REPWD[3];
unsigned int CHECK_HASH;///应用进入时登陆检测是否已经注册
unsigned int SIGN_IN_HASH;///登陆码
unsigned int REGISTER_HASH;///注册码
unsigned int MESSAGE_HASH;///信息发送码
unsigned int TALK_TO_HASH;///通信目标地址码
unsigned int SIGN_OUT_HASH;///注销
unsigned int CONTACT_HASH;///联系人码
unsigned int REPWD_HASH;///改密
unsigned int VERUPD_HASH ;///APP升级
unsigned int ADDUSER_HASH;///添加用户
unsigned int ADSUSER_HASH;///添加用户
unsigned int READDU_HASH;///回复添加用户
unsigned int HEARTBEAT_HASH;//HEART
unsigned int RSAREQ_HASH;//HEART
unsigned int PINREQ_HASH;//PINCODE
unsigned int IOTCFM_HASH;//IOT ADD CONFIRM
///************************************************
///**********全局变量****************
SOCKET server_sockfd;//TCP服务器端套接字
SOCKET server_sockfd_udp;//UDP服务器端套接字
HANDLE CLN_thread[200];
int CLN_num;
int warnfile;
int logflag;
FILE* REGISTERlocal;
FILE* loginfo;
SQL_CONN_POOL *MySqlConnPool;
pthread_mutex_t mysql_mutex;
#ifdef STPOOL
stpool_t * ThreadPool;
#else
threadPool_t *ThreadPool;
#endif // STPOOL
int g_default_color ;
char app_version[4];
char NetIP[20];
///**********************************
#endif // SERVER_H_INCLUDED
