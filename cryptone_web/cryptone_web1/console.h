#pragma once
#include "stdafx.h"

void clear_screen(int x, int y);

void gotoxy(int x, int y);

void ConsoleOutput(char* cLogFileName, char* cLogFuncName, int iCodeLine, char* cInfo, int flag);

void LogoPrint();

void miniLogo();