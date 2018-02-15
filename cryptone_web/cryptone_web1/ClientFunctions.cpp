#include "stdafx.h"
#include "globalvars.h"
#include "UserRegistration.h"
#include "Randoms.h"
#include "Container.h"
#include "AskUsername.h"
#include "AskPasswords.h"
#include "http.h"
#include "PacketFactory.h"
#include "AddNewClient.h"
#include "ClientFunctions.h"
#include "SystemInfo.h"
#include "console.h"


/*
Function:
The function to get a online list of subclients for this user.
If user offline 180 sec - user is offline.

vars:

return:
int 0 - FAILED
int 1 if all Ok
(Also, the server returns a free space on the hard disk of the server.)
*/
int GetSubclientsListOnline()
{
    unsigned char* AllData = NULL;
    int iLen = 0;
    unsigned char* ClientPacket = NULL;
    char* ServerAnswer = NULL;
    unsigned char* DecryptedData = NULL;
    char* uList = "000";

    ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Start online userlist.", 0);

    while (gUpdateKeys == 1)
    {
        Sleep(300);
    }
    gUpdateKeys = 1;


    iLen = strlen((char*)gServerPassword) + strlen(uList) + 64;
    AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"VirtualAlloc error.", 1);
        gUpdateKeys = 0;
        return NULL;
    }

    //clear_data:SubCommand
    _snprintf((char*)AllData, (iLen - 4), "%s:%s:olist", (char*)gServerPassword, uList);

    ClientPacket = PackClientPacket(AllData, gUseridhash, gAESkey, gAESVector, "job");
    VirtualFree(AllData, 0, MEM_RELEASE);
    if (ClientPacket == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"PackC lient Packet error.", 1);
        gUpdateKeys = 0;
        return NULL;
    }

    //Send Packet to Server
    ServerAnswer = SendPacketData(gServername, (char*)ClientPacket);
    //printf("ClientPacket:[%s]\r\n", ClientPacket);
#ifdef _DEBUG
    gotoxy(0, 30);
    clear_screen(0, 20);
    gotoxy(0, 20);
    printf("answer:[%s]\r\n", ServerAnswer);
    gotoxy(0, 14);
#endif
    VirtualFree(ClientPacket, 0, MEM_RELEASE);
    if (ServerAnswer == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server return error.", 1);
        gUpdateKeys = 0;
        return 0;
    }

    DecryptedData = DecryptServerPacket(ServerAnswer, gAESkey, gAESVector);
    VirtualFree(ServerAnswer, 0, MEM_RELEASE);
    if (DecryptedData == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server answer aes256_decrypt error.", 1);
        gUpdateKeys = 0;
        return 0;
    }
    if (strstr((char*)DecryptedData, ":olist"))
    {
        //DecryptedData[strlen((char*)DecryptedData) - 6] = 0;
        gotoxy(0, 30);
        clear_screen(0, 20);
        gotoxy(0, 20);
        printf("Online clients %s\r\n", (char*)DecryptedData);
        gotoxy(0, 14);
        VirtualFree(DecryptedData, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 1;
    }
	VirtualFree(DecryptedData, 0, MEM_RELEASE);
    gUpdateKeys = 0;
#ifdef _DEBUG
    gotoxy(0, 30);
    clear_screen(0, 20);
    gotoxy(0, 20);
    printf("ServerAnswer: %s\r\n", (char*)DecryptedData);
    gotoxy(0, 14);
#endif
    return 0;
}

/*
Do Ping Server every 60 sec.
*/
DWORD WINAPI MainThreadPing(CONST LPVOID lpParam)
{
    //unsigned char* strPwd = (unsigned char*)lpParam;
    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);
    while (1)
    {
        if (ClientPingServer() == 0)
        {
            ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server ping ERROR.", 1);
        }
        Sleep(180000);
    }
    return 1;
}

