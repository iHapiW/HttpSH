#include <stdio.h>

#include "connection.h"

int main(int argc, char* argv[]) 
{
    initConnection();
    char* data = getData();
    printf("%s\n", data);
    free(data);
    return 0;
}
