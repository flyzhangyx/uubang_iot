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
