#include "stdafx.h"

/*
Description of function:
Pack some data date->->zip->hex()

input data of the function:
unsigned char* AllData - Any data

return:
NULL - is failed
char* if OK
*/
char* PackData(unsigned char* AllData);

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
unsigned char* PackClientPacket(unsigned char* ClientData, unsigned char* UserID, unsigned char* AESKey, unsigned char* AESVector, char* Command);

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
unsigned char* DecryptServerPacket(char* ServerAnswer, unsigned char* AESKey, unsigned char* AESVector);