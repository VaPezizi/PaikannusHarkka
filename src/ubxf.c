#include "ubxf.h"

void freeAndPrint(mittaus * mittaus){
    printMittaus(mittaus);
    free(mittaus->payload);
}
void freeAll(mittaus * mittaus, int N){
    for (int i = 0; i < N; i++){
        free(mittaus[i].payload);
    }
}

void calculateChecksum(mittaus * mittaus, uint8_t * CK_A, uint8_t * CK_B){
    *CK_A = 0, *CK_B = 0;
    int N_BYTES = 4 + mittaus->lenght;    //4bytes + payload
    uint8_t buffer[N_BYTES];
    memcpy(buffer, &mittaus->mclass, 4); //We ignore the first two bytes
    memcpy(buffer + 4, mittaus->payload ,mittaus->lenght);
    
    for (int i = 0; i < N_BYTES;i++){
        *CK_A = *CK_A + buffer[i];
        *CK_B = *CK_B + *CK_A;
    }
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
            mittaus->mclass,
            mittaus->id,
            mittaus->lenght,
            mittaus->CK_A,
            mittaus->CK_B
        );
    printPayload(mittaus->lenght, mittaus->payload);
}