#pragma once
#include "stdafx.h"

/*
Function:
Gets the name of the server from the file, if the file is not found, it sets the default server name.
And add server name to Global Var gServername.

vars:

return:
int 0 - FAILED
int 1 - OK
*/
int ServersList();

/*
Function:
Reading a vector from a file, if a file is not found, sets the default value.
And add AES Vector to Global Var iv128.

vars:

return:
int 0 - FAILED
int 1 - OK
*/
int SetDefaultAESVector();