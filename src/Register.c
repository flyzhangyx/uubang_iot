#include"../head/SERVER.h"
int Register(CLN *a,int method)
{
    if(Check_Id_Pwd(1,a)==1)
    {
        printf("\nUSER:%s has Registed\n",a->USERID);
        return -1;
    }
    else
    {
        if(method==1)
        {
            if(IotRegister(a,0)==-1)
            {
                printf("\nIOTDEV:%s Registed Fail !\n",a->USERID);
                return -1;
            }
        }
        else
        {
            if(UserRegiter(a)==-1)
            {
                printf("\nUSER:%s Registed Fail !\n",a->USERID);
                return -1;
            }
        }
        //mkdir(a->USERID);
        printf("\nUSER/IOTDEV:%s Registed Success !\n",a->USERID);
        return 1;
    }
}
