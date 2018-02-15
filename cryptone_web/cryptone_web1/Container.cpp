#include "stdafx.h"
#include "globalvars.h"
#include "Container.h"
#include "console.h"

/*
Function:
Read global crypt keys and user info. (need to server connect)

vars:
strPwd - Container password

return:
int 0 - FAILED
int 1 if all Ok
*/
int SetKeysMem(unsigned char* strPwd)
{
	ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Set Keys to memory.", 0);
    if (strPwd == NULL)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error strPwd is null.", 1);
        return 0;
    }
    while (gUpdateKeys == 1)
    {
        Sleep(300);
    }
    gUpdateKeys = 1;

    if (gAESkey != NULL)
    {
        VirtualFree(gAESkey, 0, MEM_RELEASE);
        gAESkey = NULL;
    }
    if (gAESVector != NULL)
    {
        VirtualFree(gAESVector, 0, MEM_RELEASE);
        gAESVector = NULL;
    }
    if (gUseridhash != NULL)
    {
        VirtualFree(gUseridhash, 0, MEM_RELEASE);
        gUseridhash = NULL;
    }
    if (gUsername != NULL)
    {
        VirtualFree(gUsername, 0, MEM_RELEASE);
        gUsername = NULL;
    }
    if (gServerPassword != NULL)
    {
        VirtualFree(gServerPassword, 0, MEM_RELEASE);
        gServerPassword = NULL;
    }

    if (ReadContainer(strPwd, 5, &gAESkey, 1) == 0)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error read AESKey from container.", 1);
        gUpdateKeys = 0;
        return 0;
    }

    if (ReadContainer(strPwd, 8, &gAESVector, 1) == 0)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error read AESVector from container.", 1);
        VirtualFree(gAESkey, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 0;
    }
    if (ReadContainer(strPwd, 7, &gUseridhash, 1) == 0)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error read Client Userid hash from container.", 1);
        VirtualFree(gAESkey, 0, MEM_RELEASE);
        VirtualFree(gAESVector, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 0;
    }
    if (ReadContainer(strPwd, 6, &gUsername, 1) == 0)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error read Username from container.", 1);
        VirtualFree(gAESkey, 0, MEM_RELEASE);
        VirtualFree(gAESVector, 0, MEM_RELEASE);
        VirtualFree(gUseridhash, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 0;
    }
    if (ReadContainer(strPwd, 9, &gServerPassword, 1) == 0)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Error read Server Container Password from container.", 1);
        VirtualFree(gAESkey, 0, MEM_RELEASE);
        VirtualFree(gAESVector, 0, MEM_RELEASE);
        VirtualFree(gUseridhash, 0, MEM_RELEASE);
        VirtualFree(gUsername, 0, MEM_RELEASE);
        gUpdateKeys = 0;
        return 0;
    }
    gUpdateKeys = 0;
    return 1;
}

/*
Function:
Opens the encryption container and gets its contents in the decrypted form

vars:
unsigned char* key256 - The AES key for reading the data of the encrypted container

return:
NULL - FAILED
unsigned char* - contents in the decrypted form
*/
unsigned char* ReadAllContainer(unsigned char* key256)
{
    int iLen = 0;
    int decryptedLen = 0;
    int iSeek = 0;
    FILE* pFile = NULL;
    char* AllData = NULL;
    unsigned char *php_cipher = NULL;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    pFile = fopen("file.cfg", "rb");
    if (pFile == NULL) return 0;

    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    AllData = (char*)VirtualAlloc(NULL, lSize + 32, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        fclose(pFile);
        return 0;
    }

    iLen = fread(AllData, 1, lSize, pFile);
    if (iLen != lSize)
    {
        VirtualFree(AllData, 0, MEM_RELEASE);
        fclose(pFile);
        return 0;
    }
    fclose(pFile);

    php_cipher = aes256_decryptC((unsigned char*)AllData, iLen, key256, iv128, decryptedLen);
    VirtualFree(AllData, 0, MEM_RELEASE);
    if (php_cipher == NULL) return 0;

    return php_cipher;
}

