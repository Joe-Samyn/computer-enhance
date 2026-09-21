
#include "Unity.cpp"

#include <cstdio>
#include <cstring>
#include <stdint.h>
#include <string>

struct Arguments {
    const char* outputFile;
    uint32_t sampleSize;
};

// MODES
#define GENERATE_MODE "-generate"
#define SOLVE_MODE "-solve"

// FLAGS 
#define O "-o" // Output file
#define S "-s" // Random number seed 


// TODO: Support 'S' flag
int main(int argc, char* argv[]) {

    if (argc < 3) {
        printf("Options:\n");
        printf("\t-generate <sample size>: Generate sample input in JSON format.\n");
        printf("\t-solve <input json>: Compute the haversine distance between each sample in the input JSON.\n");
    }
    else {
        Arguments args = {};
        const char* mode = argv[1];

        int i = 2; 
        while (i < argc) {
            const char* flag = argv[i];

            if (std::strcmp(flag, O) == 0) {
                args.outputFile = argv[++i];
            }

            i++;
        }

        if (std::strcmp(mode, GENERATE_MODE) == 0) {
            // TODO: Need to verify this argument is a valid int (>0 & a number) else exit with error. 
            // TODO: Magic #2 here, need to make this a constant
            args.sampleSize = std::stoi(argv[2]); 

            if (std::strcmp(args.outputFile, "") == 0) {
                args.outputFile = "sample_out.json";
            }
            
            Generate(args.outputFile, args.sampleSize);
        }
        else if (std::strcmp(mode, SOLVE_MODE) == 0) {
            printf("Solve the JSON.\n");
        }
        else {
            // Assume they passed an incorrect mode
            printf("Options:\n");
            printf("\t-generate <sample size>: Generate sample input in JSON format.\n");
            printf("\t-solve <input json>: Compute the haversine distance between each sample in the input JSON.\n");
        }
    }
    
    return 0;
}