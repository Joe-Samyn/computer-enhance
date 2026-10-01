
#include "Unity.cpp"

#include <cstdio>
#include <cstring>
#include <stdint.h>
#include <string>

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
            Pairs pairs;
            const char* inputJson = argv[2];
            JsonValue *json = DeserializeJson(inputJson);
        }
        else 
        {
            PrintHelp();
        }
    }
    
    return 0;
}