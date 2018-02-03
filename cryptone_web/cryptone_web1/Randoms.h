#pragma once
#include "stdafx.h"

/*
Function:
A random number generator of a given size. Returns the received data in a hexadecimal format.

vars:
int RandSize - The size in bytes of the final line

return:
NULL - FAILED
unsigned char* - Returns a random number of specified sizes in the HEX format, example 'DEADBEEF1337'
*/
unsigned char* gen_random(int RandSize);

