#pragma once
#include "stdafx.h"

/*
Function:
Asks to enter the user name that will be used for registration in the system.
This function can also be used when adding a new client to an already skilled user.
The maximum size of the user name is 32 characters.
return:
NULL - FAILED
char* - User name String
*/
char* AskUserNewName();

