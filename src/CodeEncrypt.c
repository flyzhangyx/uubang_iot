#include<stdio.h>
#include<string.h>
void Encrypt(char *source_in ,int len,char *PinCode,char *source_out)
{
    memset(source_out,0,len+1);
    int i = 0;
    while(i<len)
    {
        source_out[i] = source_in[i]+PinCode[i%6];
        i++;
    }
    return ;
}

void Decrypt(char *source_in ,int len,char *PinCode,char *source_out)
{
    memset(source_out,0,len+1);
    int i = 0;
    while(i<len)
    {
        source_out[i] = source_in[i]-PinCode[i%6];
        i++;
    }
    return ;
}

//int main()
//{
//    char in[27]="qwertyuiopasdfghjklzxcvbnm";
//    char out[27]="";
//    Encrypt(in,strlen(in),"735483",out);
//    int i=0;
//    while(i<strlen(in))
//    {
//        log_debug("%x",out[i]);
//        i++;
//    }
//    Decrypt(out,strlen(out),"735483",in);
//    log_debug("");
//    i=0;
//    while(i<strlen(in))
//    {
//        log_debug("%c",in[i]);
//        i++;
//    }
//    return 1;
//}
