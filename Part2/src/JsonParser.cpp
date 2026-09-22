#include "JsonParser.h"

#include <cstdio>
#include <cerrno>


void DeserializePairs(const char* jsonFile, Pairs &pairs) {

    // 1. Open file
    FILE* file = std::fopen(jsonFile, "r");
    if (!file) {
        printf("ERROR::%d - Could not open file %s", errno, jsonFile);
        return;
    }

    // 2. Loop over characters until EOF
    int c;
    while((c = fgetc(file)) != EOF) {
        char token = (char)c;
        // A. match character to value

        switch(token) {
            case '{':
            {
                printf("Parsing object.\n");
            } break;
            case '[':
            {
                printf("Parsing array.\n");
            } break;
            case '"':
            {
                printf("Parsing string.\n");
            } break;
            case 't':
            {
                printf("Parsing true.\n");
            } break;
            case 'f':
            {
                printf("Parsing false.\n");
            } break;
            case 'n':
            {
                printf("Parsing null.\n");
            }
            default:
            {
                printf("Parsing digit.\n");
            }
        }

    }

}
