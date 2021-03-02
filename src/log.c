#include"../head/SERVER.h"
void logwrite(char* log)
{
    fputs(log,loginfo);
    fflush(loginfo);
}
