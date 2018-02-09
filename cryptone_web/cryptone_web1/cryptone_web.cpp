/* 
twitter: @oxfemale
emails: root@eas7.ru or root@kitsune.online
telegram: https://t.me/BelousovaAlisa
*/
#include "stdafx.h"
#include "globalvars.h"
#include "cryptone_web.h"
#include "PacketFactory.h"
#include "SystemInfo.h"
#include "Container.h"
#include "AskPasswords.h"
#include "AskUsername.h"
#include "http.h"
#include "Randoms.h"
#include "UserRegistration.h"
#include "AddNewClient.h"
#include "ClientFunctions.h"
#include "ReadCfgFiles.h"
#include "KeysExchange.h"
#pragma comment (lib, "Wininet.lib")

int SetMenu()
{
    char iSelect[2] = {0};
    for (;;)
    {
                printf("Menu for user[%s]:\r\n1 - Get subclients list.\r\n2 - Set client alias.\r\n3 - Upload File.\r\nq - Exit from program.\r\nselect: ", gUsername);
                iSelect[0] = _getch();
                if (iSelect[0] == '1')
                {
                    printf("%c\r\n", iSelect[0]);
                    GetSubclientsList();
                    continue;
                }
                if (iSelect[0] == '2')
                {
                    printf("%c\r\n", iSelect[0]);
                    SetSubclientsAlias();
                    continue;
                }
                
                if ( iSelect[0] == 3 )
                {
                    printf("\r\nCancel and exit.\r\n");
                    return 0;
                }
                if (iSelect[0] == 26)
                {
                    printf("\r\nCancel and exit.\r\n");
                    return 0;
                }
                if (iSelect[0] == 13)
                {
                    printf("\r\nCancel and exit.\r\n");
                    return 0;
                }
                if (iSelect[0] == 'q')
                {
                    printf("%c\r\n", iSelect[0]);
                    printf("\r\nExit.\r\n");
                    return 0;
                }

                printf("\r\n");
    }
return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char* cryptone = "cryptone.dll";
	hModuleCRYPT = NULL;
	unsigned char* strPwd = NULL;
	FILE* pFile = NULL;
    char* Servername = NULL;

   

    hModuleCRYPT = LoadLibraryA(cryptone);
    if (hModuleCRYPT == NULL)
    {
        printf("Error[%d] load [%s] dll\r\n", GetLastError(), cryptone);
        return 0;
    }

    if (SetDefaultAESVector() == 0)
    {
        printf("Error set default aes vector.\r\n");
        return 0;
    }

    if (ServersList() == 0)
    {
        printf("Error set servername.\r\n");
        return 0;
    }
    Servername = gServername;

    if( doPingServer(Servername) == 1 )
	{
		printf("Server is Alive.\r\n");
		bestType = SelectBestHttpTraffic(Servername);
	}else
	{
		printf("Server is Down.\r\n");
		return 0;
	}


	pFile = fopen("file.cfg", "rb");
	if(pFile != NULL)
	{
		fclose(pFile);
		FILE* pasFile = NULL;
		pasFile = fopen("pass.cfg", "rb");
		if(pasFile == NULL)
		{
			strPwd = (unsigned char*)AskContainerPassword( );
			if( strPwd == NULL ) return 0;
		}else{
			strPwd =  (unsigned char*)VirtualAlloc( NULL, 32, MEM_COMMIT, PAGE_READWRITE );
			if( strPwd == NULL )
			{
				fclose(pFile);
				fclose(pasFile);
				printf("Error VirtualAlloc for password buffer.\r\n");
				return 0;
			}
			memset( strPwd, '-', 32 );
			fread( strPwd, 1, 32, pasFile);
			fclose(pasFile);
		}

		//printf( "\r\nPassword is: \r\n%s\r\n", strPwd );
		if( IsContainer() == 1 )
		{
			if( TestCfgVars( strPwd ) == 0 )
			{
				printf("Error test Container vars.\r\n");
				remove( "file.cfg" );
				printf("Container keys is deleted, please, goto login/register again.\r\n");
				return 0;
			}
		
			if ( isRegUser( strPwd ) == 0 )
			{
				printf("Not reggged user.\r\n");
				remove( "file.cfg" );
				printf("Container keys is deleted, please, goto login/register again.\r\n");
				return 0;
			}else{
				printf("Reggged user.\r\n");
				
				if ( ClientPingServer( Servername, strPwd ) == 0 )
				{
					printf("Server ping ERROR.\r\n");
					return 0;
				}
				printf("Server ping OK.\r\n");
                SetKeysMem(strPwd);
                ClientServerKeysExchange(strPwd);
                SetKeysMem(strPwd);
                SetMenu();
				return 1;
			}
		}else{
			//remove( "file.cfg" );
			printf("Error: Container password is wrong or container file is bad.\r\nDelete file.cfg if not remebers container password.\r\n");
			return 0;
		}
	}else
	{
		if ( NewUserRegistration( Servername ) == 0 )
		{
			printf("Registration error.\r\n");
            		remove("file.cfg");
			return 0;
		}else
		{
			printf("Registration Ok\r\nCrypt and close program container.\r\n");
            strPwd = (unsigned char*)AskContainerPassword();
            if (strPwd == NULL) return 0;
            ClientPingServer(Servername, strPwd);
            SetKeysMem(strPwd);
            ClientServerKeysExchange(strPwd);
            SetKeysMem(strPwd);
            SetMenu();

			return 1;
		}
		VirtualFree( strPwd, 0, MEM_RELEASE );
	}


	return 0;
}


