#include "stdafx.h"
#include "globalvars.h"
#include "Randoms.h"
#include "console.h"

/*
Function:
A random number generator of a given size. Returns the received data in a hexadecimal format.

vars:
int RandSize - The size in bytes of the final line

return:
NULL - FAILED
unsigned char* - Returns a random number of specified sizes in the HEX format, example 'DEADBEEF1337'
*/
unsigned char* gen_random(int RandSize)
{
    HCRYPTPROV    hCryptProv = 0;
    HCRYPTKEY    hOriginalKey = 0;
    DWORD        dwMode = 0;;
    BYTE*  pbData = NULL;
    unsigned char* Hexedstr = NULL;
    char* hexedFinal = NULL;
    if (RandSize <= 0) return NULL;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    pbData = (BYTE*)VirtualAlloc(NULL, RandSize + 4, MEM_COMMIT, PAGE_READWRITE);
    if (pbData == NULL) return 0;

    if (!CryptAcquireContextA(&hCryptProv, NULL, NULL, 1, 0))	return 0;
    if (!CryptGenKey(hCryptProv, CALG_RC4, 0, &hOriginalKey))
    {
        if (hCryptProv) CryptReleaseContext(hCryptProv, 0);
        return 0;
    }

    dwMode = CRYPT_MODE_ECB;
    if (!CryptSetKeyParam(hOriginalKey, KP_MODE, (BYTE*)&dwMode, 0))
    {
        if (hOriginalKey) CryptDestroyKey(hOriginalKey);
        if (hCryptProv) CryptReleaseContext(hCryptProv, 0);
        return 0;
    }
    if (!CryptGenRandom(hCryptProv, RandSize, (BYTE*)pbData))
    {
        if (hOriginalKey) CryptDestroyKey(hOriginalKey);
        if (hCryptProv) CryptReleaseContext(hCryptProv, 0);
        return 0;
    }

    if (hOriginalKey) CryptDestroyKey(hOriginalKey);
    if (hCryptProv) CryptReleaseContext(hCryptProv, 0);

    Hexedstr = (unsigned char*)VirtualAlloc(NULL, RandSize + 2, MEM_COMMIT, PAGE_READWRITE);
    if (Hexedstr == NULL)
    {
        VirtualFree(pbData, 0, MEM_RELEASE);
        return 0;
    }
    char2charHex = (char2charHexExp)GetProcAddress(hModuleCRYPT, "char2charHex");
    if (char2charHex == NULL)
    {
        VirtualFree(Hexedstr, 0, MEM_RELEASE);
        VirtualFree(pbData, 0, MEM_RELEASE);
        return 0;
    }
    hexedFinal = char2charHex((unsigned char*)pbData, RandSize);
    VirtualFree(pbData, 0, MEM_RELEASE);
    if (hexedFinal == NULL)
    {
        VirtualFree(Hexedstr, 0, MEM_RELEASE);
        return 0;
    }
    memcpy_s(Hexedstr, RandSize + 1, hexedFinal, RandSize);
    VirtualFree(hexedFinal, 0, MEM_RELEASE);


    return Hexedstr;

}
