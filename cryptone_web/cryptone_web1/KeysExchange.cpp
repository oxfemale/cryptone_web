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
#include "KeysExchange.h"
#include "console.h"

/*
Do Ping KeysExchange every 10 min.
*/
DWORD WINAPI MainThreadKeysExchange(CONST LPVOID lpParam)
{
    unsigned char* strPwd = (unsigned char*)lpParam;
    while (1)
    {
        if (ClientServerKeysExchange(strPwd) == 0)
        {
			ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server ClientServerKeysExchange ERROR.", 1);
        }
        Sleep(600000);
    }
    return 1;
}

int UpdateCurrentKeys(unsigned char* strPwd, unsigned char* newAESKey, unsigned char* newAESVector, unsigned char* newClientPrivateKey, unsigned char* newClientPublicKey, unsigned char* newClientCertKey, unsigned char* newServerPublicKey)
{
    std::string aesk = "";
    while (gUpdateKeys == 1)
    {
        Sleep(1000);
    }
    gUpdateKeys = 1;

    aesk.append(BeginServerPublicKey, strlen(BeginServerPublicKey));
    aesk.append((char*)newServerPublicKey, strlen((const char*)newServerPublicKey));
    aesk.append(EndServerPublicKey, strlen(EndServerPublicKey));
    if (UpdateContainer(strPwd, BeginServerPublicKey, EndServerPublicKey, (unsigned char*)aesk.c_str()) == 0)
    {
        aesk.clear();
        gUpdateKeys = 0;
        return 0;
    }
    aesk.clear();

    aesk = "";
    aesk.append(BeginCertificate, strlen(BeginCertificate));
    aesk.append((char*)newClientCertKey, strlen((const char*)newClientCertKey));
    aesk.append(EndCertificate, strlen(EndCertificate));
    if (UpdateContainer(strPwd, BeginCertificate, EndCertificate, (unsigned char*)aesk.c_str()) == 0)
    {
        aesk.clear();
        gUpdateKeys = 0;
        return 0;
    }
    aesk.clear();

    aesk = "";
    aesk.append(BeginPublicKey, strlen(BeginPublicKey));
    aesk.append((char*)newClientPublicKey, strlen((const char*)newClientPublicKey));
    aesk.append(EndPublicKey, strlen(EndPublicKey));
    if (UpdateContainer(strPwd, BeginPublicKey, EndPublicKey, (unsigned char*)aesk.c_str()) == 0)
    {
        aesk.clear();
        gUpdateKeys = 0;
        return 0;
    }
    aesk.clear();
    
    aesk = "";
    aesk.append(BeginPrivateKey, strlen(BeginPrivateKey));
    aesk.append((char*)newClientPrivateKey, strlen((const char*)newClientPrivateKey));
    aesk.append(EndPrivateKey, strlen(EndPrivateKey));
    if (UpdateContainer(strPwd, BeginPrivateKey, EndPrivateKey, (unsigned char*)aesk.c_str()) == 0)
    {
        aesk.clear();
        gUpdateKeys = 0;
        return 0;
    }
    aesk.clear();

    aesk = "";
    aesk.append(BeginAESKey, strlen(BeginAESKey));
    aesk.append((char*)newAESKey, strlen((const char*)newAESKey));
    aesk.append(EndAESKey, strlen(EndAESKey));
    if (UpdateContainer(strPwd, BeginAESKey, EndAESKey, (unsigned char*)aesk.c_str()) == 0)
    {
        aesk.clear();
        gUpdateKeys = 0;
        return 0;
    }
    aesk.clear();

    aesk = "";
    aesk.append(BeginAESVector, strlen(BeginAESVector));
    aesk.append((char*)newAESVector, strlen((const char*)newAESVector));
    aesk.append(EndAESVector, strlen(EndAESVector));
    if (UpdateContainer(strPwd, BeginAESVector, EndAESVector, (unsigned char*)aesk.c_str()) == 0)
    {
        aesk.clear();
        gUpdateKeys = 0;
        return 0;
    }
    aesk.clear();

    if ( gAESkey != NULL )
    {
        VirtualFree(gAESkey, 0, MEM_RELEASE);
        gAESkey = NULL;
    }
    gAESkey = newAESKey;

    if (gAESVector != NULL)
    {
        VirtualFree(gAESVector, 0, MEM_RELEASE);
        gAESVector = NULL;
    }
    gAESVector = newAESVector;

    if (gClientPrivate != NULL)
    {
        VirtualFree(gClientPrivate, 0, MEM_RELEASE);
        gClientPrivate = NULL;
    }
    gClientPrivate = newClientPrivateKey;

    if (gClientPublic != NULL)
    {
        VirtualFree(gClientPublic, 0, MEM_RELEASE);
        gClientPublic = NULL;
    }
    gClientPublic = newClientPublicKey;

    if (gClientCert != NULL)
    {
        VirtualFree(gClientCert, 0, MEM_RELEASE);
        gClientCert = NULL;
    }
    gClientCert = newClientCertKey;

    if (gServerPublic != NULL)
    {
        VirtualFree(gServerPublic, 0, MEM_RELEASE);
        gServerPublic = NULL;
    }
    gServerPublic = newServerPublicKey;


    gUpdateKeys = 0;
    return 1;
}

