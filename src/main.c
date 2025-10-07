#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>
#include <unistd.h>
#include "ubxf.h"
#include "endians.h"
#include "netcode.h"

//Ublox documentation
//https://content.u-blox.com/sites/default/files/documents/u-blox-20-HPG-2.00_InterfaceDescription_UBXDOC-304424225-19888.pdf

#define FILENAME "COM8___9600_231127_110129.ubx"

/*
    CK_A = 0, CK_B = 0
    For (I = 0; I < N; I++)
    {
        CK_A = CK_A + Buffer[I]
        CK_B = CK_B + CK_A
    }*/


int main(){
    printf("Morjestaa!\n");

    FILE* file = fopen(FILENAME, "rb");
    if(!file){
        puts("Error in reading file!");
        exit(1);
    }
    //uint8_t c[2];
    mittaus mittaukset[1000];
    int idx = 0;
    while(!feof(file)){
        //fseek(file, -2, SEEK_CUR);
        mittaus testiMittaus;
        //fread(&testiMittaus, 6, 1, file);

        //Parsing the data safely
        fread(&testiMittaus, 1, 1, file);
        if(testiMittaus.psync1 == 0xb5){	//Found first sync char
            fread(&testiMittaus.psync2, 1, 1, file);
            if(testiMittaus.psync2 == 0x62){
                fread(&testiMittaus.mclass, 4, 1, file);
                
                //testiMittaus.lenght = swapEndian16(testiMittaus.lenght);

                //Allocate memory and read into payload
                testiMittaus.payload = calloc(1, testiMittaus.lenght);
                fread(testiMittaus.payload, testiMittaus.lenght, 1, file);

                fread(&testiMittaus.CK_A, 1, 2, file);

                //mittaukset = realloc(mittaukset, sizeof(mittaukset) + sizeof(mittaus));
                mittaukset[idx++] = testiMittaus;
                printMittaus(&testiMittaus);

                uint8_t CHKA, CHKB;
                calculateChecksum(&testiMittaus, &CHKA, &CHKB);

                if(CHKA != testiMittaus.CK_A || CHKB != testiMittaus.CK_B){
                    fprintf(stderr, "ERROR IN CHECKSUM!\n");
                    sleep(1);
                }
                if(testiMittaus.mclass == 0x01 && testiMittaus.id == 0x14){
                    UBX_NAV_HPPOSLLH_load testiLoad;
                    memcpy(&testiLoad, testiMittaus.payload, testiMittaus.lenght);
                    sendMeasurement(&testiLoad);
                    puts("Sent measurement, sleeping for a second");
                    sleep(1); // Sleeping for a second to avoid flooding the backend
                }
		
            }
            
        }if(idx > 1000){
            //freeAll(mittaukset, idx);
            //exit(1);
            goto EndLoop;
        }
        //printMittaus(&testiMittaus);
    }
    /*
    memcpy(&testiMittaus, file, 4);
    memcpy(&testiMittaus + 4, file + 4, testiMittaus.lenght);
    memcpy(&testiMittaus + 4 + testiMittaus.lenght, file + 4 + testiMittaus.lenght, 16);
    */
    /*while(){

    }*/
EndLoop:
    fclose(file);
    for (int i = 0; i < idx;i++){
        //freeAndPrint(&mittaukset[i]);
        free(mittaukset[i].payload);
    }
        // printMittaus(&testiMittaus);
        // free(testiMittaus.payload.repeats);
        return 0;
}
