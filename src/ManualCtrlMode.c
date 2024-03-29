#include "../head/SERVER.h"
void PrintModeScreen()
{
    FILE *Disp = fopen("Display.txt","r");
    char dispStr[100]="";
    while(!feof(Disp))
    {
        fgets(dispStr,100,Disp);
        printf("%s",dispStr);
    }
    fclose(Disp);
}
void EnterManualCtrlMode()
{
    int selectMode;
    int isExit;
    while(!isShutDown)
    {
        isExit = 0;
        getch();
        system("cls");
        ManualCtrl = 1;
        while(!isExit)
        {
            PrintModeScreen();
            selectMode = getch();
            system("cls");
            switch(selectMode)
            {
            case '1':
                ConnectToTopServer();
                printf("Cmd Down\n");
                break;
            case '2':
                if(!getAppVersion())
                {
                    printf("Input App Version:");
                    gets(app_version);
                    system("cls");
                    printf("\nNow App Version is %s",app_version);
                }
                break;
            case '3':
                PrintAllUserAndIotDevice();
                break;
            case '4':
                FindOnlineUserOrIot(111,NULL,0);
                break;
            case '5':
                isExit = 1;
                break;
            case '6':
                isShutDown = 1;
                break;
            default:
                printf("Input Err");
                break;
            }
            system("pause");
            system("cls");
        }
        ManualCtrl = 0;
    }
}
