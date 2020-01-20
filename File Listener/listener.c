/***************************************************************
                           listener.c
                             
A very basic program that loads an N64 ROM and polls for changes
(by looking at the timestamp). When a change is detected, the 
ROM is sent to the 64Drive with debug mode enabled. 

Designed to help with flow during the development of Platform64. 
Place the compiled program in the same directory as 
64drive_usb.exe and run this in the background. 

Program usage:
listener.exe romname.n64
***************************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include <windows.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    struct stat filestat;
    time_t oldtime;
    time_t curtime;
    
    // Check if a ROM was provided
    if (argc == 1)
    {
        printf("\nERROR: No input ROM detected!\n\n");
        printf("Program usage:\n");
        printf("%s romname.n64\n", argv[0]);
        return 1;
    }

    // Get file data
    if (stat(argv[1], &filestat))
    {
        printf("Unable to open ROM!\n");
        return 1;
    }

    // Poll the file until changes are made
    printf("Loaded %s into memory. Polling for changes...\n", argv[1]);
    oldtime = filestat.st_mtime;
    curtime = filestat.st_mtime;
    while (1)
    {      
        // If the file was changed
        if (curtime != oldtime)
        {
            char batch1[] = "64drive_usb.exe -l ";
            char batch2[] = " -c 6102 -z";
            char* command = (char*)malloc((sizeof(batch1)+sizeof(batch2)+sizeof(argv[1]))*sizeof(char));
            PROCESS_INFORMATION processInfo;
            STARTUPINFO info={sizeof(info)};
            
            // Set the command to run
            memset(command, 0, strlen(command));
            printf("File change detected! Sending ROM the the 64Drive.\n");
            strcat(command, batch1);
            strcat(command, argv[1]);
            strcat(command, batch2);
            
            // Execute the 64Drive usb program
            if (CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
            {
                WaitForSingleObject(processInfo.hProcess, INFINITE);
                CloseHandle(processInfo.hProcess);
                CloseHandle(processInfo.hThread);
            }
            oldtime = curtime;
            free(command);

            // Poll again
            printf("\nPolling for next file change.\n");
        }
        
        // Check if the file was changed
        stat(argv[1], &filestat);
        curtime = filestat.st_mtime;
        
        // If it wasn't, sleep for 3 seconds
        if (curtime == oldtime)
            sleep(3);
    }
}