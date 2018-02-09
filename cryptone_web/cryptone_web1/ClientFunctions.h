#pragma once
#include "stdafx.h"

/*
Function:
The client sends an encrypted request 'ping' to the server
And so it sends information about the client: Computer name, PC User name, last boot time, OS version and build version.
Also, the server returns a free space on the hard disk of the server.

vars:
Servername - server name for registration
strPwd - Container password

return:
int 0 - FAILED
int 1 if all Ok
(Also, the server returns a free space on the hard disk of the server.)
*/
int ClientPingServer(char* Servername, unsigned char* strPwd);

/*
Function:
The function to get a list of subclients for this user.

vars:

return:
int 0 - FAILED
int 1 if all Ok
(Also, the server returns a free space on the hard disk of the server.)
*/
int GetSubclientsList();