#include "stdafx.h"
#include "globalvars.h"
#include "AskPasswords.h"

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
    printf("Please input [MASTER] password maximum 32 symbols: ");
    do
    {
        AllData[iCount] = _getch();
        printf("%c", AllData[iCount]);
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
return:
NULL - FAILED
char* - Password String
*/
char* AskContainerPassword()
{
    char* AllData = NULL;
    AllData = (char*)VirtualAlloc(NULL, 32, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL) return AllData;
    memset(AllData, '-', 32);
    int iCount = 0;
    printf("Please input container password maximum 32 symbols: ");
    do
    {
        AllData[iCount] = _getch();
        printf("%c", AllData[iCount]);
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
