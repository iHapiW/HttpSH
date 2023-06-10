#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <string.h>

#include <windows.h>
#include <winhttp.h>

static HINTERNET hConnect;

void initConnection();
void sendData( char* data , char* curPath);
char* getData();

char* urlencode(char* originalText);

#endif // CONNECTION_H
