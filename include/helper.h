#ifndef HELPER_H
#define HELPER_H

#ifdef DEBUG
#include <stdio.h>
#endif

#include <stdlib.h>
#include <windows.h>

void* mallocBlock(size_t size);
void* callocBlock(size_t num, size_t size);
void* reallocBlock(void* buffer, size_t newsize);

#endif // HELPER_H
