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

