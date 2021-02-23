#include"../head/SERVER.h"
int IotRegister(CLN* a,int DevClass)
{
    char insert[200] = "";
    sprintf(insert,"%s%s%s%d%s%s%s%s",
            "INSERT INTO `iotnode` (`id`, `pinCode`, `deviceClass`, `openId`, `RegistDate`) VALUES (NULL, '",
            a->USERID,
            "', '",
            DevClass,
            "', '",
            a->USERPASSWORD,
            "', ",
            "CURRENT_TIMESTAMP)");
    //printf("\n%s",insert);
    if(mysql_real_query(&mysql,insert,strlen(insert)))
    {
        printf("\nMySQL ERR :%s",mysql_error(&mysql));
        return -1;
    }
    return 1;
}
