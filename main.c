#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>

//Ublox documentation
//https://content.u-blox.com/sites/default/files/u-blox%20M10-SPG-5.00_InterfaceDescription_UBX-20053845.pdf?utm_content=UBX-20053845

#define FILENAME "COM8___9600_231127_110129.ubx"



#pragma pack(1)
typedef struct 
{
    uint8_t psync1;
    uint8_t psync2;
    uint8_t class;
    uint8_t id;
    uint16_t lenght;
    uint8_t * payload;
    uint8_t CK_A;
    uint8_t CK_B;
}mittaus;
#pragma pack()

void printMittaus(mittaus * mittaus){
    printf("psync1: %d \n"
            "psync2: %d \n"
            "class: %d \n"
            "id: %d \n"
            "lenght: %d \n"
            "payload: %d \n2"
            "CK_A: %d \n"
            "CK_B: %d \n",
            mittaus->psync1,
            mittaus->psync2,
            mittaus->class,
            mittaus->id,
            mittaus->lenght,
            *mittaus->payload,
            mittaus->CK_A,
            mittaus->CK_B
        );
}

int main(){
    printf("Morjestaa!\n");

    FILE* file = fopen(FILENAME, "r");
    if(!file){
        puts("VIRHE");
        exit(1);
    }

    mittaus testiMittaus;
    memcpy(&testiMittaus, file, 4);
    memcpy(&testiMittaus + 4, file + 4, testiMittaus.lenght);
    memcpy(&testiMittaus + 4 + testiMittaus.lenght, file + 4 + testiMittaus.lenght, 16);
    /*while(){

    }*/
    printMittaus(&testiMittaus);
    return 0;
}