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

    gAESkey = newAESKey;
    gAESVector = newAESVector;
    gClientPrivate = newClientPrivateKey;
    gClientPublic = newClientPublicKey;
    gClientCert = newClientCertKey;
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
        printf("newAESKey = gen_random error.\r\n");
        return NULL;
    }

    newAESVector = gen_random(16);
    if (newAESVector == NULL)
    {
        printf("newAESVector = gen_random error.\r\n");
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
        printf("Error strPwd is null.\r\n");
        return 0;
    }
    

    NewKeysAllData = GegNewKeys( &newAESKey, &newAESVector, &newClientPublicKey, &newClientPrivateKey, &newClientCertKey);
    if (NewKeysAllData == NULL)
    {
        printf("Error: get NewKeysAllData error.\r\n");
        return NULL;
    }

    PackedData = PackData(NewKeysAllData);
    VirtualFree(NewKeysAllData, 0, MEM_RELEASE);
    if (PackedData == NULL)
    {
        printf("Error: get PackedData error.\r\n");
        VirtualFree(newAESKey, 0, MEM_RELEASE);
        VirtualFree(newAESVector, 0, MEM_RELEASE);
        return NULL;
    }

    iLen = strlen((char*)gServerPassword) + strlen(PackedData) + 64;
    AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        printf("VirtualAlloc error.\r\n");
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
        printf("PackClientPacket error.\r\n");
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
    printf("answer:[%s]\r\n", ServerAnswer);
#endif
    VirtualFree(ClientPacket, 0, MEM_RELEASE);
    if (ServerAnswer == NULL)
    {
        printf("Server return error.\r\n");
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
        printf("Server answer aes256_decrypt error.\r\n");
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
        printf("Server Public key: %s\r\n", (char*)DecryptedData);
#endif
        while (UpdateCurrentKeys(strPwd, newAESKey, newAESVector, newClientPrivateKey, newClientPublicKey, newClientCertKey, DecryptedData) == 0)
        {
            Sleep(500);
            printf("Keys updated - FAILED, sleep and trye again.\r\n");
        }
        VirtualFree(newAESKey, 0, MEM_RELEASE);
        VirtualFree(newAESVector, 0, MEM_RELEASE);
        VirtualFree(newClientPrivateKey, 0, MEM_RELEASE);
        VirtualFree(newClientPublicKey, 0, MEM_RELEASE);
        VirtualFree(newClientCertKey, 0, MEM_RELEASE);
        VirtualFree(DecryptedData, 0, MEM_RELEASE);
        return 1;
    }
    VirtualFree(newAESKey, 0, MEM_RELEASE);
    VirtualFree(newAESVector, 0, MEM_RELEASE);
    VirtualFree(newClientPrivateKey, 0, MEM_RELEASE);
    VirtualFree(newClientPublicKey, 0, MEM_RELEASE);
    VirtualFree(newClientCertKey, 0, MEM_RELEASE);
    VirtualFree(DecryptedData, 0, MEM_RELEASE);
   printf("Keys updated - OK.\r\n");
   
    return 0;
}
