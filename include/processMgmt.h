#ifndef PROCESSMGMT_H
#define PROCESSMGMT_H

#ifdef DEBUG
#include <stdio.h>
#endif

#include <stdlib.h>

#include <windows.h>

static HANDLE procOutRd = NULL;
static HANDLE procOutWr = NULL;
static PROCESS_INFORMATION procInfo;

LPTSTR getCwd();
LPSTR getOutput(LPSTR cmd);

#endif // PROCESSMGMT_H
