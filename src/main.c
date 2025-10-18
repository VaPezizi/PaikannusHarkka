#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>
#include <unistd.h>
#include "ubxf.h"
#include "endians.h"
#include "netcode.h"
#include "fileRead.h"

//Ublox documentation
//https://content.u-blox.com/sites/default/files/documents/u-blox-20-HPG-2.00_InterfaceDescription_UBXDOC-304424225-19888.pdf

//Right one:
//https://content.u-blox.com/sites/default/files/documents/u-blox-F9-HPS-1.40_InterfaceDescription_UBXDOC-963802114-13138.pdf?utm_content=UBXDOC-963802114-13138

#define FILENAME "AJOMATKA.ubx"

/*
    CK_A = 0, CK_B = 0
    For (I = 0; I < N; I++)
    {
        CK_A = CK_A + Buffer[I]
        CK_B = CK_B + CK_A
    }*/


int main(int argc, char** argv){
    //printf("Morjestaa!\n");

    if(argc < 2){

        printf("Using default filename: %s\n", FILENAME);

        beginReadFile(FILENAME, sizeof(FILENAME));
    }
    else{
        if (strcmp(argv[1], "-h") == 0) { // Help menu
            printf("Usage:\n");
            printf("  -h                Show this help menu\n");
            printf("  -f <filename>     Read UBX data from the specified file\n");
            printf("  -s <serial_port>  Read UBX data from the specified serial port\n");
            printf("\n\n  Note: reading from serial port is not implemented for Windows OS!\n");
            return 0;
        } else if (strcmp(argv[1], "-f") == 0) { // File mode
            if (argc < 3) {
                printf("Please provide a filename after '-f' argument!\n");
                return 1;
            }
            printf("Reading from file: %s\n", argv[2]);
            beginReadFile(argv[2], strlen(argv[2]));
        } else if (strcmp(argv[1], "-s") == 0) { // Serial mode
            if (argc < 3) {
                printf("Please provide a serial port after '-s' argument!\n");
                return 1;
            }
            printf("Reading from serial port: %s\n", argv[2]);
            beginReadSerial(argv[2]);
        } else {
            printf("Unknown argument '%s'. Use '-h' for help.\n", argv[1]);
            return 1;
        }
     
    }

}
