#pragma once
#include "stdafx.h"

/*
Read any size data from http server
Each time it adds new data to the received data and returns it,
as well as returns the number of bytes received from the server.
And also returns the total number of output data
return:
NULL - FAILED
char* - All data including newly received from the network
DWORD &dwByteRead - The number of bytes received after the last request to the server
DWORD &dwAllByteRead - All bytes including newly received from the network
*/
char* readNetData(HINTERNET hRequest, char* AllData, DWORD packetSize, DWORD &dwByteRead, DWORD &dwAllByteRead);

/*
Function:
Send universal HTTP/HTTPS request Get/Post/Header

vars:
bool secureFlag - If set to 'true' a secure channel will be used HTTPS, if set to 'false'channel will be use HTTP
char* server - Server domain name
char* page - Page url, example: '/someurl/do.php'
char* params - Page params, example 'id=1&job=ABEF5103534666F5A4FDEF37F705B2CB7F0C4F'
int packetSize - The general parameter, not used, is adjusted inside the function automatically if the data is greater than 4048 bytes.
char* addHeader - Data that can be passed in the request header, example 'job: ABEF5103534666F5A4FDEF37F705B2CB7F0C4F'
char* postData - Data prepared for a POST query
return:
NULL - FAILED
char* - return Server answer text
*/
char* SendHTTPSRequest_GET(bool secureFlag, char* server, char* page, char* params, int packetSize, char* addHeader, char* postData);

/*
Function:
Interrogates the server for its survivability.
Sends a request with the text 'ping' and waits for a response 'pong'
If the response is received, the server is considered alive and it is possible to work with it further.

vars:
char* ServerName - Server domain name

return:
int 0 - FAILED
int 1 - OK
*/
int doPingServer(char* ServerName);

/*
Function:
The function to find the fastest and most secure way to transfer data on the protocole http/https
vars:
char* CheckServer - Server domain name

return:
int 0 - FAILED
int > 0 The fastest and most secure way to transfer data
*/
int SelectBestHttpTraffic(char* CheckServer);

/*
Function:
Sends data to the server, but automatically choosing the best data transfer option

vars:
char* ServerName - Server domain name
char* stringData - Data to be sent to the server

return:
NULL - FAILED
char* - Server answer
*/
char* SendPacketData(char* ServerName, char* stringData);

/*
Function:
Sends data to the http server using POST request

vars:
char* ServerName - Server domain name
char* stringData - Data to be sent to the server

return:
NULL - FAILED
char* - Server answer
*/
char* SendDataPOST(char* ServerName, char* stringData);

/*
Function:
Sends data to the https server using POST request

vars:
char* ServerName - Server domain name
char* stringData - Data to be sent to the server

return:
NULL - FAILED
char* - Server answer
*/
char* SendSSLDataPOST(char* ServerName, char* stringData);

/*
Function:
Sends data to the http server using GET request

vars:
char* ServerName - Server domain name
char* stringData - Data to be sent to the server

return:
NULL - FAILED
char* - Server answer
*/
char* SendDataGET(char* ServerName, char* stringData);

/*
Function:
Sends data to the https server using GET request

vars:
char* ServerName - Server domain name
char* stringData - Data to be sent to the server

return:
NULL - FAILED
char* - Server answer
*/
char* SendSSLDataGET(char* ServerName, char* stringData);

/*
Function:
Sends data to the https server using headers

vars:
char* ServerName - Server domain name
char* stringData - Data to be sent to the server

return:
NULL - FAILED
char* - Server answer
*/
char* SendSSLDataHEAD(char* ServerName, char* stringData);

/*
Function:
Sends data to the http server using headers

vars:
char* ServerName - Server domain name
char* stringData - Data to be sent to the server

return:
NULL - FAILED
char* - Server answer
*/
char* SendDataHEAD(char* ServerName, char* stringData);