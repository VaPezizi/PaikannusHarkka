#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>
#include "endians.h"

//Ublox documentation
//https://content.u-blox.com/sites/default/files/u-blox%20M10-SPG-5.00_InterfaceDescription_UBX-20053845.pdf?utm_content=UBX-20053845

#define FILENAME "COM8___9600_231127_110129.ubx"



#pragma pack(1)
//Repeated groups in UBX message end
typedef struct{
    uint16_t firstValue;
    uint16_t secondValue;
} repeatGroup;

//Struct for a payload of UBX message
typedef struct
{
    uint32_t aField;
    uint32_t anotherField;
    uint16_t bitfield;
    uint8_t reserved0[5];
    uint8_t numRepeat;
    repeatGroup * repeats;
}payload;

typedef struct 
{
    uint8_t psync1;
    uint8_t psync2;
    uint8_t class;
    uint8_t id;
    uint16_t lenght;
    payload payload;
    uint8_t CK_A;
    uint8_t CK_B;
}mittaus;
#pragma pack()

/*
    CK_A = 0, CK_B = 0
    For (I = 0; I < N; I++)
    {
        CK_A = CK_A + Buffer[I]
        CK_B = CK_B + CK_A
    }*/

void calculateChecksum(mittaus * mittaus, uint8_t * CK_A, uint8_t * CK_B){
    *CK_A = 0, *CK_B = 0;
    int N_BYTES = 4 + mittaus->lenght;    //4bytes + payload
    uint8_t buffer[N_BYTES];
    memcpy(buffer, (uint8_t*)mittaus + 2, N_BYTES); //We ignore the first two bytes
    
    for (int i = 0; i < N_BYTES;i++){
        *CK_A = *CK_A + buffer[i];
        *CK_B = *CK_B + *CK_A;
    }
    printf("Original CK_A: %" PRIu8 "\n", mittaus->CK_A);
    printf("Original CK_B: %" PRIu8 "\n", mittaus->CK_B);

    printf("Calculated CK_A: %" PRIu8 "\n", *CK_A);
    printf("Calculated CK_B: %" PRIu8 "\n", *CK_B);
}

void printPayload(payload * load){

    printf("----------PAYLOAD------------:\n"
            "aField: %" PRIu32 "\n"
            "anotherField: %" PRIu32 "\n"
            "Flags: %" PRIu16 "\n"
            "reserved0: %d \n"
            "numRepeat: %" PRIu8 "\n",
            load->aField, load->anotherField, load->bitfield, load->reserved0, load->numRepeat);
    if(load->numRepeat > 0){
        printf("-----REPEATE GROUP-----\n");
        for (int i = 0; i < load->numRepeat; i++){
            printf("(NEW GROUP)\n");
            printf("RepeatGroup1: %" PRIu16 "\n"
                   "RepeatGroup2: %" PRIu16 "\n",
                   load->repeats[i].firstValue, load->repeats[i].secondValue);
        }
    }else{
        printf("No repeats!\n");
    }
}

void printMittaus(mittaus * mittaus){
    printf("psync1: 0x%x \n"
            "psync2: 0x%x \n"
            "class: %" PRIu8 "\n"
            "id: %" PRIu8 "\n"
            "lenght: %" PRIu16 "\n"
            "CK_A: %" PRIu8 "\n"
            "CK_B: %" PRIu8 "\n",
            mittaus->psync1,
            mittaus->psync2,
            mittaus->class,
            mittaus->id,
            mittaus->lenght,
            mittaus->CK_A,
            mittaus->CK_B
        );
    //printPayload(&mittaus->payload);
}

void freeAndPrint(mittaus * mittaus){
    printMittaus(mittaus);
    free(mittaus->payload.repeats);
}
void freeAll(mittaus * mittaus, int N){
    for (int i = 0; i < N; i++){
        free(mittaus[i].payload.repeats);
    }
}

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
        fread(&testiMittaus, 6, 1, file);
    //printf("Address of testimittaus: %x \n", &testiMittaus);
    //printf("Address of testimittaus.checksum: %x \n", &testiMittaus.CK_A);
        //First 15 bytes of the payload are constant (see page 46 in linked site)
        fread(&testiMittaus.payload, 16, 1, file);

        //Took me too long to realize that the data is in little endian format
        testiMittaus.payload.aField = swapEndian32(testiMittaus.payload.aField);
        testiMittaus.payload.anotherField = swapEndian32(testiMittaus.payload.anotherField);
        testiMittaus.lenght = swapEndian16(testiMittaus.lenght);

        printf("numRep: %" PRIu8 "\n", testiMittaus.payload.numRepeat);
        size_t repeatSize = 0;
        if(testiMittaus.payload.numRepeat == 0){
        }else{
            size_t repeatSize = 4 * testiMittaus.payload.numRepeat;
            testiMittaus.payload.repeats = calloc(1, repeatSize);
            fread(testiMittaus.payload.repeats, sizeof(repeatGroup), testiMittaus.payload.numRepeat, file);
        }

        fread(&testiMittaus.CK_A, sizeof(uint8_t), 2, file);

        //mittaukset = realloc(mittaukset, sizeof(mittaukset) + sizeof(mittaus));
        mittaukset[idx++] = testiMittaus;
        printMittaus(&testiMittaus);

        uint8_t CHKA, CHKB;
        calculateChecksum(&testiMittaus, &CHKA, &CHKB);
        if(testiMittaus.CK_A == CHKA && testiMittaus.CK_B == CHKB){
            puts("Checksum approved!!");
            
        }else if(idx > 3){
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
        free(mittaukset[i].payload.repeats);
    }
        // printMittaus(&testiMittaus);
        // free(testiMittaus.payload.repeats);
        return 0;
}