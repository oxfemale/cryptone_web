#include "stdafx.h"
#include "globalvars.h"
#include "AskUsername.h"

/*
Function:
Asks to enter the user name that will be used for registration in the system. 
This function can also be used when adding a new client to an already skilled user.
The maximum size of the user name is 32 characters.
return:
NULL - FAILED
char* - User name String
*/
char* AskUserNewName()
{
    char* AllData = NULL;
    AllData = (char*)VirtualAlloc(NULL, 32, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL) return AllData;
    memset(AllData, '-', 32);
    int iCount = 0;
    printf("Please input Username 32 symbols max: ");
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
            AllData[iCount] = 0;
            printf("\r\n");
            return AllData;
        }
        iCount++;
    } while (iCount < 32);

    return AllData;
}