#include "netcode.h"
#include <curl/curl.h>
#include "ubxf.h"


void sendMeasurement(UBX_NAV_HPPOSLLH_load * m){
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl){
        char json[256];
        
        //Reading json into buffer
        snprintf(json, sizeof(json),
            "{"
            "\"iTOW\": %u,"
            "\"lon\": %.7f,"
            "\"lat\": %.7f,"
            "\"height\": %d,"
            "\"hMSL\": %d,"
            "\"hAcc\": %u,"
            "\"vAcc\": %u"
            "}",
            m->iTOW,
            m->lon / 10000000.0,
            m->lat / 10000000.0,
            m->height,
            m->hMSL,
            m->hAcc,
            m->vAcc
        );

        curl_easy_setopt(curl, CURLOPT_URL, BACKEND_URL);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(json));
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Content-Type: application/json"));


        res = curl_easy_perform(curl);

        if(res != CURLE_OK){
            fprintf(stderr, "CURL ERROR, sendMeasurement() %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }

}