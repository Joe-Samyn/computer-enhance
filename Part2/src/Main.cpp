
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
    printf("\t-time: View the performing timing for 1 second on this CPU platform.");
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

    if (argc < 2) {
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
        else if (std::strcmp(mode, "-time") == 0)
        {
            unsigned long long osStartTime = GetOSTime(); // nanoseconds
            unsigned long long osEndTime; // nanoseconds
            unsigned long long osElapsedTime = GetOSTime() - osStartTime; // nanoseconds
            unsigned long long osFreq = GetOSFrequency(); // ticks / second
            unsigned long long cpuTimeStart = GetCPUTime(); // ticks/cycles
            while(osElapsedTime < GetOSFrequency())
            {
                osEndTime = GetOSTime();
                osElapsedTime = osEndTime - osStartTime;
            }
            
            unsigned long long cpuTimeEnd = GetCPUTime();
            unsigned long long cpuTimeElapsed = cpuTimeEnd - cpuTimeStart;

            printf("OS Timer: %llu -> %llu = %llu elapsed\n", osStartTime, osEndTime, osElapsedTime);
            printf("OS Seconds: %llu -> %llu elapsed\n", osElapsedTime, osElapsedTime / GetOSFrequency());
            printf("CPU Time: %llu -> %llu = %llu elapsed\n", cpuTimeStart, cpuTimeEnd, cpuTimeElapsed);
            printf("Approx CPU Time: %llu (Hz)\n", (cpuTimeElapsed * osFreq) / osElapsedTime);
        }
        else 
        {
            PrintHelp();
        }
    }
    
    return 0;
}