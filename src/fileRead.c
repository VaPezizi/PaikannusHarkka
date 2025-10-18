#include "fileRead.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>
#include "ubxf.h"
#include "endians.h"
#include "netcode.h"


#ifndef _WIN32
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#endif
    
int beginReadFile(const char* filename, size_t filenameLen){

    FILE* file = fopen(filename, "rb");
    if(!file){
        puts("Error in reading file!");
        exit(1);
    }
    mittaus mittaukset[10000];
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
        }        
        idx++;
        if(idx > 10000){
            freeAll(mittaukset, idx);
            cleanUpFile(file);
            exit(1);
        }
    }
    return 0;
}
int cleanUpFile(FILE * file){
    fclose(file);
    return 0;
}

#define SERIAL_PORT "/dev/ttyACM1"  // Replace with your device path
#define BAUD_RATE B9600            // Adjust based on your device's baud rate

#ifndef WIN32
int beginReadSerial(const char* portName) {
    int serial_fd = open(portName, O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_fd < 0) {
        perror("Error opening serial port");
        return 1;
    }

    // Configure the serial port
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(serial_fd, &tty) != 0) {
        perror("Error getting terminal attributes");
        close(serial_fd);
        return 1;
    }

    cfsetospeed(&tty, BAUD_RATE);
    cfsetispeed(&tty, BAUD_RATE);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit characters
    tty.c_iflag &= ~IGNBRK;                     // Disable break processing
    tty.c_lflag = 0;                            // No signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;                            // No remapping, no delays
    tty.c_cc[VMIN] = 1;                         // Read doesn't block
    tty.c_cc[VTIME] = 1;                        // 0.1 second read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);     // Shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);           // Ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);         // No parity
    tty.c_cflag &= ~CSTOPB;                    // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                   // No hardware flow control

    if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) {
        perror("Error setting terminal attributes");
        close(serial_fd);
        return 1;
    }

    // Read UBX data
    printf("Reading UBX data from %s...\n", portName);

    while (1) {
        mittaus testiMittaus;
        // Parsing the data safely
        ssize_t n = read(serial_fd, &testiMittaus.psync1, 1);
        if (n <= 0) continue;

        if (testiMittaus.psync1 == 0xb5) { // Found first sync char
            read(serial_fd, &testiMittaus.psync2, 1);
            if (testiMittaus.psync2 == 0x62) {
                read(serial_fd, &testiMittaus.mclass, 4);

                // Allocate memory and read into payload
                testiMittaus.payload = calloc(1, testiMittaus.lenght);
                read(serial_fd, testiMittaus.payload, testiMittaus.lenght);

                read(serial_fd, &testiMittaus.CK_A, 2);

                printMittaus(&testiMittaus);

                uint8_t CHKA, CHKB;
                calculateChecksum(&testiMittaus, &CHKA, &CHKB);

                if (CHKA != testiMittaus.CK_A || CHKB != testiMittaus.CK_B) {
                    fprintf(stderr, "ERROR IN CHECKSUM!\n");
                    sleep(1);
                }
                if (testiMittaus.mclass == 0x01 && testiMittaus.id == 0x14) {
                    UBX_NAV_HPPOSLLH_load testiLoad;
                    memcpy(&testiLoad, testiMittaus.payload, testiMittaus.lenght);
                    sendMeasurement(&testiLoad);
                    puts("Sent measurement, sleeping for a second");
                    sleep(1); // Sleeping for a second to avoid flooding the backend
                }
                free(testiMittaus.payload);
            }
        }
    }
    close(serial_fd);
    return 0;
}
#endif
#ifdef WIN32
    int beginReadSerial(const char* portName) {
        printf("Serial reading not implemented for Windows yet.\n");
        return 1;
    }
#endif