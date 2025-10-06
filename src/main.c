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
/*typedef struct
{
    uint32_t aField;
    uint32_t anotherField;
    uint16_t bitfield;
    uint8_t reserved0[5];
    uint8_t numRepeat;
    repeatGroup * repeats;
}payload;*/

typedef struct{
	uint32_t iTOW;
	int32_t lon;
	int32_t lat;
	int32_t height;
	int32_t hMSL;
	uint32_t hAcc;
	uint32_t vAcc;

}UBX_NAV_POSLLH_load;

typedef struct{
	uint8_t byte;
}payload;

typedef struct 
{
    uint8_t psync1;
    uint8_t psync2;
    uint8_t class;
    uint8_t id;
    uint16_t lenght;
    payload * payload;
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
    memcpy(buffer, &mittaus->class, 4); //We ignore the first two bytes
    memcpy(buffer + 4, mittaus->payload ,mittaus->lenght);
    
    for (int i = 0; i < N_BYTES;i++){
        *CK_A = *CK_A + buffer[i];
        *CK_B = *CK_B + *CK_A;
    }
    /*printf("Original CK_A: %" PRIu8 "\n", mittaus->CK_A);
    printf("Original CK_B: %" PRIu8 "\n", mittaus->CK_B);

    printf("Calculated CK_A: %" PRIu8 "\n", *CK_A);
    printf("Calculated CK_B: %" PRIu8 "\n", *CK_B);
	*/
}

void printPayload(size_t len, payload * load){

	printf("----------PAYLOAD------------:\n");
	for(int i = 0; i < len; i++){
		printf("Payload byte n.%d: 0x%x\r\n", i, load[i]);
	}
	

	printf("-----------------------------:\n");
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
    printPayload(mittaus->lenght, mittaus->payload);
}

void freeAndPrint(mittaus * mittaus){
    printMittaus(mittaus);
    free(mittaus->payload);
}
void freeAll(mittaus * mittaus, int N){
    for (int i = 0; i < N; i++){
        free(mittaus[i].payload);
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
        //fread(&testiMittaus, 6, 1, file);

	//Parsing the data safely
	fread(&testiMittaus, 1, 1, file);
	if(testiMittaus.psync1 == 0xb5){	//Found first sync char
		fread(&testiMittaus.psync2, 1, 1, file);
		if(testiMittaus.psync2 == 0x62){
			fread(&testiMittaus.class, 4, 1, file);
			
			testiMittaus.lenght = swapEndian16(testiMittaus.lenght);

			//Allocate memory and read into payload
			testiMittaus.payload = calloc(1, testiMittaus.lenght);
			fread(testiMittaus.payload, testiMittaus.lenght, 1, file);

			fread(&testiMittaus.CK_A, 1, 2, file);

			//mittaukset = realloc(mittaukset, sizeof(mittaukset) + sizeof(mittaus));
			mittaukset[idx++] = testiMittaus;
			//printMittaus(&testiMittaus);

			uint8_t CHKA, CHKB;
			calculateChecksum(&testiMittaus, &CHKA, &CHKB);
			if(testiMittaus.CK_A == CHKA && testiMittaus.CK_B == CHKB){
			    //puts("Checksum approved!!");
			}
			printf("Class: 0x%x, ID: 0x%x", testiMittaus.class, testiMittaus.id);
			if(testiMittaus.class == 0x01){
				printf("Arvo: 0x%x\n", testiMittaus.id);
			}	
			if(testiMittaus.class == 0x01 && testiMittaus.id == 0x02){
				UBX_NAV_POSLLH_load testiLoad;
				memcpy(&testiLoad, testiMittaus.payload, testiMittaus.lenght);

				printf("lat: %d, lon: %d, height: %d\n", testiLoad.lat, testiLoad.lon, testiLoad.height);
			}
		
	}

        //Took me too long to realize that the data is in little endian format

            
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
