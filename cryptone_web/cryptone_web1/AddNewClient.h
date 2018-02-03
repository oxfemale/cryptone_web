#pragma once
#include "stdafx.h"

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
int AddNewClientRegHandshake(unsigned char* UserID, unsigned char* AESKey, unsigned char* AESVector, unsigned char*KeyPub, char* Servername, unsigned char* strPwd);

/*
Function:
Adding a new client to an existing user.
Remeber, username is Global var!

vars:
Servername - server name for registration

return:
1 if all Ok or 0 if Error
*/
int AddNewClientToUser(char* Servername);