unsigned char* GegNewKeys(unsigned char** newAESKey2, unsigned char** newAESVector2, unsigned char** newClientPublicKey2, unsigned char** newClientPrivateKey2, unsigned char** newClientCertKey2)
{
    int iLen = 0;
    unsigned char* AllData = NULL;
    unsigned char* newAESKey = NULL;
    unsigned char* newAESVector = NULL;
    unsigned char* newClientPublicKey = NULL;
    unsigned char* newClientPrivateKey = NULL;
    unsigned char* newClientCertKey = NULL;

    GenRSAKeysToMem = (GenRSAKeysToMemExp)GetProcAddress(hModuleCRYPT, "GenRSAKeysToMem");
    if (GenRSAKeysToMem == NULL) return 0;

    //Generate RSA keys and save it to memory
    if (GenRSAKeysToMem(&newClientPrivateKey, &newClientPublicKey, &newClientCertKey) == false) return NULL;

    newAESKey = gen_random(32);
    if (newAESKey == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"newAESKey = gen_random error.", 1);
        return NULL;
    }

    newAESVector = gen_random(16);
    if (newAESVector == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"newAESVector = gen_random error.", 1);
        VirtualFree(newAESKey, 0, MEM_RELEASE);
        return NULL;
    }

    iLen = strlen((char*)newAESKey) + strlen((char*)newAESVector) + strlen((char*)newClientPublicKey) + 32;
    AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        VirtualFree(newAESKey, 0, MEM_RELEASE);
        VirtualFree(newAESVector, 0, MEM_RELEASE);
        VirtualFree(newClientPrivateKey, 0, MEM_RELEASE);
        VirtualFree(newClientPublicKey, 0, MEM_RELEASE);
        VirtualFree(newClientCertKey, 0, MEM_RELEASE);
        return NULL;
    }
    _snprintf((char*)AllData, (iLen - 4), "%s:%s:%s", newAESKey, newAESVector, newClientPublicKey);

    *newAESKey2 = newAESKey;
    *newAESVector2 = newAESVector;
    *newClientPublicKey2 = newClientPublicKey;
    *newClientPrivateKey2 = newClientPrivateKey;
    *newClientCertKey2 = newClientCertKey;

    return AllData;
}

/*
Function:
The client sends an encrypted request to the server with new AES key/Vector + new Client Public key
Also, the server returns the new server public key.

vars:
strPwd - Container password

return:
int 0 - FAILED
int 1 if all Ok
(Also, the server returns a free space on the hard disk of the server.)
*/
int ClientServerKeysExchange(unsigned char* strPwd)
{
    unsigned char* AllData = NULL;
    int iLen = 0;
    unsigned char* NewKeysAllData = NULL;
    unsigned char* ClientPacket = NULL;
    unsigned char* newAESKey = NULL;
    unsigned char* newAESVector = NULL;
    unsigned char* newClientPublicKey = NULL;
    unsigned char* newClientPrivateKey = NULL;
    unsigned char* newClientCertKey = NULL;
    unsigned char* newServerPublicKey = NULL;
    char* PackedData = NULL;
    char* ServerAnswer = NULL;
    unsigned char* DecryptedData = NULL;

    if (strPwd == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error strPwd is null.", 1);
        return 0;
    }
    
    Sleep(10000);
    NewKeysAllData = GegNewKeys( &newAESKey, &newAESVector, &newClientPublicKey, &newClientPrivateKey, &newClientCertKey);
    if (NewKeysAllData == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"get NewKeysAllData error.", 1);
        return NULL;
    }

    PackedData = PackData(NewKeysAllData);
    VirtualFree(NewKeysAllData, 0, MEM_RELEASE);
    if (PackedData == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"get PackedData error.", 1);
        VirtualFree(newAESKey, 0, MEM_RELEASE);
        VirtualFree(newAESVector, 0, MEM_RELEASE);
        return NULL;
    }

    iLen = strlen((char*)gServerPassword) + strlen(PackedData) + 64;
    AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"VirtualAlloc error.", 1);
        VirtualFree(newAESKey, 0, MEM_RELEASE);
        VirtualFree(newAESVector, 0, MEM_RELEASE);
        VirtualFree(newClientPrivateKey, 0, MEM_RELEASE);
        VirtualFree(newClientPublicKey, 0, MEM_RELEASE);
        VirtualFree(newClientCertKey, 0, MEM_RELEASE);
        return NULL;
    }

    //clear_data:SubCommand
    _snprintf((char*)AllData, (iLen - 4), "%s:%s:keys", (char*)gServerPassword, PackedData);
    VirtualFree(PackedData, 0, MEM_RELEASE);

    ClientPacket = PackClientPacket(AllData, gUseridhash, gAESkey, gAESVector, "job");
    VirtualFree(AllData, 0, MEM_RELEASE);
    if (ClientPacket == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Pack Client Packet error.", 1);
        VirtualFree(newAESKey, 0, MEM_RELEASE);
        VirtualFree(newAESVector, 0, MEM_RELEASE);
        VirtualFree(newClientPrivateKey, 0, MEM_RELEASE);
        VirtualFree(newClientPublicKey, 0, MEM_RELEASE);
        VirtualFree(newClientCertKey, 0, MEM_RELEASE);
        return NULL;
    }


    //Send Packet to Server
    ServerAnswer = SendPacketData(gServername, (char*)ClientPacket);
    //printf("ClientPacket:[%s]\r\n", ClientPacket);
