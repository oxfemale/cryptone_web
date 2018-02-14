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
#include "console.h"

/*
function:
(use only for add new client to exists user)
Temp user Client/Server final HandShake function exchange of crypto keys and public keys

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
int AddNewClientRegHandshake(unsigned char* UserID, unsigned char* AESKey, unsigned char* AESVector, unsigned char*KeyPub, char* Servername, unsigned char* strPwd)
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
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"AESKeyNew = gen_random error.", 1);
        return NULL;
    }

    AESVectorNew = gen_random(16);
    if (AESVectorNew == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"AESVectorNew = gen_random error.", 1);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        return NULL;
    }

    ServerContainerPassword = gen_random(32);
    if (ServerContainerPassword == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"ServerContainerPassword = gen_random error.", 1);
        VirtualFree(AESVectorNew, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        return NULL;
    }

    MasterPassword = AskMasterPassword();
    if (MasterPassword == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"AskMasterPassword error.", 1);
        VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
        VirtualFree(AESVectorNew, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        return NULL;
    }

    iLen = strlen((char*)ServerContainerPassword) + strlen((char*)AESKeyNew) + strlen((char*)AESVectorNew) + strlen((char*)KeyPub) + strlen(MasterPassword) + 64;
    AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"VirtualAlloc error.", 1);
        VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
        VirtualFree(AESVectorNew, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        return NULL;
    }

    //clear_data:SubCommand
    _snprintf((char*)AllData, (iLen - 4), "%s:%s:%s:%s:%s:end", MasterPassword, (char*)ServerContainerPassword, (char*)AESKeyNew, (char*)AESVectorNew, (char*)KeyPub);
    VirtualFree(MasterPassword, 0, MEM_RELEASE);

    unsigned char* ClientPacket = PackClientPacket(AllData, UserID, AESKey, AESVector, "tm2");
    VirtualFree(AllData, 0, MEM_RELEASE);

    if (ClientPacket == NULL)
    {
        VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
        VirtualFree(AESVectorNew, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
    }

    //Send Packet to Server
    ServerAnswer = SendPacketData(Servername, (char*)ClientPacket);
#ifdef _DEBUG
	gotoxy(0, 25);
	clear_screen(0, 16);
	gotoxy(0, 16);
    printf("ServerAnswer:\r\n%s\r\n", ServerAnswer);
	gotoxy(0, 14);
#endif
    VirtualFree(ClientPacket, 0, MEM_RELEASE);

    if (ServerAnswer == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server return NULL.", 1);
        VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
        VirtualFree(AESVectorNew, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        return 0;
    }

    DecryptedData = DecryptServerPacket(ServerAnswer, AESKeyNew, AESVectorNew);
    VirtualFree(ServerAnswer, 0, MEM_RELEASE);
    if (DecryptedData == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server answer aes256_decrypt error.", 1);
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
			ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"VirtualAlloc error.", 1);
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
#ifdef _DEBUG
		gotoxy(0, 25);
		clear_screen(0, 16);
		gotoxy(0, 16);
        printf("Server Public key: %s\r\n", ServerPublicKey );
		gotoxy(0, 14);
#endif

        std::string aesk = "";
        aesk.append(BeginServerPublicKey, strlen(BeginServerPublicKey));
        aesk.append((char*)ServerPublicKey, strlen((const char*)ServerPublicKey));
        aesk.append(EndServerPublicKey, strlen(EndServerPublicKey));
        if (UpdateContainer(strPwd, BeginServerPublicKey, EndServerPublicKey, (unsigned char*)aesk.c_str()) == 0)
        {
            aesk.clear();
			ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error update container, container deleted.", 1);
            remove("file.cfg");
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
			ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error update container, container deleted.", 1);
            remove("file.cfg");
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
			ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error update container, container deleted.", 1);
            remove("file.cfg");
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
			ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error update container, container deleted.", 1);
            remove("file.cfg");
            VirtualFree(ServerPublicKey, 0, MEM_RELEASE);
            VirtualFree(DecryptedData, 0, MEM_RELEASE);
            VirtualFree(ServerContainerPassword, 0, MEM_RELEASE);
            VirtualFree(AESVectorNew, 0, MEM_RELEASE);
            VirtualFree(AESKey, 0, MEM_RELEASE);
            return 0;
        }
        aesk.clear();

		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Registered, all data saved to container.", 0);

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
Adding a new client to an existing user.
Remeber, username is Global var!

vars:
Servername - server name for registration

return:
1 if all Ok or 0 if Error
*/
int AddNewClientToUser(char* Servername)
{
    unsigned char* strPwd = NULL;
    unsigned char* AESvsUSER = NULL;
    char iSelect[2] = { 0 };

	ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"TMP user registration.", 0);

    AESvsUSER = FirstHandshake(Servername, 1);
    if (AESvsUSER == NULL)
    {
        return 0;
    }

    //printf("Decrypted data: %s\r\n", AESvsUSER);
    strPwd = (unsigned char*)AskContainerPassword();
    if (strPwd == NULL)
    {
        VirtualFree(AESvsUSER, 0, MEM_RELEASE);
        return 0;
    }

    unsigned char *AESVector = NULL;
    unsigned char *AESKey = NULL;
    unsigned char *UserID = NULL;
    if (FirstHandshakeSaveData(AESvsUSER, strPwd, &AESVector, &AESKey, &UserID) == 0)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error parse/save handshake data.", 1);
        VirtualFree(strUsername, 0, MEM_RELEASE);
        VirtualFree(AESvsUSER, 0, MEM_RELEASE);
        return 0;
    }

    unsigned char *KeyPub = NULL;
    if (ReadContainer(strPwd, 2, &KeyPub, 0) == 0)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error read Public key from container.", 1);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(UserID, 0, MEM_RELEASE);
        VirtualFree(AESvsUSER, 0, MEM_RELEASE);
        return 0;
    }


    if (AddNewClientRegHandshake(UserID, AESKey, AESVector, KeyPub, Servername, strPwd) == 0)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Final registration Error, Saving step to container, please try late with your container password.", 1);
        std::string aesk = "";
        char* step1 = "---BIGIN FINAL ERROR----";
        char* step2 = "---END FINAL ERROR----";
        char* final = "Restep it please or delete container\n";
        aesk.append(step1, strlen(step1));
        aesk.append(final, strlen(final));
        aesk.append(step2, strlen(step2));
        if (UpdateContainer(strPwd, step1, step2, (unsigned char*)aesk.c_str()) == 0)
        {
			ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error safe final step registration, container deleted, please try register again.", 1);
            remove("file.cfg");
            aesk.clear();
            VirtualFree(KeyPub, 0, MEM_RELEASE);
            VirtualFree(AESKey, 0, MEM_RELEASE);
            VirtualFree(AESVector, 0, MEM_RELEASE);
            VirtualFree(UserID, 0, MEM_RELEASE);
            VirtualFree(AESvsUSER, 0, MEM_RELEASE);
            return 0;
        }
        aesk.clear();

        VirtualFree(KeyPub, 0, MEM_RELEASE);
        VirtualFree(AESKey, 0, MEM_RELEASE);
        VirtualFree(AESVector, 0, MEM_RELEASE);
        VirtualFree(UserID, 0, MEM_RELEASE);
        VirtualFree(AESvsUSER, 0, MEM_RELEASE);
        return 0;
    }

    VirtualFree(KeyPub, 0, MEM_RELEASE);
    VirtualFree(AESKey, 0, MEM_RELEASE);
    VirtualFree(AESVector, 0, MEM_RELEASE);
    VirtualFree(UserID, 0, MEM_RELEASE);
    VirtualFree(AESvsUSER, 0, MEM_RELEASE);

    return 1;
}