/***************************************************************
                             nrdc.c
                             
A very basic program that allows you to write the header of an
N64 ROM. The program can be run by dragging an N64 ROM onto it 
and filling in the information requested by the program, or the 
data can already be provided as extra arguments.

This registration step is necessary for certain emulators (like 
project64) to set the ROM's framerate according to whether it is
PAL or NTSC.

Program usage:
nrdc.exe romname.n64
or
nrdc.exe romname.n64 "ROMNAME" MANUFACTURER ID REGION
***************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char* argv[])
{
    // Create some temporary variables
    int i;
    FILE *ptr;
    char buffer[21];
    memset(buffer, 0, 21);
    
    // Check if we have the proper commands
    if (argc == 1)
    {
        printf("\nERROR: No input ROM detected!\n\n");
        printf("Program usage:\n");
        printf("%s romname.n64\n", argv[0]);
        printf("or\n", argv[0]);
        printf("%s romname.n64 \"ROMNAME\" MANUFACTURER ID REGION\n", argv[0]);
        return 1;
    }
    // Read the ROM.
    ptr = fopen(argv[1],"r+b");
    if (ptr == NULL)
    {
        printf("\nError opening file\n");
        return 1;
    }
    
    // Set the name
    fseek(ptr, 0x20, SEEK_SET);
    if (argc <= 2)
    {
        printf("Give your ROM a name (maximum of 20 characters): ");
        while (fgets(buffer, 21, stdin) == NULL)
            printf("You must provide a valid name for your ROM. Try again: ");
        for (i=0;i<21;i++)
            if (buffer[i] < ' ' || buffer[i] > '~')
                buffer[i] = ' ';
        fwrite(buffer, 1, 20, ptr);
        fseek(stdin,0,SEEK_END);
        printf("\n");
    }
    else
    {
        int size = strlen(argv[2]);
        if (size > 20)
        {
            printf("\nERROR: Your ROM name is too large! It must be under 20 characters\n");
            return 1;
        }
        fwrite(argv[2], 1, size, ptr);
    }
 
    // Set the manufacturer
    fseek(ptr, 0x07, SEEK_CUR);
    if (argc <= 3)
    {
        printf("Give your ROM a manufacturer. You can choose:\nN - Nintendo\n0 - None\nOr any other letter\nManufacturer: ");
        buffer[0] = getchar();
        toupper(buffer[0]);
        while (buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] < 'A' || buffer[0] > 'Z')
        {
            if (buffer[0] == '0')
                break;
            printf("You must provide a valid manufacturer for your ROM. Try again: ");
            fseek(stdin,0,SEEK_END);
            buffer[0] = getchar();
            toupper(buffer[0]);
        }
        fwrite(buffer, 1, 1, ptr);
        fseek(stdin,0,SEEK_END);
        printf("\n");
    }
    else
    {
        toupper(argv[3][0]);
        if (strlen(argv[3]) != 1 || argv[3][0] < 'A' || argv[3][0] > 'Z')
        {
            printf("\nERROR: The manufacturer should be a single character between A or Z!\n");
            return 1;
        }
        fwrite(argv[3], 1, 1, ptr);
    }
    
    // Set the ID
    if (argc <= 4)
    {
        printf("Give your ROM an ID (2 Letters): ");
        buffer[0] = getchar();
        buffer[1] = getchar();
        while (buffer[0] == '\n' || buffer[0] == '\r' || buffer[1] == '\n' || buffer[1] == '\r' || buffer[0] < 'A' || buffer[0] > 'Z' || buffer[1] < 'A' || buffer[1] > 'Z')
        {
            printf("You must provide a valid ID for your ROM. Try again: ");
            fseek(stdin,0,SEEK_END);
            buffer[0] = getchar();
            buffer[1] = getchar();
        }
        toupper(buffer[0]);
        toupper(buffer[1]);
        fwrite(buffer, 1, 2, ptr);
        fseek(stdin,0,SEEK_END);
        printf("\n");
    }
    else
    {
        toupper(argv[4][0]);
        toupper(argv[4][1]);
        if (strlen(argv[4]) != 2 || argv[4][0] < 'A' || argv[4][0] > 'Z' || argv[4][1] < 'A' || argv[4][1] > 'Z')
        {
            printf("\nERROR: The ROM ID should be two characters between A and Z!\n");
            return 1;
        }
        fwrite(argv[4], 1, 2, ptr);        
    }
    
    // Set the region
    if (argc <= 5)
    {
        printf("Finally, choose your ROM's region. Your options are:\nB - Brazil\nD - Germany\nE - USA\nF - France\nG - Italy\nH - Japan\nI - Europe\nJ - Spanish\nK - Australia\n0 - Unknown\nRegion: ");
        buffer[0] = getchar();
        toupper(buffer[0]);
        while (buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] < 'D' || buffer[0] > 'K')
        {
            if (buffer[0] == '0' || buffer[0] == 'B')
                break;
            printf("You must provide a valid region for your ROM. Try again: ");
            fseek(stdin,0,SEEK_END);
            buffer[0] = getchar();
            toupper(buffer[0]);
        }
        fwrite(buffer, 1, 1, ptr);
    }
    else
    {
        toupper(argv[5][0]);
        if (strlen(argv[5]) != 1 || argv[5][0] < 'D' || argv[5][0] > 'K')
        {
            if (argv[5][0] != 'B' && argv[5][0] != '0')
            {
                printf("\nERROR: The ROM region should be a single character between D and K, a 0, or a B!\n");
                return 1;
            }
        }
        fwrite(argv[5], 1, 1, ptr);          
    }
    
    // Finish
    printf("\nYour ROM has been sucessfully registered!\n");
    fclose(ptr);
    return 0;
}