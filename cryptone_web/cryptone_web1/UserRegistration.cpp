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

/*
Function:
First packet message for registration on server.

vars:
unsigned char* VectorRandom - Random temp AES key for client/server exchange
unsigned char* PubKeyBuffer - Client Public key for server
int iFlag - 0 for normal registration, 1 - temp user registrator for add new client to user

return:
NULL - if FAILED
unsigned char* - Server answer text
*/
unsigned char* RegPack1(unsigned char* VectorRandom, unsigned char* PubKeyBuffer, int iFlag)
{
    int iDatSize = 0;
    unsigned char* AllData = NULL;
    unsigned char *cpp_cipher = NULL;
    char *Base64Data = NULL;
    char* ZippedChars = NULL;
    char* HexChars = NULL;
    int iTmpSize = 0;
    int iZiplen = 0;

    aes256_encryptC = (aes256_encryptCExp)GetProcAddress(hModuleCRYPT, "aes256_encryptC");
    if (aes256_encryptC == NULL) return 0;
    base64Encode = (base64EncodeExp)GetProcAddress(hModuleCRYPT, "base64Encode");
    if (base64Encode == NULL) return 0;
    compress_stringC = (compress_stringCExp)GetProcAddress(hModuleCRYPT, "compress_stringC");
    if (compress_stringC == NULL) return 0;
    char2charHex = (char2charHexExp)GetProcAddress(hModuleCRYPT, "char2charHex");
    if (char2charHex == NULL) return 0;

    iDatSize = strlen((char*)strUsername) + strlen((char*)PubKeyBuffer);
    iTmpSize = iDatSize + 256;

    AllData = (unsigned char*)VirtualAlloc(NULL, iTmpSize, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL) return NULL;

    iDatSize = iDatSize + 128;
    _snprintf((char*)AllData, iDatSize, "%s:%s\r\n", (char*)strUsername, (char*)PubKeyBuffer);
    iTmpSize = strlen((char*)AllData);

    cpp_cipher = aes256_encryptC(AllData, iTmpSize, VectorRandom, iv128, iDatSize);
    VirtualFree(AllData, 0, MEM_RELEASE);

    if (cpp_cipher == NULL) return NULL;

    size_t sLen = iDatSize;
    Base64Data = base64Encode(cpp_cipher, sLen);
    VirtualFree(cpp_cipher, 0, MEM_RELEASE);
    if (Base64Data == NULL)
    {
        VirtualFree(AllData, 0, MEM_RELEASE);
        return NULL;
    }
    iTmpSize = strlen(Base64Data) + strlen((char*)VectorRandom) + 32;
    AllData = (unsigned char*)VirtualAlloc(NULL, iTmpSize + 32, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        VirtualFree(Base64Data, 0, MEM_RELEASE);
        return NULL;
    }

    if (iFlag == 0)
    {
        _snprintf((char*)AllData, iTmpSize, "reg:%s:%s", (char*)VectorRandom, Base64Data);
    }
    else {
        _snprintf((char*)AllData, iTmpSize, "tm1:%s:%s", (char*)VectorRandom, Base64Data);
    }
    VirtualFree(Base64Data, 0, MEM_RELEASE);

    iTmpSize = strlen((char*)AllData);
    ZippedChars = compress_stringC((char*)AllData, iTmpSize, iZiplen);
    VirtualFree(AllData, 0, MEM_RELEASE);
    if (ZippedChars == NULL) return NULL;

    HexChars = char2charHex((unsigned char*)ZippedChars, iZiplen);
    VirtualFree(ZippedChars, 0, MEM_RELEASE);
    if (HexChars == NULL) return NULL;

    iTmpSize = iDatSize + strlen(HexChars) + 8;
    AllData = (unsigned char*)VirtualAlloc(NULL, iTmpSize, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        VirtualFree(HexChars, 0, MEM_RELEASE);
        return NULL;
    }
    _snprintf((char*)AllData, iTmpSize, "var=%s", HexChars);
    VirtualFree(HexChars, 0, MEM_RELEASE);

    return (unsigned char*)AllData;

    return NULL;
}

