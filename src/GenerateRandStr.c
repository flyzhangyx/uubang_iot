#include "../head/SERVER.h"
void generateRandString(char *dest,unsigned int len)
{
    const unsigned char allChar[63]="0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned int cnt,randNo;
    for(cnt=0; cnt<len; cnt++)
    {
        randNo=rand()%62;
        *dest=allChar[randNo];
        dest++;
    }
    *dest='\0';
}
void generateRandIntStr(char *dest,unsigned int len)
{
    const unsigned char allChar[11]="0123456789";
    unsigned int cnt,randNo;
    for(cnt=0; cnt<len; cnt++)
    {
        randNo=rand()%10;
        *dest=allChar[randNo];
        dest++;
    }
    *dest='\0';
}
