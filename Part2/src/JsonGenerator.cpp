#include "JsonGenerator.h"
#include "Entry.h"

#include <cstdio>
#include <random>

FILE* OpenFile(const char* outputFile) {
    FILE *outfile = fopen(outputFile, "w");
    if (!outfile) {
        printf("Could not open file %s\n", outputFile);
        printf("Error: %d\n", errno);
        return 0;
    }

    fprintf(outfile, "{\"pairs\":[");

    return outfile;
}

void CloseFile(FILE* outfile) {
    fprintf(outfile, "] }\n");

    fclose(outfile);
}

void WriteEntriesJson(FILE* outfile, Entry *entries, uint32_t sampleSize) {
    for (int i = 0; i < sampleSize; i++) {
        // TODO: Super overkill to determine if a ',' should be there or not at the end. Need to fix.
        if (i < sampleSize - 1) {
            fprintf(outfile, "{\"x0\": %f, \"y0\": %f, \"x1\": %f, \"y1\": %f},", entries[i].x0, entries[i].y0, entries[i].x1, entries[i].y1);
        }
        else {
            fprintf(outfile, "{\"x0\": %f, \"y0\": %f, \"x1\": %f, \"y1\": %f}", entries[i].x0, entries[i].y0, entries[i].x1, entries[i].y1);
        }
    }
}

void Generate(const char* outputFile, uint32_t sampleSize) {
    
    FILE *outfile = OpenFile(outputFile);

    if (!outfile) return;

    Entry entries[sampleSize];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> latitudeGen(-90.0, 90.0);
    std::uniform_real_distribution<float> longitudeGen(-180.0, 180.0);
    for(int i = 0; i < sampleSize; i++) {
        Entry entry = {
            .x0=latitudeGen(gen),
            .y0=longitudeGen(gen),
            .x1=latitudeGen(gen),
            .y1=longitudeGen(gen)
        };

        entries[i] = entry;
    }

    WriteEntriesJson(outfile, entries, sampleSize);

    CloseFile(outfile);

}