/*
Function:
Do First handshake and ask server for new user vars

vars:
char* Servername - Server domain name
int iFlag - 0 normal new user registration, 1 - temp user registration

return:
NULL - error
unsigned char* - Server answer
*/
unsigned char* FirstHandshake(char* Servername, int iFlag)
{
    size_t sLen = 0;
    unsigned char* VectorRandom = NULL;
    char* ServerRegData = NULL;
    unsigned char* debase64 = NULL;
    char* unZippedChars = NULL;
    unsigned char *CertKeyBuffer = NULL;
    unsigned char *PackedBuffer1 = NULL;

    base64Decode = (base64DecodeExp)GetProcAddress(hModuleCRYPT, "base64Decode");
    if (base64Decode == NULL) return 0;
    private_decrypt = (private_decryptExp)GetProcAddress(hModuleCRYPT, "private_decrypt");
    if (private_decrypt == NULL) return 0;
    decompress_stringC = (decompress_stringCExp)GetProcAddress(hModuleCRYPT, "decompress_stringC");
    if (decompress_stringC == NULL) return 0;

    VectorRandom = gen_random(32);
    if (VectorRandom == NULL) return 0;

    if (MakeKeystoContainer(VectorRandom) == 0)
    {
        printf("Error Create TMP container\r\n");
        VirtualFree(VectorRandom, 0, MEM_RELEASE);
        return 0;
    }
    printf("Create TMP container\r\n");


    if (ReadContainer(VectorRandom, 2, &CertKeyBuffer, 0) == 0)
    {
        printf("Error read Public key from container.\r\n");
        VirtualFree(VectorRandom, 0, MEM_RELEASE);
        return 0;
    }

    if (iFlag == 0)
    {
        strUsername = (unsigned char*)AskUserNewName();
    }

    if (strUsername == NULL)
    {
        printf("Error: Username is empty, must be global.\r\n");
        VirtualFree(VectorRandom, 0, MEM_RELEASE);
        VirtualFree(CertKeyBuffer, 0, MEM_RELEASE);
        return 0;
    }

    PackedBuffer1 = RegPack1(VectorRandom, CertKeyBuffer, iFlag);
    printf("ClientPubKey:\r\n%s\r\n", CertKeyBuffer);

    ServerRegData = SendPacketData(Servername, (char*)PackedBuffer1);

    printf("ServerRegData:\r\n%s\r\n", ServerRegData);

    VirtualFree(PackedBuffer1, 0, MEM_RELEASE);
    VirtualFree(CertKeyBuffer, 0, MEM_RELEASE);
    if (ServerRegData == NULL)
    {
        printf("Server return error.\r\n");
        VirtualFree(VectorRandom, 0, MEM_RELEASE);
        remove("file");
        return 0;
    }

    //printf("ServerRegData: [%s]\r\n[%d]\r\n", ServerRegData, strlen((char*)ServerRegData));	

    sLen = strlen(ServerRegData);
    int iLen = base64Decode((const char *)ServerRegData, sLen, &debase64);
    VirtualFree(ServerRegData, 0, MEM_RELEASE);
    if (iLen == 0)
    {
        printf("Error base64 decode answer.\r\n");
        VirtualFree(VectorRandom, 0, MEM_RELEASE);
        remove("file");
        return 0;
    }

    int unzippedLenTmp = 0;
    unZippedChars = decompress_stringC((char*)debase64, iLen, unzippedLenTmp);
    VirtualFree(debase64, 0, MEM_RELEASE);
    if (unZippedChars == NULL)
    {
        printf("Error decompress answer.\r\n");
        VirtualFree(VectorRandom, 0, MEM_RELEASE);
        remove("file");
        return 0;
    }

    unsigned char *PrivKey = 0;
    if (ReadContainer(VectorRandom, 1, &PrivKey, 0) == 0)
    {
        printf("Error read private key from conyainer.\r\n");
        VirtualFree(debase64, 0, MEM_RELEASE);
        VirtualFree(VectorRandom, 0, MEM_RELEASE);
        remove("file");
        return 0;
    }

    unsigned char* decrypteddata = (unsigned char*)VirtualAlloc(NULL, (unzippedLenTmp * 2) + 32, MEM_COMMIT, PAGE_READWRITE);
    if (decrypteddata == NULL)
    {
        printf("Error VirtualAlloc for decrypted data.\r\n");
        VirtualFree(VectorRandom, 0, MEM_RELEASE);
        VirtualFree(PrivKey, 0, MEM_RELEASE);
        VirtualFree(debase64, 0, MEM_RELEASE);
        remove("file");
        return 0;
    }

    int idecryptedlen = private_decrypt((unsigned char*)unZippedChars, unzippedLenTmp, PrivKey, decrypteddata);
    VirtualFree(PrivKey, 0, MEM_RELEASE);
    VirtualFree(debase64, 0, MEM_RELEASE);
    if (idecryptedlen <= 0)
    {
        printf("Error private decrypt answer.\r\n");
        VirtualFree(decrypteddata, 0, MEM_RELEASE);
        VirtualFree(VectorRandom, 0, MEM_RELEASE);
        remove("file");
        return 0;
    }
    VirtualFree(VectorRandom, 0, MEM_RELEASE);
    remove("file");

    return decrypteddata;
}