/*
Function:
The client sends an encrypted request 'ping' to the server
And so it sends information about the client: Computer name, PC User name, last boot time, OS version and build version.
Also, the server returns a free space on the hard disk of the server.

vars:

return:
int 0 - FAILED
int 1 if all Ok
(Also, the server returns a free space on the hard disk of the server.)
*/
int ClientPingServer( )
{
    unsigned char* AllData = NULL;
    int iLen = 0;
    unsigned char* SystemInfo = NULL;
    unsigned char* ClientPacket = NULL;
    char* PackedData = NULL;
    char* ServerAnswer = NULL;
    unsigned char* DecryptedData = NULL;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    while (gUpdateKeys == 1)
    {
        Sleep(300);
    }
    gUpdateKeys = 1;

    if (gUseridhash == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"gUseridhash is null.", 1);
        return 0;
    }
    if (gServerPassword == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"gServerPassword is null.", 1);
        return 0;
    }
    if (gServername == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"gServername is null.", 1);
        return 0;
    }
    if (gAESkey == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"gAESkey is null.", 1);
        return 0;
    }
    if (gAESVector == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"gAESVector is null.", 1);
        return 0;
    }


    SystemInfo = GetSystemInfo();
    if (SystemInfo == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"get SystemInfo error.", 1);
        gUpdateKeys = 0;
        return NULL;
    }

    PackedData = PackData(SystemInfo);
    VirtualFree(SystemInfo, 0, MEM_RELEASE);
    if (PackedData == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"get PackedData error.", 1);
        gUpdateKeys = 0;
        return NULL;
    }

    iLen = strlen((char*)gServerPassword) + strlen(PackedData) + 64;
    AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"VirtualAlloc error.", 1);
        gUpdateKeys = 0;
        return NULL;
    }

    //printf("ServerContainerPassword: %s\r\n", ServerContainerPassword);
    //clear_data:SubCommand
    _snprintf((char*)AllData, (iLen - 4), "%s:%s:ping", (char*)gServerPassword, PackedData);
    VirtualFree(PackedData, 0, MEM_RELEASE);

    ClientPacket = PackClientPacket(AllData, gUseridhash, gAESkey, gAESVector, "job");
    VirtualFree(AllData, 0, MEM_RELEASE);

    if (ClientPacket == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Pack Client Packet error.", 1);
        gUpdateKeys = 0;
        return NULL;
    }


    //Send Packet to Server
    ServerAnswer = SendPacketData(gServername, (char*)ClientPacket);
    //printf("ClientPacket:[%s]\r\n", ClientPacket);
#ifdef _DEBUG
    gotoxy(0, 30);
    clear_screen(0, 20);
    gotoxy(0, 20);
    printf("answer:[%s]\r\n", ServerAnswer);
    gotoxy(0, 14);
#endif
    VirtualFree(ClientPacket, 0, MEM_RELEASE);
    if (ServerAnswer == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server return NULL.", 1);
        gUpdateKeys = 0;
        return 0;
    }

    DecryptedData = DecryptServerPacket(ServerAnswer, gAESkey, gAESVector);
    VirtualFree(ServerAnswer, 0, MEM_RELEASE);

    if (DecryptedData == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server answer aes256_decrypt error.", 1);
        gUpdateKeys = 0;
        return 0;
    }
    if (strstr((char*)DecryptedData, ":pong"))
    {
        DecryptedData[strlen((char*)DecryptedData) - 5] = 0;
        gotoxy(0, 1);
        clear_screen(0, 0);
        gotoxy(0, 0);
        printf("Ping Server, server disk space: %s", (char*)DecryptedData);
        gotoxy(0, 14);

        VirtualFree(DecryptedData, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 1;
    }
    VirtualFree(DecryptedData, 0, MEM_RELEASE);
    //printf("Decrypted data:\r\n%s\r\n", DecryptedData);
    gUpdateKeys = 0;
    return 0;
}


/*
Function:
The function to get a list of subclients for this user.

vars:

return:
int 0 - FAILED
int 1 if all Ok
(Also, the server returns a free space on the hard disk of the server.)
*/
int GetSubclientsList( )
{
    unsigned char* AllData = NULL;
    int iLen = 0;
    unsigned char* ClientPacket = NULL;
    char* ServerAnswer = NULL;
    unsigned char* DecryptedData = NULL;
    char* uList = "000";
    
	ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Start userlist.", 0);

    while (gUpdateKeys == 1)
    {
        Sleep(300);
    }
    gUpdateKeys = 1;


    iLen = strlen((char*)gServerPassword) + strlen(uList) + 64;
    AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"VirtualAlloc error.", 1);
        gUpdateKeys = 0;
        return NULL;
    }

    //clear_data:SubCommand
    _snprintf((char*)AllData, (iLen - 4), "%s:%s:ulist", (char*)gServerPassword, uList);

    ClientPacket = PackClientPacket(AllData, gUseridhash, gAESkey, gAESVector, "job");
    VirtualFree(AllData, 0, MEM_RELEASE);
    if (ClientPacket == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Pack Client Packet error.", 1);
        gUpdateKeys = 0;
        return NULL;
    }

    //Send Packet to Server
    ServerAnswer = SendPacketData(gServername, (char*)ClientPacket);
    //printf("ClientPacket:[%s]\r\n", ClientPacket);
#ifdef _DEBUG
    gotoxy(0, 30);
    clear_screen(0, 20);
    gotoxy(0, 20);
    printf("answer:[%s]\r\n", ServerAnswer);
    //gotoxy(0, 14);
