#include "stdafx.h"
#include "globalvars.h"
#include "AskPasswords.h"
#include "console.h"

/*
Function:
Asks the master password for add new subclients for Username
The maximum size of the master password is 32 characters.
return:
NULL - FAILED
char* - Password String
*/
char* AskMasterPassword()
{
    char* AllData = NULL;
    AllData = (char*)VirtualAlloc(NULL, 32, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL) return AllData;
    memset(AllData, '-', 32);
    int iCount = 0;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    gotoxy(0, 30);
    clear_screen(0, 15);
    gotoxy(0, 15);
    printf("[MASTER] password for all clients (max 32) or q for exit: ");
    do
    {
        AllData[iCount] = _getch();
        printf("%c", AllData[iCount]);
        
        if (AllData[iCount] == 'q') exit(0);

        if (AllData[iCount] == 3)
        {
            printf("\r\nCancel and exit.\r\n");
            VirtualFree(AllData, 0, MEM_RELEASE);
            return NULL;
        }
        if (AllData[iCount] == 26)
        {
            printf("\r\nCancel and exit.\r\n");
            VirtualFree(AllData, 0, MEM_RELEASE);
            return NULL;
        }
        if (AllData[iCount] == 13)
        {
            AllData[iCount] = '-';
            printf("\tMaster password for Add/Remove SubClients and ReLogin.\r\n");
            return AllData;
        }
        iCount++;
    } while (iCount != 32);

    return AllData;
}

/*
Function:
Asks the password for the crypto container, it stores the settings and important variables
The maximum size of the crypto container password is 32 characters.

var:
char* Message - Addd text message for user

return:
NULL - FAILED
char* - Password String
*/
char* AskContainerPassword(char* Message)
{
    char* AllData = NULL;
    AllData = (char*)VirtualAlloc(NULL, 32, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL) return AllData;
    memset(AllData, '-', 32);
    int iCount = 0;
    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);
    
    gotoxy(0, 30);
    clear_screen(0, 15);
    gotoxy(0, 15);
    if (Message == NULL)
    {
        printf("Container password (max 32)  or q for exit: ");
    }
    else {
        printf("%s or q for exit: ", Message);
    }
    
    do
    {
        AllData[iCount] = _getch();
        printf("%c", AllData[iCount]);

        if (AllData[iCount] == 'q') exit(0);

        if (AllData[iCount] == 3)
        {
            printf("\r\nCancel and exit.\r\n");
            VirtualFree(AllData, 0, MEM_RELEASE);
            return NULL;
        }
        if (AllData[iCount] == 26)
        {
            printf("\r\nCancel and exit.\r\n");
            VirtualFree(AllData, 0, MEM_RELEASE);
            return NULL;
        }
        if (AllData[iCount] == 13)
        {
            AllData[iCount] = '-';
            printf("\r\n");
            return AllData;
        }
        iCount++;
    } while (iCount != 32);

    return AllData;
}