/*
function:
Parse first handshake from server and put it to vars, strPwd, Vector, Key, ID

vars:
unsigned char* AESvsUSER - Server answer string for parse
unsigned char* strPwd - Container password
unsigned char** Vector - By default is NULL and only return parsed AES Vector from server answer
unsigned char** Key - By default is NULL and only return parsed AES Key from server answer
unsigned char** ID - By default is NULL and only return parsed UserIdHash from server answer

return 0 if FAILED or 1 if OK
unsigned char** Vector - return parsed AES Vector from server answer
unsigned char** Key - return parsed AES Key from server answer
unsigned char** ID - return parsed UserIdHash from server answer

*/
int FirstHandshakeSaveData(unsigned char* AESvsUSER, unsigned char* strPwd, unsigned char** Vector, unsigned char** Key, unsigned char** ID)
{
    unsigned char* AESVector = NULL;
    unsigned char* AESKey = NULL;
    unsigned char* UserID = NULL;


    if (AESvsUSER == NULL) return 0;
    if (strUsername == NULL) return 0;

    int iLen = strlen((const char*)AESvsUSER);
    int iCount = 0;


    if (MakeKeystoContainer(strPwd) == 1)
    {
        printf("Container created.\r\n");
    }
    else {
        printf("Error created Container.\r\n");
        return 0;
    }

    //865e1ee87b60bb8025bc56b17d92d423:1ed0b0d2746fa2dafa9d71c89620297b:e12028285d370f41:okey
    AESVector = (unsigned char*)VirtualAlloc(NULL, 17, MEM_COMMIT, PAGE_READWRITE);
    if (AESVector == NULL) return 0;
    AESKey = (unsigned char*)VirtualAlloc(NULL, 33, MEM_COMMIT, PAGE_READWRITE);
    if (AESKey == NULL) return 0;
    UserID = (unsigned char*)VirtualAlloc(NULL, 33, MEM_COMMIT, PAGE_READWRITE);
    if (UserID == NULL) return 0;
    int iTmp = 0;

    while (iCount < iLen)
    {
        if (AESvsUSER[iCount] == ':')
        {
            iCount++;
            break;
        }
        UserID[iCount] = AESvsUSER[iCount];
        iCount++;
    }
    iTmp = 0;
    while (iCount < iLen)
    {
        if (AESvsUSER[iCount] == ':')
        {
            iCount++;
            break;
        }
        AESKey[iTmp] = AESvsUSER[iCount];
        iCount++;
        iTmp++;
    }
    iTmp = 0;
    while (iCount < iLen)
    {
        if (AESvsUSER[iCount] == ':')
        {
            iCount++;
            break;
        }
        AESVector[iTmp] = AESvsUSER[iCount];
        iCount++;
        iTmp++;
    }

    std::string aesk = "";
    aesk.append(BeginAESKey, strlen(BeginAESKey));
    aesk.append((char*)AESKey, strlen((const char*)AESKey));
    aesk.append(EndAESKey, strlen(EndAESKey));
    if (UpdateContainer(strPwd, BeginAESKey, EndAESKey, (unsigned char*)aesk.c_str()) == 0)
    {
        aesk.clear();
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(UserID, 0, MEM_RELEASE);
        return 0;
    }
    aesk.clear();

    aesk = "";
    aesk.append(BeginUsername, strlen(BeginUsername));
    aesk.append((char*)strUsername, strlen((const char*)strUsername));
    aesk.append(EndUsername, strlen(EndUsername));
    if (UpdateContainer(strPwd, BeginUsername, EndUsername, (unsigned char*)aesk.c_str()) == 0)
    {
        aesk.clear();
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(UserID, 0, MEM_RELEASE);
        return 0;
    }
    aesk.clear();

    aesk = "";
    aesk.append(BeginClientId, strlen(BeginClientId));
    aesk.append((char*)UserID, strlen((const char*)UserID));
    aesk.append(EndClientId, strlen(EndClientId));
    if (UpdateContainer(strPwd, BeginClientId, EndClientId, (unsigned char*)aesk.c_str()) == 0)
    {
        aesk.clear();
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(UserID, 0, MEM_RELEASE);
        return 0;
    }
    aesk.clear();

    aesk = "";
    aesk.append(BeginAESVector, strlen(BeginAESVector));
    aesk.append((char*)AESVector, strlen((const char*)AESVector));
    aesk.append(EndAESVector, strlen(EndAESVector));
    if (UpdateContainer(strPwd, BeginAESVector, EndAESVector, (unsigned char*)aesk.c_str()) == 0)
    {
        aesk.clear();
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(UserID, 0, MEM_RELEASE);
        return 0;
    }
    aesk.clear();


    *Vector = AESVector;
    *Key = AESKey;
    *ID = UserID;

    return 1;
}

