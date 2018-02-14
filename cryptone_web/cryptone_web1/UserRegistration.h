#pragma once
#include "stdafx.h"

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
unsigned char* RegPack1(unsigned char* VectorRandom, unsigned char* PubKeyBuffer, int iFlag);

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
unsigned char* FirstHandshake(char* Servername, int iFlag);

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
int FirstHandshakeSaveData(unsigned char* AESvsUSER, unsigned char* strPwd, unsigned char** Vector, unsigned char** Key, unsigned char** ID);

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
int FinalRegHandshake(unsigned char* UserID, unsigned char* AESKey, unsigned char* AESVector, unsigned char*KeyPub, char* Servername, unsigned char* strPwd);

/*
Function:
User Registration function.
If the user is registered, you can add a new client to the existing user and call TempUserRegistration()

vars:
Servername - server name for registration

return:
1 if all Ok or 0 if Error
*/
/*
Function:
User Registration function.
If the user is registered, you can add a new client to the existing user and call TempUserRegistration()

vars:
Servername - server name for registration

return:
1 if all Ok or 0 if Error
*/
int NewUserRegistration(char* Servername);
