#pragma once
#include "stdafx.h"

/*
Function:
Opens the encryption container and gets its contents in the decrypted form

vars:
unsigned char* key256 - The AES key for reading the data of the encrypted container

return:
NULL - FAILED
unsigned char* - contents in the decrypted form
*/
unsigned char* ReadAllContainer(unsigned char* key256);

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
int TestCfgVars(unsigned char* key256);

/*
Function:
Opens the encryption container and checks for variables, user name and hash

vars:
unsigned char* key256 - The AES key for reading the data of the encrypted container

return:
int 0 - FAILED
int 1 - OK
*/
int isRegUser(unsigned char* key256);

/*
Function:
Checks for the configuration file
return:
0 - does not exist
1 - is present
*/
int IsContainer();

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
int MakeKeystoContainer(unsigned char* key256);

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
int ReadContainer(unsigned char* key256, int iKeyType, unsigned char **KeyBuffer, int cleanFlag);

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
unsigned char* FoundCfgVar(unsigned char* php_cipher, int decryptedLen, int iKeyType, int cleanFlag);

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
int pos(char *s, char *c, int n);

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
int FindStartEndString(unsigned char* php_cipher, char* cBegin1, char* cEnd1, int &iBegin, int &iEnd, int cleanFlag);

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
unsigned char* CutOldCfgVar(unsigned char* php_cipher, char* cBegin1, char* cEnd1);

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
int UpdateContainer(unsigned char* key256, char* cBegin, char* cEnd, unsigned char* newVar);