/*
Function:
Opens the encrypted container and checks for variables:
1 - Private
2 - Public
3 - Cert

vars:
unsigned char* key256 - The AES key for reading the data of the encrypted container

return:
int 0 - FAILED
int 1 - OK
*/
int TestCfgVars(unsigned char* key256)
{
    unsigned char* testKey1 = NULL;
    unsigned char* testKey2 = NULL;
    unsigned char* testKey3 = NULL;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    if (ReadContainer(key256, 1, &testKey1, 0) == 0)
    {
        return 0;
    }
    VirtualFree(testKey1, 0, MEM_RELEASE);
    if (ReadContainer(key256, 2, &testKey2, 0) == 0)
    {
        return 0;
    }
    VirtualFree(testKey2, 0, MEM_RELEASE);
    if (ReadContainer(key256, 3, &testKey3, 0) == 0)
    {
        return 0;
    }
    VirtualFree(testKey3, 0, MEM_RELEASE);
    return 1;
}

/*
Function:
Opens the encryption container and checks for variables, user name and hash

vars:
unsigned char* key256 - The AES key for reading the data of the encrypted container

return:
int 0 - FAILED
int 1 - OK
*/
int isRegUser(unsigned char* key256)
{
    unsigned char* UsernameKey = NULL;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    if (ReadContainer(key256, 5, &UsernameKey, 0) == 0) return 0;
    VirtualFree(UsernameKey, 0, MEM_RELEASE);

    unsigned char* ClientId = NULL;
    if (ReadContainer(key256, 6, &ClientId, 0) == 0) return 0;
    VirtualFree(ClientId, 0, MEM_RELEASE);

    return 1;
}

/*
Function:
Checks for the configuration file
return:
0 - does not exist
1 - is present
*/
int IsContainer()
{
    FILE * pFile = NULL;
    
    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    pFile = fopen("file.cfg", "r");
    if (pFile != NULL)
    {
        fclose(pFile);
        return 1;
    }
    return 0;
}

/*
Function:
Creates RSA keys, writes their encrypted container with a password.
Private key, Public key, Certificate key.

vars:
unsigned char* key256 - The key to which the container will be encrypted, the maximum key size is 32 bytes.

return:
0 - FAILED
1 - SUCCESS
*/
int MakeKeystoContainer(unsigned char* key256)
{
    aes256_encryptC = (aes256_encryptCExp)GetProcAddress(hModuleCRYPT, "aes256_encryptC");
    if (aes256_encryptC == NULL) return 0;
    GenRSAKeysToMem = (GenRSAKeysToMemExp)GetProcAddress(hModuleCRYPT, "GenRSAKeysToMem");
    if (GenRSAKeysToMem == NULL) return 0;
    int iLen = 0;
    FILE* pFile = NULL;
    char* AllData = NULL;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    //Generate RSA keys and save it to memory
    unsigned char* PrivateKeyStr = NULL;
    unsigned char* PublicKeyStr = NULL;
    unsigned char* CertificateKeyStr = NULL;
    if (GenRSAKeysToMem(&PrivateKeyStr, &PublicKeyStr, &CertificateKeyStr) == false) return 0;
    iLen = strlen((char*)PrivateKeyStr) + strlen((char*)PublicKeyStr) + strlen((char*)CertificateKeyStr) + 32;
    AllData = (char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        VirtualFree(PrivateKeyStr, 0, MEM_RELEASE);
        VirtualFree(PublicKeyStr, 0, MEM_RELEASE);
        VirtualFree(CertificateKeyStr, 0, MEM_RELEASE);
        return 0;
    }
    _snprintf(AllData, (iLen - 4), "%s%s%s\r\n", PrivateKeyStr, PublicKeyStr, CertificateKeyStr);

    int dataLen = strlen(AllData);
    int cryptedLen = 0;
    int decryptedLen = 0;
    unsigned char *cpp_cipher = aes256_encryptC((unsigned char *)AllData, dataLen, key256, iv128, cryptedLen);
    if (cpp_cipher == NULL)
    {
        VirtualFree(PrivateKeyStr, 0, MEM_RELEASE);
        VirtualFree(PublicKeyStr, 0, MEM_RELEASE);
        VirtualFree(CertificateKeyStr, 0, MEM_RELEASE);
        VirtualFree(AllData, 0, MEM_RELEASE);
        return 0;
    }

    VirtualFree(PrivateKeyStr, 0, MEM_RELEASE);
    VirtualFree(PublicKeyStr, 0, MEM_RELEASE);
    VirtualFree(CertificateKeyStr, 0, MEM_RELEASE);
    VirtualFree(AllData, 0, MEM_RELEASE);

    pFile = fopen("file.cfg", "wb");
    if (pFile == NULL)
    {
        VirtualFree(cpp_cipher, 0, MEM_RELEASE);
        return 0;
    }
    fwrite(cpp_cipher, 1, cryptedLen, pFile);
    if (ferror(pFile))
    {
        fclose(pFile);
        VirtualFree(cpp_cipher, 0, MEM_RELEASE);
        return 0;
    }
    fclose(pFile);

    VirtualFree(cpp_cipher, 0, MEM_RELEASE);
    return 1;
}

