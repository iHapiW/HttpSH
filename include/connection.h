#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>

#include <string.h>
#include <stdlib.h>

#include <windows.h>
#include <winhttp.h>

static HINTERNET hConnect;
static HINTERNET hSession;

void initConnection();
void sendData( char* data , char* curPath );
char* getData();
void closeSession();

#endif // CONNECTION_H
