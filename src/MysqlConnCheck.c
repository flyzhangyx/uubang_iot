#include "../head/SERVER.h"
int mysql_master_connect_ping(MYSQL *mysql)
{
    if(mysql_ping(mysql) != 0)
    {
        pthread_mutex_lock(mysql_mutex);
        /*close & reconnect*/
        if(mysql_ping(mysql) != 0)
        {
            mysql_close(mysql);
            if(!MySqlInit(mysql))
            {
                pthread_mutex_unlock(mysql_mutex);
                return 0;
            }
        }
        else
        {
            pthread_mutex_unlock(mysql_mutex);
            return 2;
        }
        pthread_mutex_unlock(mysql_mutex);
        return 1;
    }
    else
    {
        return 1;
    }
}
