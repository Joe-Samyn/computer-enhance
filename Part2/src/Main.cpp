
#include "Unity.cpp"
#include "Entry.h"

#include <cstdio>
#include <cstring>
#include <stdint.h>
#include <string>

void SolveForPairs(CoordinatePairs *pairs)
{
    Entry *entries = pairs->entries;
    double sumCoef = 1 / (double)pairs->count;
    double sum = 0;
    for (int i = 0; i < pairs->count; i++)
    {
        double distance = HaversineDistance(entries[i]);
        sum += distance*sumCoef;
    }

    printf("Sum: %f\n", sum);
}

void PrintHelp() {
    printf("Options:\n");
    printf("\t-generate <sample size>: Generate sample input in JSON format.\n");
    printf("\t-solve <input json>: Compute the haversine distance between each sample in the input JSON.\n");
}

int GetArgument(int argc, char* argv[], const char* flag) {

    int i = 1;
    const char* target;
    while((i < argc) && (strcmp(target, flag) != 0)) {
        target = argv[i];
        i++;
    }

    return (i >= argc) ? -1 : i;
}


// TODO: Support 'S' flag
int main(int argc, char* argv[]) {

    if (argc < 3) {
        PrintHelp();
    }
    else {
        const char* mode = argv[1];
        if (std::strcmp(mode, "-generate") == 0) 
        {
            int sampleSize = std::stoi(argv[2]);

            int outfileIndex = GetArgument(argc, argv, "-o");
            const char* outfile = outfileIndex == -1 ? "sample.json" : argv[outfileIndex];
            
            Generate(outfile, sampleSize);
        }
        else if (std::strcmp(mode, "-solve") == 0) 
        {
            const char* inputJson = argv[2];
            CoordinatePairs pairs = DeserializeCoordinatePairs(inputJson);
            SolveForPairs(&pairs);
        }
        else 
        {
            PrintHelp();
        }
    }
    
    return 0;
}