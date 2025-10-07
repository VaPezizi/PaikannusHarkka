#include <inttypes.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#pragma pack(1)
typedef struct{
    uint8_t version;
    uint8_t reserved1[2];
	int8_t bitfield;
    uint32_t iTOW;
	int32_t lon;    //To get coordinate, divide by 10000000
	int32_t lat;    
	int32_t height;
	int32_t hMSL;
    int8_t lonHp;
    int8_t latHp;
    int8_t heightHp;
    int8_t hMSLHp;
	uint32_t hAcc;
	uint32_t vAcc;

}UBX_NAV_HPPOSLLH_load;

typedef struct{
	uint8_t byte;
}payload;

typedef struct 
{
    uint8_t psync1;
    uint8_t psync2;
    uint8_t mclass;
    uint8_t id;
    uint16_t lenght;
    payload * payload;
    uint8_t CK_A;
    uint8_t CK_B;
}mittaus;

#pragma pack()


void freeAll(mittaus * mittaus, int N);
void freeAndPrint(mittaus * mittaus);
void printMittaus(mittaus * mittaus);
void printPayload(size_t len, payload * load);
void calculateChecksum(mittaus * mittaus, uint8_t * CK_A, uint8_t * CK_B);