/*
function:
Final Client/Server HandShake function exchange of crypto keys and public keys

input vars:
unsigned char* UserID - UseridHash
unsigned char* AESKey - AES key
unsigned char* AESVector - AES Vector
unsigned char* KeyPub - Public key
char* Servername - Server name
unsigned char* strPwd - Container password

return:
NULL - FAIL
1 - OK
*/
int FinalRegHandshake(unsigned char* UserID, unsigned char* AESKey, unsigned char* AESVector, unsigned char*KeyPub, char* Servername, unsigned char* strPwd)
{
    int iLen = 0, iTmp = 0, cryptedLen = 0, zippedLen = 0;
    if (UserID == NULL) return NULL;
    if (AESKey == NULL) return NULL;
    if (AESVector == NULL) return NULL;
    if (Servername == NULL) return NULL;

    unsigned char* AESKeyNew = NULL;
    unsigned char* AESVectorNew = NULL;
    unsigned char* ServerContainerPassword = NULL;
    unsigned char* AllData = NULL;
    unsigned char *cpp_cipher = NULL;
    char* base64str = NULL;
    char* ZippedChars = NULL;
    char* hexed = NULL;
    char* ServerAnswer = NULL;
    unsigned char *DecryptedData = NULL;
    char* unZippedChars = NULL;
    char* MasterPassword = NULL;

    AESKeyNew = gen_random(32);
    if (AESKeyNew == NULL)
    {
        printf("AESKeyNew = gen_random error.\r\n");
        return NULL;
    }

    AESVectorNew = gen_random(16);
    if (AESVectorNew == NULL)
    {
        printf("AESVectorNew = gen_random error.\r\n");
        VirtualFree(AESKey, 0, MEM_RELEASE);
        return NULL;
    }

    ServerContainerPassword = gen_random(32);
    if (ServerContainerPassword == NULL)
    {
        printf("ServerContainerPassword = gen_random error.\r\n");
        VirtualFree(AESVectorNew, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        return NULL;
    }

    MasterPassword = AskMasterPassword();
    if (MasterPassword == NULL)
    {
        printf("AskMasterPassword error.\r\n");
        VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
        VirtualFree(AESVectorNew, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        return NULL;
    }

    iLen = strlen((char*)ServerContainerPassword) + strlen((char*)AESKeyNew) + strlen((char*)AESVectorNew) + strlen((char*)KeyPub) + strlen(MasterPassword) + 64;
    AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        printf("VirtualAlloc error.\r\n");
        VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
        VirtualFree(AESVectorNew, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        return NULL;
    }

    //clear_data:SubCommand
    _snprintf((char*)AllData, (iLen - 4), "%s:%s:%s:%s:%s:end", MasterPassword, (char*)ServerContainerPassword, (char*)AESKeyNew, (char*)AESVectorNew, (char*)KeyPub);
    VirtualFree(MasterPassword, 0, MEM_RELEASE);

    unsigned char* ClientPacket = PackClientPacket(AllData, UserID, AESKey, AESVector, "fin");
    VirtualFree(AllData, 0, MEM_RELEASE);

    if (ClientPacket == NULL)
    {
        VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
        VirtualFree(AESVectorNew, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
    }

    //Send Packet to Server
    ServerAnswer = SendPacketData(Servername, (char*)ClientPacket);
	printf("ServerAnswer:\r\n%s\r\n", ServerAnswer);

    VirtualFree(ClientPacket, 0, MEM_RELEASE);

    if (ServerAnswer == NULL)
    {
        printf("Server return error.\r\n");
        VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
        VirtualFree(AESVectorNew, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        return 0;
    }

    DecryptedData = DecryptServerPacket(ServerAnswer, AESKeyNew, AESVectorNew);
    VirtualFree(ServerAnswer, 0, MEM_RELEASE);
    if (DecryptedData == NULL)
    {
        printf("Server answer aes256_decrypt error.\r\n");
        VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
        VirtualFree(AESVectorNew, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        return 0;
    }

    //printf( "Decrypted data:\r\n%s\r\n", DecryptedData );

    if (strstr((char*)DecryptedData, "registered"))
    {
        iLen = strlen((char*)DecryptedData) + 64;
        AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
        if (AllData == NULL)
        {
            printf("VirtualAlloc error.\r\n");
            VirtualFree(DecryptedData, 0, MEM_RELEASE);
            VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
            VirtualFree(AESVectorNew, 0, MEM_RELEASE);
            VirtualFree(AESKey, 0, MEM_RELEASE);
            return NULL;
        }

        iLen = strlen((char*)DecryptedData);
        int iCounter = 0;
        while (iCounter < iLen)
        {
            if (DecryptedData[iCounter] == ':') break;
            AllData[iCounter] = DecryptedData[iCounter];
            iCounter++;
        }
        AllData[iCounter] = 0;

        unsigned char* ServerPublicKey = AllData;

        //printf("Server Public key: %s\r\n", ServerPublicKey );

        std::string aesk = "";
        aesk.append(BeginServerPublicKey, strlen(BeginServerPublicKey));
        aesk.append((char*)ServerPublicKey, strlen((const char*)ServerPublicKey));
        aesk.append(EndServerPublicKey, strlen(EndServerPublicKey));
        if (UpdateContainer(strPwd, BeginServerPublicKey, EndServerPublicKey, (unsigned char*)aesk.c_str()) == 0)
        {
            aesk.clear();
            printf("Error update container, container deleted.\r\n");
            remove("file");
            VirtualFree(ServerPublicKey, 0, MEM_RELEASE);
            VirtualFree(DecryptedData, 0, MEM_RELEASE);
            VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
            VirtualFree(AESVectorNew, 0, MEM_RELEASE);
            VirtualFree(AESKey, 0, MEM_RELEASE);
            return 0;
        }
        aesk.clear();

        aesk = "";
        aesk.append(BeginContainerPass, strlen(BeginContainerPass));
        aesk.append((char*)ServerContainerPassword, strlen((const char*)ServerContainerPassword));
        aesk.append(EndContainerPass, strlen(EndContainerPass));
        if (UpdateContainer(strPwd, BeginContainerPass, EndContainerPass, (unsigned char*)aesk.c_str()) == 0)
        {
            aesk.clear();
            printf("Error update container, container deleted.\r\n");
            remove("file");
            VirtualFree(ServerPublicKey, 0, MEM_RELEASE);
            VirtualFree(DecryptedData, 0, MEM_RELEASE);
            VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
            VirtualFree(AESVectorNew, 0, MEM_RELEASE);
            VirtualFree(AESKey, 0, MEM_RELEASE);
            return 0;
        }

        aesk = "";
        aesk.append(BeginAESVector, strlen(BeginAESVector));
        aesk.append((char*)AESVectorNew, strlen((const char*)AESVectorNew));
        aesk.append(EndAESVector, strlen(EndAESVector));
        if (UpdateContainer(strPwd, BeginAESVector, EndAESVector, (unsigned char*)aesk.c_str()) == 0)
        {
            aesk.clear();
            printf("Error update container, container deleted.\r\n");
            remove("file");
            VirtualFree(ServerPublicKey, 0, MEM_RELEASE);
            VirtualFree(DecryptedData, 0, MEM_RELEASE);
            VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
            VirtualFree(AESVectorNew, 0, MEM_RELEASE);
            VirtualFree(AESKey, 0, MEM_RELEASE);
            return 0;
        }
        aesk.clear();

        aesk = "";
        aesk.append(BeginAESKey, strlen(BeginAESKey));
        aesk.append((char*)AESKeyNew, strlen((const char*)AESKeyNew));
        aesk.append(EndAESKey, strlen(EndAESKey));
        if (UpdateContainer(strPwd, BeginAESKey, EndAESKey, (unsigned char*)aesk.c_str()) == 0)
        {
            aesk.clear();
            printf("Error update container, container deleted.\r\n");
            remove("file");
            VirtualFree(ServerPublicKey, 0, MEM_RELEASE);
            VirtualFree(DecryptedData, 0, MEM_RELEASE);
            VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
            VirtualFree(AESVectorNew, 0, MEM_RELEASE);
            VirtualFree(AESKey, 0, MEM_RELEASE);
            return 0;
        }
        aesk.clear();

        printf("Registered, all data saved to container.\r\n");

        return 1;
    }

    //Save it to container before destroy.
    VirtualFree(DecryptedData, 0, MEM_RELEASE);
    VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
    VirtualFree(AESVectorNew, 0, MEM_RELEASE);
    VirtualFree(AESKey, 0, MEM_RELEASE);

    return NULL;
}

/*
Function:
User Registration function.
If the user is registered, you can add a new client to the existing user and call TempUserRegistration()

vars:
Servername - server name for registration

return:
1 if all Ok or 0 if Error
*/
int NewUserRegistration(char* Servername)
{
    unsigned char* strPwd = NULL;
    unsigned char* AESvsUSER = NULL;
    char iSelect[2] = { 0 };

    printf("Register NEW USER:\r\n");

    for (;;)
    {
        AESvsUSER = FirstHandshake(Servername, 0);
        if (AESvsUSER == NULL)
        {
            VirtualFree(strUsername, 0, MEM_RELEASE); //global var in FirstHandshake()
        }
        else
        {
            if (strstr((char*)AESvsUSER, "okey")) break;
            if (strstr((char*)AESvsUSER, "exists"))
            {
                printf("User already registered:\r\nSelect another NickName - 1\r\nAdd new SubClient for this NickName - 2\r\nselect: ");
                iSelect[0] = _getch();
                if (iSelect[0] == '1')
                {
                    printf("1\r\n");
                    VirtualFree(AESvsUSER, 0, MEM_RELEASE);
                    iSelect[0] = 0;
                    continue;
                }
                if (iSelect[0] == '2')
                {
                    if (AddNewClientToUser(Servername) == 0)
                    {
                        printf("Create TMP users error.\r\n");
                    }
                    else {
                        printf("Create TMP users OK.\r\n");
                        VirtualFree(AESvsUSER, 0, MEM_RELEASE);
                        return 1;
                    }
                    VirtualFree(AESvsUSER, 0, MEM_RELEASE);
                    return 0;
                }
            }
        }
        Sleep(100);
    }
    //printf("Decrypted data: %s\r\n", AESvsUSER);
    strPwd = (unsigned char*)AskContainerPassword();
    if (strPwd == NULL)
    {
        VirtualFree(strUsername, 0, MEM_RELEASE);
        VirtualFree(AESvsUSER, 0, MEM_RELEASE);
        return 0;
    }

    unsigned char *AESVector = NULL;
    unsigned char *AESKey = NULL;
    unsigned char *UserID = NULL;
    if (FirstHandshakeSaveData(AESvsUSER, strPwd, &AESVector, &AESKey, &UserID) == 0)
    {
        printf("Error parse/save handshake data\r\n");
        VirtualFree(strUsername, 0, MEM_RELEASE);
        VirtualFree(AESvsUSER, 0, MEM_RELEASE);
        return 0;
    }

    unsigned char *KeyPub = NULL;
    if (ReadContainer(strPwd, 2, &KeyPub, 0) == 0)
    {
        printf("Error read Public key from container.\r\n");
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(UserID, 0, MEM_RELEASE);
        VirtualFree(strUsername, 0, MEM_RELEASE);
        VirtualFree(AESvsUSER, 0, MEM_RELEASE);
        return 0;
    }


    if (FinalRegHandshake(UserID, AESKey, AESVector, KeyPub, Servername, strPwd) == 0)
    {
        printf("Final registration Error, Saving step to container, please try late with your container password.\r\n");
        std::string aesk = "";
        char* step1 = "---BIGIN FINAL ERROR----";
        char* step2 = "---END FINAL ERROR----";
        char* final = "Restep it please or delete container\n";
        aesk.append(step1, strlen(step1));
        aesk.append(final, strlen(final));
        aesk.append(step2, strlen(step2));
        if (UpdateContainer(strPwd, step1, step2, (unsigned char*)aesk.c_str()) == 0)
        {
            printf("Error safe final step registration, container deleted, please try register again.\r\n");
            remove("file");
            aesk.clear();
            VirtualFree(KeyPub, 0, MEM_RELEASE);
            VirtualFree(AESKey, 0, MEM_RELEASE);
            VirtualFree(AESVector, 0, MEM_RELEASE);
            VirtualFree(UserID, 0, MEM_RELEASE);
            VirtualFree(strUsername, 0, MEM_RELEASE);
            VirtualFree(AESvsUSER, 0, MEM_RELEASE);
            return 0;
        }
        aesk.clear();

        VirtualFree(KeyPub, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(UserID, 0, MEM_RELEASE);
        VirtualFree(strUsername, 0, MEM_RELEASE);
        VirtualFree(AESvsUSER, 0, MEM_RELEASE);
        return 0;
    }

    VirtualFree(KeyPub, 0, MEM_RELEASE);
    VirtualFree(AESKey, 0, MEM_RELEASE);
    VirtualFree(AESVector, 0, MEM_RELEASE);
    VirtualFree(UserID, 0, MEM_RELEASE);
    VirtualFree(strUsername, 0, MEM_RELEASE);
    VirtualFree(AESvsUSER, 0, MEM_RELEASE);

    return 1;
}