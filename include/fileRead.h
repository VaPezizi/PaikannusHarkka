#include <stdlib.h>
#include <stdio.h>

#ifndef FILEREAD_H
#define FILEREAD_H

int beginReadFile(const char* filename, size_t filenameLen);
int cleanUpFile(FILE * file);

int beginReadSerial(const char* portName);
#endif