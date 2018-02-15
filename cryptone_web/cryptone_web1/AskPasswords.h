#pragma once
#include "stdafx.h"

/*
Function:
Ask master password for add new subclients for Username
The maximum size of the master password is 32 characters.
return:
NULL - FAILED
char* - Password String
*/
char* AskMasterPassword();

/*
Function:
Asks the password for the crypto container, it stores the settings and important variables
The maximum size of the crypto container password is 32 characters.

var:
char* Message - Addd text message for user

return:
NULL - FAILED
char* - Password String
*/
char* AskContainerPassword(char* Message);