#include "../head/SERVER.h"
int mysql_master_connect_ping()
{
    if(mysql_ping(&mysql) != 0)
    {
        pthread_mutex_lock(&mysql_mutex);
        /*close & reconnect*/
        if(mysql_ping(&mysql) != 0)
        {
            mysql_close(sock);
            MySqlInit();
            if(NULL==sock)
            {
                pthread_mutex_unlock(&mysql_mutex);
                exit(-11);
            }
        }
        else
        {
            pthread_mutex_unlock(&mysql_mutex);
            return 2;
        }
        pthread_mutex_unlock(&mysql_mutex);
        return 1;
    }
    else
    {
        return 1;
    }
}
