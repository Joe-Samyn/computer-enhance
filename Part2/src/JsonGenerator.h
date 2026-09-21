
#include <stdint.h>

struct Entry {
    int32_t x0; 
    int32_t y0; 
    int32_t x1;
    int32_t y1;
};

void Generate(const char* outputFile, int32_t sampleSize);