#ifdef _DEBUG
    gotoxy(0, 25);
    clear_screen(0, 16);
    gotoxy(0, 16);
    printf("answer:[%s]\r\n", ServerAnswer);
    gotoxy(0, 14);
#endif
    VirtualFree(ClientPacket, 0, MEM_RELEASE);
    if (ServerAnswer == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server answer is NULL.", 1);
        VirtualFree(newAESKey, 0, MEM_RELEASE);
        VirtualFree(newAESVector, 0, MEM_RELEASE);
        VirtualFree(newClientPrivateKey, 0, MEM_RELEASE);
        VirtualFree(newClientPublicKey, 0, MEM_RELEASE);
        VirtualFree(newClientCertKey, 0, MEM_RELEASE);
        return 0;
    }

    DecryptedData = DecryptServerPacket(ServerAnswer, gAESkey, gAESVector);
    VirtualFree(ServerAnswer, 0, MEM_RELEASE);
    if (DecryptedData == NULL)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server answer aes256_decrypt error.", 1);
        VirtualFree(newAESKey, 0, MEM_RELEASE);
        VirtualFree(newAESVector, 0, MEM_RELEASE);
        VirtualFree(newClientPrivateKey, 0, MEM_RELEASE);
        VirtualFree(newClientPublicKey, 0, MEM_RELEASE);
        VirtualFree(newClientCertKey, 0, MEM_RELEASE);
        return 0;
    }
    if (strstr((char*)DecryptedData, ":KeysExOK"))
    {
        iLen = strlen(":KeysExOK");
        DecryptedData[strlen((char*)DecryptedData) - iLen] = 0;
#ifdef _DEBUG
        gotoxy(0, 25);
        clear_screen(0, 16);
        gotoxy(0, 16);
        printf("Server Public key: %s\r\n", (char*)DecryptedData);
        gotoxy(0, 14);
#endif
        while (UpdateCurrentKeys(strPwd, newAESKey, newAESVector, newClientPrivateKey, newClientPublicKey, newClientCertKey, DecryptedData) == 0)
        {
            Sleep(500);
            ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Keys updated - FAILED, sleep and trye again.", 1);
        }
/*
        VirtualFree(newAESKey, 0, MEM_RELEASE);
        VirtualFree(newAESVector, 0, MEM_RELEASE);
        VirtualFree(newClientPrivateKey, 0, MEM_RELEASE);
        VirtualFree(newClientPublicKey, 0, MEM_RELEASE);
        VirtualFree(newClientCertKey, 0, MEM_RELEASE);
        VirtualFree(DecryptedData, 0, MEM_RELEASE);
 */   
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Keys updated - OK", 0);
        return 1;
    }
    /*
    VirtualFree(newAESKey, 0, MEM_RELEASE);
    VirtualFree(newAESVector, 0, MEM_RELEASE);
    VirtualFree(newClientPrivateKey, 0, MEM_RELEASE);
    VirtualFree(newClientPublicKey, 0, MEM_RELEASE);
    VirtualFree(newClientCertKey, 0, MEM_RELEASE);
    VirtualFree(DecryptedData, 0, MEM_RELEASE);
    */
   
    return 0;
}
