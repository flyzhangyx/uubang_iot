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
    CLN b;
    strcpy(b.info,a->info);
    b.ADDR=a->ADDR;
    b.remote_socket=a->remote_socket;
    strcpy(b.USERID,a->USERID);
    strcpy(b.USERPASSWORD,a->USERPASSWORD);
    USER Tag = RegistedUserHead->next;
    b.USERKEY_ID = 1;
    if(Tag!=NULL)
    b.USERKEY_ID = Tag->USERKEY_ID+1;
    AddtoLocal(&b);
    return 1;
}