/*
Function:
Opens the encryption container and returns the required variable from it

vars:
unsigned char* key256 - The AES key for reading the data of the encrypted container
iKeyType:
1 - Private
2 - Public
3 - Cert
4 - Server side Public
5 - AES key for traffic
6 - Symbol username (nickname)
7 - Client Userid hash
8 - AES vectro for traffic
9 - Container password
unsigned char **KeyBuffer - Test variable in which the result will be returned
int cleanFlag - 1 Trim the tags of the text you are searching for or 0 for text you are searching

return:
int - 0 FAILED, 1 OK
*/
int ReadContainer(unsigned char* key256, int iKeyType, unsigned char **KeyBuffer, int cleanFlag)
{
    aes256_decryptC = (aes256_decryptCExp)GetProcAddress(hModuleCRYPT, "aes256_decryptC");
    if (aes256_decryptC == NULL) return 0;
    unsigned char *php_cipher1 = NULL;
    int iLen = 0;
    int decryptedLen = 0;
    FILE* pFile = NULL;
    char* AllData = NULL;
    unsigned char *php_cipher = NULL;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    pFile = fopen("file.cfg", "rb");
    if (pFile == NULL) return 0;
    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    AllData = (char*)VirtualAlloc(NULL, lSize + 32, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        fclose(pFile);
        return 0;
    }
    iLen = fread(AllData, 1, lSize, pFile);
    if (iLen != lSize)
    {
        VirtualFree(AllData, 0, MEM_RELEASE);
        fclose(pFile);
    }
    php_cipher = aes256_decryptC((unsigned char*)AllData, iLen, key256, iv128, decryptedLen);
    VirtualFree(AllData, 0, MEM_RELEASE);
    fclose(pFile);
    if (php_cipher == NULL) return 0;
    if (cleanFlag == 777)
    {
        *KeyBuffer = php_cipher;
        return 1;
    }

    php_cipher1 = FoundCfgVar(php_cipher, decryptedLen, iKeyType, cleanFlag);
    VirtualFree(php_cipher, 0, MEM_RELEASE);
    if (php_cipher1 == NULL)
    {
        *KeyBuffer = 0;
        return 0;
    }
    *KeyBuffer = php_cipher1;

    return 1;

}

