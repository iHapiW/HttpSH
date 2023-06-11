#ifndef CONNECTION_H
#define CONNECTION_H

#ifdef DEBUG
#include <stdio.h>
#endif

#include <string.h>

#include <windows.h>
#include <winhttp.h>

static HINTERNET hConnect;
static HINTERNET hSession;

void initConnection();
void sendData( char* data , char* curPath );
char* getData();
void closeSession();

char* urlencode(char* originalText);

#endif // CONNECTION_H
