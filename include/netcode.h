#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ubxf.h"
#pragma once


#define BACKEND_URL "http://localhost:3001/api/measurements"

void sendMeasurement(UBX_NAV_HPPOSLLH_load * m);