/*
Function:
Cut a piece of string from the text and return it

vars:
unsigned char* php_cipher - The text in which the search will be performed
int decryptedLen - Size of the text in which the search will be performed
int iKeyType:
1 - Private
2 - Public
3 - Cert
4 - Server side Public
5 - AES key for traffic
6 - Symbol username (nickname)
7 - Client Userid hash
8 - AES vectro for traffic
9 - Container password
int cleanFlag - Trim the tags of the text you are searching for or 0 for text you are searching

return:
NULL - FAILED
unsigned char* - text you are searching
*/
unsigned char* FoundCfgVar(unsigned char* php_cipher, int decryptedLen, int iKeyType, int cleanFlag)
{
    int iBegin = 0;
    int iEnd = 0;
    unsigned char *MyCfgVar = NULL;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    MyCfgVar = (unsigned char*)VirtualAlloc(NULL, decryptedLen + 32, MEM_COMMIT, PAGE_READWRITE);
    if (MyCfgVar == NULL) return 0;

    if (iKeyType == 1)
    {
        if (FindStartEndString(php_cipher, BeginPrivateKey, EndPrivateKey, iBegin, iEnd, cleanFlag) == 0)
        {
            VirtualFree(MyCfgVar, 0, MEM_RELEASE);
            return 0;
        }
        int iCpySize = iEnd - iBegin;
        memcpy_s(MyCfgVar, decryptedLen, &php_cipher[iBegin], iCpySize);
        return MyCfgVar;
    }

    if (iKeyType == 2)
    {
        iBegin = 0, iEnd = 0;
        if (FindStartEndString(php_cipher, BeginPublicKey, EndPublicKey, iBegin, iEnd, cleanFlag) == 0)
        {
            VirtualFree(MyCfgVar, 0, MEM_RELEASE);
            return 0;
        }
        int iCpySize = iEnd - iBegin;
        memcpy_s(MyCfgVar, decryptedLen, &php_cipher[iBegin], iCpySize);
        return MyCfgVar;
    }

    if (iKeyType == 3)
    {
        iBegin = 0, iEnd = 0;
        if (FindStartEndString(php_cipher, BeginCertificate, EndCertificate, iBegin, iEnd, cleanFlag) == 0)
        {
            VirtualFree(MyCfgVar, 0, MEM_RELEASE);
            return 0;
        }
        int iCpySize = iEnd - iBegin;
        memcpy_s(MyCfgVar, decryptedLen, &php_cipher[iBegin], iCpySize);
        return MyCfgVar;
    }

    if (iKeyType == 4)
    {
        iBegin = 0, iEnd = 0;
        if (FindStartEndString(php_cipher, BeginServerPublicKey, EndServerPublicKey, iBegin, iEnd, cleanFlag) == 0)
        {
            VirtualFree(MyCfgVar, 0, MEM_RELEASE);
            return 0;
        }
        int iCpySize = iEnd - iBegin;
        memcpy_s(MyCfgVar, decryptedLen, &php_cipher[iBegin], iCpySize);
        return MyCfgVar;
    }
    if (iKeyType == 5)
    {
        iBegin = 0, iEnd = 0;
        if (FindStartEndString(php_cipher, BeginAESKey, EndAESKey, iBegin, iEnd, cleanFlag) == 0)
        {
            VirtualFree(MyCfgVar, 0, MEM_RELEASE);
            return 0;
        }
        int iCpySize = iEnd - iBegin;
        memcpy_s(MyCfgVar, decryptedLen, &php_cipher[iBegin], iCpySize);
        return MyCfgVar;
    }
    if (iKeyType == 6)
    {
        iBegin = 0, iEnd = 0;
        if (FindStartEndString(php_cipher, BeginUsername, EndUsername, iBegin, iEnd, cleanFlag) == 0)
        {
            VirtualFree(MyCfgVar, 0, MEM_RELEASE);
            return 0;
        }
        int iCpySize = iEnd - iBegin;
        memcpy_s(MyCfgVar, decryptedLen, &php_cipher[iBegin], iCpySize);
        return MyCfgVar;
    }
    if (iKeyType == 7)
    {
        iBegin = 0, iEnd = 0;
        if (FindStartEndString(php_cipher, BeginClientId, EndClientId, iBegin, iEnd, cleanFlag) == 0)
        {
            VirtualFree(MyCfgVar, 0, MEM_RELEASE);
            return 0;
        }
        int iCpySize = iEnd - iBegin;
        memcpy_s(MyCfgVar, decryptedLen, &php_cipher[iBegin], iCpySize);
        return MyCfgVar;
    }
    if (iKeyType == 8)
    {
        iBegin = 0, iEnd = 0;
        if (FindStartEndString(php_cipher, BeginAESVector, EndAESVector, iBegin, iEnd, cleanFlag) == 0)
        {
            VirtualFree(MyCfgVar, 0, MEM_RELEASE);
            return 0;
        }
        int iCpySize = iEnd - iBegin;
        memcpy_s(MyCfgVar, decryptedLen, &php_cipher[iBegin], iCpySize);
        return MyCfgVar;
    }
    if (iKeyType == 9)
    {
        iBegin = 0, iEnd = 0;
        if (FindStartEndString(php_cipher, BeginContainerPass, EndContainerPass, iBegin, iEnd, cleanFlag) == 0)
        {
            VirtualFree(MyCfgVar, 0, MEM_RELEASE);
            return 0;
        }
        int iCpySize = iEnd - iBegin;
        memcpy_s(MyCfgVar, decryptedLen, &php_cipher[iBegin], iCpySize);
        return MyCfgVar;
    }
    return 0;
}

