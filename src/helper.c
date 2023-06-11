#include "helper.h"
#include <minwinbase.h>


void* mallocBlock(size_t size)
{
    void* buffer = NULL;
    do
    {
        buffer = malloc(size);
        if(buffer == NULL)
        {
#ifdef DEBUG
            fputs("Memory Allocation Error!", stderr);
#endif
            Sleep(50);
        }
    } while(buffer == NULL);
    
    return buffer;
}


void* callocBlock(size_t num, size_t size)
{
    void* buffer = NULL;
    do
    {
        buffer = calloc(num, size);
        if(buffer == NULL)
        {
#ifdef DEBUG
            fputs("Memory Allocation Error!", stderr);
#endif
            Sleep(50);
        }
    } while(buffer == NULL);

    return buffer;
}


void* reallocBlock(void* buffer, size_t newsize)
{
    void* temp = NULL;

    do {
        temp = realloc(buffer, newsize);
        if(temp == NULL)
        {
#ifdef DEBUG
            fputs("Memory Allocation Error!", stderr);
#endif
            Sleep(50);
        }
    } while(temp == NULL);

    return temp;
}

