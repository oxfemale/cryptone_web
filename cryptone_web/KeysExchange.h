#pragma once
#include "stdafx.h"
/*
Function:
The client sends an encrypted request to the server with new AES key/Vector + new Client Public key
Also, the server returns the new server public key.

vars:
strPwd - Container password

return:
int 0 - FAILED
int 1 if all Ok
(Also, the server returns a free space on the hard disk of the server.)
*/
int ClientServerKeysExchange(unsigned char* strPwd);