/*
function:
Search for a substring in the string + search for the position with which the substring begins
vars:
char *s - The text in which the search will be performed
char *c - Search string
int n - The number of the symbol from which to start the search

return:
int -1 if FAILED or int number of position with which the substring begins
*/
int pos(char *s, char *c, int n)
{
    int i, j;		// Счетчики для циклов
    int lenC, lenS;	// Длины строк

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

                    //Находим размеры строки исходника и искомого
    for (lenC = 0; c[lenC]; lenC++);
    for (lenS = 0; s[lenS]; lenS++);

    for (i = 0; i <= lenS - lenC; i++) // Пока есть возможность поиска
    {
        for (j = 0; s[i + j] == c[j]; j++); // Проверяем совпадение посимвольно
                                            // Если посимвольно совпадает по длине искомого
                                            // Вернем из функции номер ячейки, откуда начинается совпадение
                                            // Учитывать 0-терминатор  ( '\0' )
        if (j - lenC == 1 && i == lenS - lenC && !(n - 1)) return i;
        if (j == lenC)
            if (n - 1) n--;
            else return i;
    }
    //Иначе вернем -1 как результат отсутствия подстроки
    return -1;
}

/*
Function:
Getting start and end numbers part of a string in the text

vars:
unsigned char* php_cipher - The text in which the search will be performed
char* cBegin1 - The beginning of the search string
char* cEnd1 - End of the search string
int &iBegin - The number of the first character of the cBegin1 string
int &iEnd - The number of the first character of the cEnd1 string
int cleanFlag

return:
0 - FAILED
1 - OK
int &iBegin - The number of the first character of the cBegin1 string
int &iEnd - The number of the first character of the cEnd1 string
*/
int FindStartEndString(unsigned char* php_cipher, char* cBegin1, char* cEnd1, int &iBegin, int &iEnd, int cleanFlag)
{
    int i = 0, n = 0;
    int res = 0;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    for (i = 1; n != -1; i++)
    {
        if (res == 0)
        {
            n = pos((char*)php_cipher, cBegin1, i);
            if (n >= 0)
            {
                iBegin = n;
                res++;
            }
        }
        if (res == 1)
        {
            n = pos((char*)php_cipher, cEnd1, i);
            if (n >= 0)
            {
                int k = strlen(cEnd1);
                iEnd = n + k;
                res++;
                break;
            }
        }
    }
    if (cleanFlag == 0)
    {
        if (res == 2) return 1;
    }
    else {
        iBegin = iBegin + strlen(cBegin1);
        iEnd = iEnd - strlen(cEnd1);
        if (res == 2) return 1;
    }

    return 0;
}

/*
Function:
Cut the string with the tags from the text and return the processed text.
example: cut <a> and </a> tags - "<b>password</b><a>test</a><z>user</z>", return "<b>password</b><z>user</z>"
vars:
unsigned char* php_cipher - Original text
char* cBegin1 - Start tag
char* cEnd1 - End tag

return:
NULL - FAILED
unsigned char* - Returns text with no variable and no tags.
*/
unsigned char* CutOldCfgVar(unsigned char* php_cipher, char* cBegin1, char* cEnd1)
{
    int iBegin = 0;
    int iEnd = 0;
    int iPreStart = 0;
    unsigned char *MyCfgVar = NULL;
    int decryptedLen = strlen((char*)php_cipher);

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    MyCfgVar = (unsigned char*)VirtualAlloc(NULL, decryptedLen + 32, MEM_COMMIT, PAGE_READWRITE);
    if (MyCfgVar == NULL) return 0;
    if (FindStartEndString(php_cipher, cBegin1, cEnd1, iBegin, iEnd, 0) == 0)
    {
        VirtualFree(MyCfgVar, 0, MEM_RELEASE);
        return 0;
    }
    int iCpySize = iEnd - iBegin;
    if (iBegin == 0)
    {
        iCpySize = decryptedLen - iEnd;
        memcpy_s(MyCfgVar, decryptedLen, &php_cipher[iEnd], iCpySize);
        return MyCfgVar;
    }
    if (iEnd == decryptedLen)
    {
        iCpySize = (decryptedLen - iBegin) - 1;
        memcpy_s(MyCfgVar, decryptedLen, php_cipher, iCpySize);
        return MyCfgVar;
    }

    memcpy_s(MyCfgVar, decryptedLen, php_cipher, iBegin);
    iCpySize = decryptedLen - iEnd;
    memcpy_s(&MyCfgVar[iBegin], decryptedLen, &php_cipher[iEnd], iCpySize);

    return MyCfgVar;
}

