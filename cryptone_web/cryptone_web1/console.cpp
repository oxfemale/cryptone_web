#include "stdafx.h"
#include "globalvars.h"
#include "UserRegistration.h"
#include "Randoms.h"
#include "Container.h"
#include "AskUsername.h"
#include "AskPasswords.h"
#include "http.h"
#include "PacketFactory.h"
#include "AddNewClient.h"
#include "ClientFunctions.h"
#include "SystemInfo.h"
#include "console.h"

void miniLogo()
{
    std::cout << R"(
                             w                     
      .d8b 8d8b Yb  dP 88b. w8ww .d8b. 8d8b. .d88b 
      8    8P    YbdP  8  8  8   8' .8 8P Y8  8.dP' 
      `Y8P 8      dP   88P'  Y8P `Y8P' 8   8 `Y88P 
                 dP    8                           
)";
    return;
}

void LogoPrint()
{
    std::cout << R"(
                                           d8P                           
                                        d888888P                         
     d8888b  88bd88b?88   d8P ?88,.d88b,  ?88'   d8888b   88bd88b   d8888b
    d8P' `P  88P'  `d88   88  `?88'  ?88  88P   d8P' ?88  88P' ?8b d8b_,dP
    88b      d88    ?8(  d88   88b  d8P   88b   88b  d88  d88  88P 88b    
    `?888P'  d88   `?88P'?8b   888888P'  `?8b  `?8888P'   d88' 88b` ?888P'
                         )88   88P'                                     
                        ,d8P   d88                                       
                     ?888P'    ?8P                                            
)" << '\n';
    return;
}


void ConsoleOutput(char* cLogFileName, char* cLogFuncName, int iCodeLine, char* cInfo, int flag)
{
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
    if (flag == 4)
    {
        printf("%s", cInfo);
        return;
    }
    if (flag == 3)
    {
        if (gLogFile != NULL)
        {
            fprintf(gLogFile, "TIME: %d:%.2d:%.2d %.2d:%.2d:%.2d INFO %s->%s() line %d: %s\r\n", 1900 + timeinfo->tm_year, 1 + timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, cLogFileName, cLogFuncName, iCodeLine, cInfo);
        }
        return;
    }

	gotoxy(0, 1);
	clear_screen(0, 0);
	gotoxy(0, 0);
	if (flag == 0)
	{
		printf("INFO: %s", cInfo);
		if (gLogFile != NULL)
		{
			fprintf(gLogFile, "TIME: %d:%.2d:%.2d %.2d:%.2d:%.2d INFO %s->%s() line %d: %s\r\n", 1900 + timeinfo->tm_year, 1+timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, cLogFileName, cLogFuncName, iCodeLine, cInfo);
		}
	}
	if (flag == 1)
	{
		printf( "ERROR: %s", cInfo);
		if (gLogFile != NULL)
		{
			fprintf(gLogFile, "TIME: %d:%.2d:%.2d %.2d:%.2d:%.2d ERROR: %s->%s() line %d: %s\r\n", 1900 + timeinfo->tm_year, 1+timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, cLogFileName, cLogFuncName, iCodeLine, cInfo);
		}
	}
    
    
    gotoxy(0, 14);
    return;
}

void clear_screen(int x, int y)
{
    HANDLE hndl = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hndl, &csbi);
    DWORD written;
    DWORD n = csbi.dwSize.X * csbi.dwCursorPosition.Y + csbi.dwCursorPosition.X + 1;
    COORD curhome = { x,y };
    FillConsoleOutputCharacter(hndl, ' ', n, curhome, &written);
    csbi.srWindow.Bottom -= csbi.srWindow.Top;
    csbi.srWindow.Top = 0;
    SetConsoleWindowInfo(hndl, TRUE, &csbi.srWindow);
    SetConsoleCursorPosition(hndl, curhome);
}

void gotoxy(int x, int y)
{
    static HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    static COORD WritePos;
    WritePos.X = x;
    WritePos.Y = y;
    SetConsoleCursorPosition(hStdOut, WritePos);
}
