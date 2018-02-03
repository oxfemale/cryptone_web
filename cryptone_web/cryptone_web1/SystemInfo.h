#pragma once
#include "stdafx.h"


typedef LONG(WINAPI *pNtQuerySystemInformation)(UINT, PVOID, ULONG, PULONG);
typedef struct _SYSTEM_TIME_OF_DAY_INFORMATION
{
	LARGE_INTEGER BootTime;
	LARGE_INTEGER CurrentTime;
	LARGE_INTEGER TimeZoneBias;
	ULONG CurrentTimeZoneId;
} SYSTEM_TIME_OF_DAY_INFORMATION, *PSYSTEM_TIME_OF_DAY_INFORMATION;

BOOL UpTime(SYSTEMTIME *pstBootTime);

/*
Get System Info

return:
NULL - failed
unsigned char* - return system info
*/
unsigned char* GetSystemInfo();