/*
Function:
Updates the configuration variable between the tags in the encrypted container

vars:
unsigned char* key256 - The key to which the container will be encrypted
char* cBegin - Start tag string
char* cEnd - End tag string
unsigned char* newVar - The new configuration variable.

return:
int 0 - FAILED
int 1 - UPDATED
*/
int UpdateContainer(unsigned char* key256, char* cBegin, char* cEnd, unsigned char* newVar)
{
    aes256_decryptC = (aes256_decryptCExp)GetProcAddress(hModuleCRYPT, "aes256_decryptC");
    if (aes256_decryptC == NULL) return 0;
    aes256_encryptC = (aes256_encryptCExp)GetProcAddress(hModuleCRYPT, "aes256_encryptC");
    if (aes256_encryptC == NULL) return 0;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    unsigned char *php_cipher1 = NULL;
    int iLen = 0;
    int decryptedLen = 0;
    int cryptedLen = 0;
    int iSeek = 0;
    FILE* pFile = NULL;
    char* AllData = NULL;
    unsigned char *php_cipher = NULL;
    unsigned char *cpp_cipher = NULL;
    unsigned char *new_php_cipher = NULL;
    pFile = fopen("file.cfg", "rb");
    if (pFile == NULL) return 0;
    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);
    AllData = (char*)VirtualAlloc(NULL, lSize + 32, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        fclose(pFile);
        return 0;
    }
    iLen = fread(AllData, 1, lSize, pFile);
    if (iLen != lSize)
    {
        VirtualFree(AllData, 0, MEM_RELEASE);
        fclose(pFile);
        return 0;
    }
    fclose(pFile);

    php_cipher = aes256_decryptC((unsigned char*)AllData, iLen, key256, iv128, decryptedLen);
    VirtualFree(AllData, 0, MEM_RELEASE);
    if (php_cipher == NULL) return 0;

    int iBegin = 0, iEnd = 0;
    if (FindStartEndString(php_cipher, cBegin, cEnd, iBegin, iEnd, 0) == 1)
    {
        new_php_cipher = CutOldCfgVar(php_cipher, cBegin, cEnd);
        VirtualFree(php_cipher, 0, MEM_RELEASE);
        if (new_php_cipher == NULL)
        {
            return 0;
        }
        php_cipher = NULL;
        php_cipher = new_php_cipher;
    }

    iLen = strlen((char*)php_cipher) + strlen((char*)newVar) + 32;
    AllData = NULL;
    AllData = (char*)VirtualAlloc(NULL, iLen + 32, MEM_COMMIT, PAGE_READWRITE);
    if (AllData == NULL)
    {
        VirtualFree(php_cipher, 0, MEM_RELEASE);
        return 0;
    }
    //_snprintf(AllData, iLen+4, "%s%s", newVar, php_cipher );
    iSeek = strlen((char*)newVar);
    memcpy_s(AllData, iLen - 1, newVar, iSeek);
    memcpy_s(&AllData[iSeek], iLen - 1, php_cipher, strlen((char*)php_cipher));
    VirtualFree(php_cipher, 0, MEM_RELEASE);
    php_cipher = NULL;
    iSeek = strlen(AllData);
    cpp_cipher = aes256_encryptC((unsigned char *)AllData, iSeek, key256, iv128, cryptedLen);
    VirtualFree(AllData, 0, MEM_RELEASE);
    if (cpp_cipher == NULL) return 0;
    pFile = fopen("file.cfg", "wb");
    if (pFile == NULL)
    {
        VirtualFree(php_cipher, 0, MEM_RELEASE);
        return 0;
    }
    fwrite(cpp_cipher, 1, cryptedLen, pFile);
    if (ferror(pFile))
    {
        fclose(pFile);
        VirtualFree(cpp_cipher, 0, MEM_RELEASE);
        return 0;
    }
    VirtualFree(cpp_cipher, 0, MEM_RELEASE);
    fclose(pFile);

    return 1;
}
