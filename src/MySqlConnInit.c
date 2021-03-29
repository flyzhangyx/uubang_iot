#include"../head/SERVER.h"
MYSQL* MySqlInit(MYSQL *mysql)
{
    char * host = "flyzhangyx.com";  //因为是作为本机测试，所以填写的是本地IP
    char * user = "iotServer";       //这里改为你的用户名，即连接MySQL的用户名
    char * passwd = "182084Qq."; //这里改为你的用户密码
    char * db = "iotServer";      //这里改为你要连接的数据库的名字
    unsigned int port = 3306;           //这是MySQL的服务器的端口，如果你没有修改过的话就是3306。
    char * unix_socket = NULL;    //unix_socket这是unix下的，我在Windows下，所以就把它设置为NULL
    unsigned long client_flag = 0;      //这个参数一般为0
    if(!mysql_init(mysql))
    {
        return NULL;
    }
    MYSQL* conn=NULL;
    if ((conn=mysql_real_connect(mysql, host, user, passwd, db, port, unix_socket, client_flag)) == NULL ) //连接MySQL
    {
        mysql_close(mysql);
        log_error("Connect to MySQL FAILED , Reason :%s",mysql_error(mysql));
        return NULL;
    }
    else
    {
        //log_debug("Connect to MySQL SUCCESS! ");
        return conn;
    }
}

/*CREATE TABLE `iotserver`.`MessgaeDate(Recommed using date to establish table)` ( `userId` INT UNSIGNED NOT NULL , `friendId` INT UNSIGNED NOT NULL , `Content` TEXT NOT NULL , `SendDate` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ) ENGINE = InnoDB;
INSERT INTO `usermessagecache` (`userId`, `friendId`, `Content`, `SendDate`) VALUES ('3', '4', '1232の21呃13の呃3非非法违法', CURRENT_TIMESTAMP);
*/

