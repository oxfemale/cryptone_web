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

/*
Function:
The client sends an encrypted request 'ping' to the server
And so it sends information about the client: Computer name, PC User name, last boot time, OS version and build version.
Also, the server returns a free space on the hard disk of the server.

vars:
Servername - server name for registration
strPwd - Container password

return:
int 0 - FAILED
int 1 if all Ok
(Also, the server returns a free space on the hard disk of the server.)
*/
int ClientPingServer(char* Servername, unsigned char* strPwd)
{
    unsigned char* AllData = NULL;
    int iLen = 0;
    unsigned char *AESKey = NULL;
    unsigned char *ClientUseridhash = NULL;
    unsigned char *AESVector = NULL;
    unsigned char *Username = NULL;
    unsigned char *ServerContainerPassword = NULL;
    unsigned char* SystemInfo = NULL;
    unsigned char* ClientPacket = NULL;
    char* PackedData = NULL;
    char* ServerAnswer = NULL;
    unsigned char* DecryptedData = NULL;

    if (strPwd == NULL)
    {
        printf("Error strPwd is null.\r\n");
        return 0;
    }
    if (Servername == NULL)
    {
        printf("Error Servername is null.\r\n");
        return 0;
    }
    while (gUpdateKeys == 1)
    {
        Sleep(300);
    }
    gUpdateKeys = 1;

    if (ReadContainer(strPwd, 5, &AESKey, 1) == 0)
    {
        printf("Error read AESKey from container.\r\n");
        gUpdateKeys = 0;
        return 0;
    }
    if (ReadContainer(strPwd, 8, &AESVector, 1) == 0)
    {
        printf("Error read AESVector from container.\r\n");
        VirtualFree(AESKey, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 0;
    }
    if (ReadContainer(strPwd, 7, &ClientUseridhash, 1) == 0)
    {
        printf("Error read Client Userid hash from container.\r\n");
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 0;
    }
    if (ReadContainer(strPwd, 6, &Username, 1) == 0)
    {
        printf("Error read Username from container.\r\n");
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(ClientUseridhash, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 0;
    }
    if (ReadContainer(strPwd, 9, &ServerContainerPassword, 1) == 0)
    {
        printf("Error read Server Container Password from container.\r\n");
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(ClientUseridhash, 0, MEM_RELEASE);
        VirtualFree(Username, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 0;
    }

    SystemInfo = GetSystemInfo();
    if (SystemInfo == NULL)
    {
        printf("Error: get SystemInfo error.\r\n");
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(ClientUseridhash, 0, MEM_RELEASE);
        VirtualFree(Username, 0, MEM_RELEASE);
        VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return NULL;
    }

    PackedData = PackData(SystemInfo);
    VirtualFree(SystemInfo, 0, MEM_RELEASE);
    if (PackedData == NULL)
    {
        printf("Error: get PackedData error.\r\n");
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(ClientUseridhash, 0, MEM_RELEASE);
        VirtualFree(Username, 0, MEM_RELEASE);
        VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return NULL;
    }

    iLen = strlen((char*)ServerContainerPassword) + strlen(PackedData) + 64;
    AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        printf("VirtualAlloc error.\r\n");
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(ClientUseridhash, 0, MEM_RELEASE);
        VirtualFree(Username, 0, MEM_RELEASE);
        VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return NULL;
    }

    printf("ServerContainerPassword: %s\r\n", ServerContainerPassword);
    //clear_data:SubCommand
    _snprintf((char*)AllData, (iLen - 4), "%s:%s:ping", (char*)ServerContainerPassword, PackedData);
    VirtualFree(PackedData, 0, MEM_RELEASE);
    VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);

    ClientPacket = PackClientPacket(AllData, ClientUseridhash, AESKey, AESVector, "job");
    VirtualFree(AllData, 0, MEM_RELEASE);
    VirtualFree(ClientUseridhash, 0, MEM_RELEASE);
    if (ClientPacket == NULL)
    {
        printf("PackClientPacket error.\r\n");
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(Username, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return NULL;
    }


    //Send Packet to Server
    ServerAnswer = SendPacketData(Servername, (char*)ClientPacket);
    //printf("ClientPacket:[%s]\r\n", ClientPacket);
    printf("answer:[%s]\r\n", ServerAnswer);
    VirtualFree(ClientPacket, 0, MEM_RELEASE);
    if (ServerAnswer == NULL)
    {
        printf("Server return error.\r\n");
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(Username, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 0;
    }

    DecryptedData = DecryptServerPacket(ServerAnswer, AESKey, AESVector);
    VirtualFree(ServerAnswer, 0, MEM_RELEASE);
    VirtualFree(AESKey, 0, MEM_RELEASE);
    VirtualFree(AESVector, 0, MEM_RELEASE);
    VirtualFree(Username, 0, MEM_RELEASE);
    if (DecryptedData == NULL)
    {
        printf("Server answer aes256_decrypt error.\r\n");
        gUpdateKeys = 0;
        return 0;
    }
    if (strstr((char*)DecryptedData, ":pong"))
    {
        DecryptedData[strlen((char*)DecryptedData) - 4] = 0;
        printf("Server Free Disk space: %s\r\n", (char*)DecryptedData);
        VirtualFree(DecryptedData, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 1;
    }
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
    
    printf("Start userlist.\r\n");

    while (gUpdateKeys == 1)
    {
        Sleep(300);
    }
    gUpdateKeys = 1;


    iLen = strlen((char*)gServerPassword) + strlen(uList) + 64;
    AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        printf("VirtualAlloc error.\r\n");
        gUpdateKeys = 0;
        return NULL;
    }

    //clear_data:SubCommand
    _snprintf((char*)AllData, (iLen - 4), "%s:%s:ulist", (char*)gServerPassword, uList);

    ClientPacket = PackClientPacket(AllData, gUseridhash, gAESkey, gAESVector, "job");
    VirtualFree(AllData, 0, MEM_RELEASE);
    if (ClientPacket == NULL)
    {
        printf("PackClientPacket error.\r\n");
        gUpdateKeys = 0;
        return NULL;
    }

    //Send Packet to Server
    ServerAnswer = SendPacketData(gServername, (char*)ClientPacket);
    //printf("ClientPacket:[%s]\r\n", ClientPacket);
    printf("answer:[%s]\r\n", ServerAnswer);
    VirtualFree(ClientPacket, 0, MEM_RELEASE);
    if (ServerAnswer == NULL)
    {
        printf("Server return error.\r\n");
        gUpdateKeys = 0;
        return 0;
    }

    DecryptedData = DecryptServerPacket(ServerAnswer, gAESkey, gAESVector);
    VirtualFree(ServerAnswer, 0, MEM_RELEASE);
    if (DecryptedData == NULL)
    {
        printf("Server answer aes256_decrypt error.\r\n");
        gUpdateKeys = 0;
        return 0;
    }
    if (strstr((char*)DecryptedData, ":ulist"))
    {
        DecryptedData[strlen((char*)DecryptedData) - 4] = 0;
        printf("Subclients list:\r\n(%s)\r\n", (char*)DecryptedData);
        VirtualFree(DecryptedData, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 1;
    }
    gUpdateKeys = 0;
    printf("ServerAnswer: %s\r\n", (char*)DecryptedData);
    return 0;
}
