#include"../head/SERVER.h"
MYSQL* MySqlInit(MYSQL *mysql)
{
    char * host = "flyzhangyx.com";  //��Ϊ����Ϊ�������ԣ�������д���Ǳ���IP
    char * user = "iotServer";       //�����Ϊ����û�����������MySQL���û���
    char * passwd = "182084Qq."; //�����Ϊ����û�����
    char * db = "iotServer";      //�����Ϊ��Ҫ���ӵ����ݿ������
    unsigned int port = 3306;           //����MySQL�ķ������Ķ˿ڣ������û���޸Ĺ��Ļ�����3306��
    char * unix_socket = NULL;    //unix_socket����unix�µģ�����Windows�£����ԾͰ�������ΪNULL
    unsigned long client_flag = 0;      //�������һ��Ϊ0
    if(!mysql_init(mysql))
    {
        return NULL;
    }
    MYSQL* conn=NULL;
    if ((conn=mysql_real_connect(mysql, host, user, passwd, db, port, unix_socket, client_flag)) == NULL ) //����MySQL
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
INSERT INTO `usermessagecache` (`userId`, `friendId`, `Content`, `SendDate`) VALUES ('3', '4', '1232��21��13����3�ǷǷ�Υ��', CURRENT_TIMESTAMP);
*/

