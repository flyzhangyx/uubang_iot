#include"../head/SERVER.h"
int Register(CLN *a,int method)
{
    if(Check_Id_Pwd(1,a)==1)
    {
        log_info("USER:%s has Registed",a->USERID);
        return -1;
    }
    else
    {
        if(method==1)
        {
            if(IotRegister(a,(int)a->info[3])==-1)
            {
                log_info("IOTDEV:%s Registed Fail !",a->USERID);
                return -1;
            }
        }
        else
        {
            if(UserRegiter(a)==-1)
            {
                log_info("USER:%s Registed Fail !",a->USERID);
                return -1;
            }
        }
        //mkdir(a->USERID);
        log_info("USER/IOTDEV:%s Registed Success !",a->USERID);
        UpdateLocalRegUserAndIotlist();
        return 1;
    }
}
