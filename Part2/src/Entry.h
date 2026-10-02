#pragma once

#include <stdint.h>
#include <cstdio>

struct Entry {
    float x0;
    float y0;
    float x1;
    float y1;
};

struct CoordinatePairs {
    int count;
    Entry* entries;
};

void PrintCoordinatePairs(CoordinatePairs *pairs)
{
    for (int i = 0; i < pairs->count; i++)
    {
        printf("Entry: \n");
        printf("\tx0=%f\n", pairs->entries[i].x0);
        printf("\ty0=%f\n", pairs->entries[i].y0);
        printf("\tx1=%f\n", pairs->entries[i].x1);
        printf("\ty1=%f\n", pairs->entries[i].y1);
    }
}