#endif
    VirtualFree(ClientPacket, 0, MEM_RELEASE);
    if (ServerAnswer == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server return NULL.", 1);
        gUpdateKeys = 0;
        return 0;
    }

    DecryptedData = DecryptServerPacket(ServerAnswer, gAESkey, gAESVector);
    VirtualFree(ServerAnswer, 0, MEM_RELEASE);
    if (DecryptedData == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server answer aes256_decrypt error.", 1);
        gUpdateKeys = 0;
        return 0;
    }
    if (strstr((char*)DecryptedData, ":ulist"))
    {
        //DecryptedData[strlen((char*)DecryptedData) - 4] = 0;
        gotoxy(0, 30);
        clear_screen(0, 20);
        gotoxy(0, 20);
        printf("Subclients %s\r\n", (char*)DecryptedData);
        //gotoxy(0, 14);
        VirtualFree(DecryptedData, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 1;
    }
	VirtualFree(DecryptedData, 0, MEM_RELEASE);
    gUpdateKeys = 0;
#ifdef _DEBUG
    gotoxy(0, 30);
    clear_screen(0, 20);
    gotoxy(0, 20);
    printf("ServerAnswer: %s\r\n", (char*)DecryptedData);
    //gotoxy(0, 14);
#endif
    return 0;
}

/*
Function:
The function to set a alias for subclient.

vars:

return:
int 0 - FAILED
int 1 if all Ok
(Also, the server returns a free space on the hard disk of the server.)
*/
int SetSubclientsAlias()
{
    unsigned char* AllData = NULL;
    int iLen = 0;
    unsigned char* ClientPacket = NULL;
    char* ServerAnswer = NULL;
    unsigned char* DecryptedData = NULL;
    char* ClientAlias = NULL;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    while (gUpdateKeys == 1)
    {
        Sleep(300);
    }
    gUpdateKeys = 1;

    
    ClientAlias = (char*)VirtualAlloc(NULL, 32, MEM_COMMIT, PAGE_READWRITE);
    if (ClientAlias == NULL) return 0;
    memset(ClientAlias, 0, 32);
    int iCount = 0;

    gotoxy(0, 30);
    clear_screen(0, 14);
    gotoxy(0, 14);
    printf("Please input subclient alias 32 symbols max: ");
    do
    {
        ClientAlias[iCount] = _getch();
        printf("%c", ClientAlias[iCount]);
        if (ClientAlias[iCount] == 3)
        {
            printf("\r\nCancel and exit.\r\n");
            VirtualFree(ClientAlias, 0, MEM_RELEASE);
            return NULL;
        }
        if (ClientAlias[iCount] == 26)
        {
            printf("\r\nCancel and exit.\r\n");
            VirtualFree(ClientAlias, 0, MEM_RELEASE);
            return NULL;
        }
        if (ClientAlias[iCount] == 13)
        {
            ClientAlias[iCount] = 0;
            printf("\r\n");
            break;
        }
        iCount++;
    } while (iCount < 32);


    iLen = strlen((char*)gServerPassword) + strlen(ClientAlias) + 64;
    AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"VirtualAlloc error.", 1);
        gUpdateKeys = 0;
        return NULL;
    }

    //clear_data:SubCommand
    _snprintf((char*)AllData, (iLen - 4), "%s:%s:alias", (char*)gServerPassword, ClientAlias);
    VirtualFree(ClientAlias, 0, MEM_RELEASE);
    
    ClientPacket = PackClientPacket(AllData, gUseridhash, gAESkey, gAESVector, "job");
    VirtualFree(AllData, 0, MEM_RELEASE);
    if (ClientPacket == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Pack Client Packet error.", 1);
        gUpdateKeys = 0;
        return NULL;
    }

    //Send Packet to Server
    ServerAnswer = SendPacketData(gServername, (char*)ClientPacket);
    //printf("ClientPacket:[%s]\r\n", ClientPacket);
#ifdef _DEBUG
	gotoxy(0, 30);
	clear_screen(0, 20);
	gotoxy(0, 20);
    printf("answer:[%s]\r\n", ServerAnswer);
#endif
    VirtualFree(ClientPacket, 0, MEM_RELEASE);
    if (ServerAnswer == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server return error.", 1);
        gUpdateKeys = 0;
        return 0;
    }

    DecryptedData = DecryptServerPacket(ServerAnswer, gAESkey, gAESVector);
    VirtualFree(ServerAnswer, 0, MEM_RELEASE);
    if (DecryptedData == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server answer aes256_decrypt error.", 1);
        gUpdateKeys = 0;
        return 0;
    }
    if (strstr((char*)DecryptedData, ":alias"))
    {
        DecryptedData[strlen((char*)DecryptedData) - 6] = 0;
		gotoxy(0, 1);
		clear_screen(0, 0);
		gotoxy(0, 0);
        printf("New alias is set to: %s\r\n", (char*)DecryptedData);
		gotoxy(0, 14);
        VirtualFree(DecryptedData, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 1;
    }
	VirtualFree(DecryptedData, 0, MEM_RELEASE);
    gUpdateKeys = 0;
#ifdef _DEBUG
	gotoxy(0, 30);
	clear_screen(0, 20);
	gotoxy(0, 20);
    printf("ServerAnswer: %s\r\n", (char*)DecryptedData);
#endif
    return 0;
}
