#pragma once
#include "stdafx.h"

/*
Function:
The function to get a online list of subclients for this user.
If user offline 60 sec - user is offline.

vars:

return:
int 0 - FAILED
int 1 if all Ok
(Also, the server returns a free space on the hard disk of the server.)
*/
int GetSubclientsListOnline();

/*
Function:
The client sends an encrypted request 'ping' to the server
And so it sends information about the client: Computer name, PC User name, last boot time, OS version and build version.
Also, the server returns a free space on the hard disk of the server.

vars:

return:
int 0 - FAILED
int 1 if all Ok
(Also, the server returns a free space on the hard disk of the server.)
*/
int ClientPingServer( );

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

/*
Function:
The function to set a alias for subclient.

vars:

return:
int 0 - FAILED
int 1 if all Ok
(Also, the server returns a free space on the hard disk of the server.)
*/
int SetSubclientsAlias();

/*
Do Ping Server every 60 sec.
*/
DWORD WINAPI MainThreadPing(CONST LPVOID lpParam);
