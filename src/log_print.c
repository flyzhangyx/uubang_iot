//封装带颜色打印接口
int ManualCtrl;
#ifdef DEBUG_PRINT
#define log_debug(format, args...)     do{  if(!ManualCtrl) \
                                            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_GREEN);\
                                            time_t now_time;\
                                            time(&now_time);\
                                            char time_now[50];\
                                            strftime(time_now,50,"%Y-%m-%d %X",localtime(&now_time));\
                                            if(!ManualCtrl)\
                                            printf("[%s][DBG][%s:%d] " #format "\n",time_now,__func__,__LINE__,##args) ;\
                                            char log[1000];\
                                            sprintf(log,"[%s][DBG][%s:%d] " #format "\n", time_now,__func__,__LINE__,##args);\
                                            logwrite(log);\
                                            }while(0)
#else
#define log_debug(format, args...)
#endif // DEBUG_PRINT

#ifdef INFO_PRINT
#define log_info(format, args...)      do{  if(!ManualCtrl) \
                                            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_BLUE|BACKGROUND_RED|BACKGROUND_GREEN);\
                                            time_t now_time;\
                                            time(&now_time);\
                                            char time_now[50];\
                                            strftime(time_now,50,"%Y-%m-%d %X",localtime(&now_time));\
                                            if(!ManualCtrl)\
                                            printf("[%s][INF][%s:%d] " #format "\n", time_now,__func__,__LINE__,##args);\
                                            char log[1000];\
                                            sprintf(log,"[%s][INF][%s:%d] " #format "\n", time_now,__func__,__LINE__,##args);\
                                            logwrite(log);\
                                            }while(0)
#else
#define log_info(format, args...) do{  \
                                            time_t now_time;\
                                            time(&now_time);\
                                            char time_now[50];\
                                            strftime(time_now,50,"%Y-%m-%d %X",localtime(&now_time));\
                                            char log[1000];\
                                            sprintf(log,"[%s][INF][%s:%d] " #format "\n", time_now,__func__,__LINE__,##args);\
                                            logwrite(log);\
                                            }while(0)
#endif // INFO_PRINT

#define log_error(format, args...)       do{ if(!ManualCtrl) \
                                            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_RED);\
                                            time_t now_time;\
                                            time(&now_time);\
                                            char time_now[50];\
                                            strftime(time_now,50,"%Y-%m-%d %X",localtime(&now_time));\
                                            if(!ManualCtrl)\
                                            printf("[%s][ERR][%s:%d] " #format "\n", time_now,__func__,__LINE__,##args);\
                                            char log[1000];\
                                            sprintf(log,"[%s][ERR][%s:%d] " #format "\n", time_now,__func__,__LINE__,##args);\
                                            logwrite(log);\
                                            }while(0)
//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), g_default_color);
