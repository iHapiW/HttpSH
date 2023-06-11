#include "processMgmt.h"

#include "helper.h"

LPTSTR getCwd()
{
    LPTSTR cwd = NULL;
    DWORD size;
    while(TRUE)
    {
        size = GetCurrentDirectory(0, NULL);

        cwd = mallocBlock(size);

        DWORD written = GetCurrentDirectory(size, cwd);
        if(written + 1 != size)
        {
#ifdef DEBUG
            fprintf(stderr, "Cwd Size Not Equal to written!: %ld\n", GetLastError());
#endif
            free(cwd);
            Sleep(1000);
            continue;
        }
        break;
    }

    size_t slashCount = 0;
    for(size_t i = 0; i < size; ++i)
    {
        if(cwd[i] == '\\')
            slashCount++;
    }

    // Slash Escaped Current Working Directory
    char* secwd = mallocBlock(size+slashCount);
    
    size_t c = 0;
    for(size_t i = 0; i < size; ++i)
    {
        if(cwd[i] != '\\')
            secwd[c] = cwd[i];
        else
        {
            secwd[c] = cwd[i];
            secwd[c+1] = cwd[i];
            c++;
        }
        c++;
    }

    secwd[size+slashCount-1] = 0x00;

    return secwd;
}


void _spawnProcess(LPSTR cmd)
{
    STARTUPINFO supInfo;
    BOOL err = FALSE;

    ZeroMemory( &procInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory( &supInfo, sizeof(STARTUPINFO));
    supInfo.cb = sizeof(STARTUPINFO);
    supInfo.hStdError = procOutWr;
    supInfo.hStdOutput = procOutWr;
    supInfo.hStdInput = NULL;
    supInfo.dwFlags |= STARTF_USESTDHANDLES;

    size_t cmdLen = strlen(cmd);
    char* proc = mallocBlock(cmdLen + 12);
    memcpy(proc, "cmd.exe /c ", 11);
    memcpy(proc+11, cmd, cmdLen);
    proc[cmdLen + 12] = 0x00;

    while(TRUE)
    {
        err = CreateProcess(NULL, proc , NULL, NULL, TRUE, 0, NULL, NULL, &supInfo, &procInfo);
        if( err != TRUE)
        {
#ifdef DEBUG
            fprintf(stderr, "Couldn't Spawn Process: %ld\n", GetLastError());
#endif
            Sleep(1000);
            continue;
        }
        break;
    }

    free(proc);
    CloseHandle(procInfo.hThread);
}

LPSTR _rdPipe()
{

    while(TRUE)
    {
        DWORD result = WaitForSingleObject(procInfo.hProcess, INFINITE);
        if(result == WAIT_FAILED)
        {
#ifdef DEBUG
            fprintf(stderr, "Waiting for Process Failed: %ld\n", GetLastError());
#endif
            continue;
        }
        if( result == WAIT_TIMEOUT )
        {
#ifdef DEBUG
            puts("Wait Timed Out"); 
#endif
            return NULL;
        }
        break;
    }

    DWORD buffSize;
    DWORD err = FALSE;

    do {
        err = PeekNamedPipe(procOutRd, 0, 0, 0, &buffSize, 0);
#ifdef DEBUG
        if(err != TRUE)
            fprintf(stderr, "PeekNamedPipe Error: %ld\n", GetLastError());
#endif
    } while( err != TRUE );

    char* buffer = mallocBlock(buffSize+1);
    DWORD temp;
    do {
        err = PeekNamedPipe(procOutRd, buffer, buffSize+1, NULL, NULL, NULL);
#ifdef DEBUG
        if(err != TRUE)
            fprintf(stderr, "PeekNamedPipe Error: %ld\n", GetLastError());
#endif
    } while( err != TRUE );

    buffer[buffSize] = 0x00;
    
    return buffer;
}


LPSTR getOutput(LPSTR cmd)
{
    // Setup Attributes
    SECURITY_ATTRIBUTES attrs;
    attrs.nLength = sizeof(SECURITY_ATTRIBUTES);
    attrs.bInheritHandle = TRUE;
    attrs.lpSecurityDescriptor = NULL;

    BOOL err = FALSE;

    while(TRUE)
    {
        err = CreatePipe(&procOutRd, &procOutWr, &attrs, 0);
        if(err != TRUE)
        {
#ifdef DEBUG
            fprintf(stderr, "Create Pipe Error: %ld\n", GetLastError());
#endif
            Sleep(500);
            continue;
        }
        break;
    }

    _spawnProcess(cmd);
    LPSTR output = _rdPipe();

    CloseHandle(procOutRd);
    CloseHandle(procOutWr);

    procOutRd = NULL;
    procOutWr = NULL;

    return output;
}
