
#include "PerformanceUtil.cpp"

#include <stdio.h>
#include <cstdlib>

#define RUNTIME 10000


enum Error {
    // No error occurred
    ERROR_NONE,

    // Error reading file 
    ERROR_FILE_READ,

    // Number of bytes read does not match expected number of bytes
    ERROR_SIZE_MISMATCH
};

struct Buffer {
    /**
     * Buffer for read file contents
     */
    char* buffer;

    /**
     * Number of bytes read
     */
    unsigned long count;
};

struct TestRunConfiguration {
    
    /**
     * Path and name of the file to read for tests
     */
    const char* file;

    /**
     * Current elapsed time in milliseconds
     */
    int timer;

    /**
     * Enable printing every new minimum encountered to the console
     */
    bool displayNewMin;

    /**
     * Estimated CPU frequency (rdtsc cycles / s). 
     * Note: This is required to estimate runtime in milliseconds
     */
    unsigned long long cpuFrequency;
};

struct TestRunResults {
    /**
     * Minimum runtime for the test run 
     */
    unsigned long long min;

    /**
     * Total summed runtime for the test runs
     */
    unsigned long long total;

    /**
     * Maximum runtime for the test run
     */
    unsigned long long max;

    /**
     * The number of times the test was ran
     */
    unsigned long long numberOfRuns;

    /**
     * Error that occurred while reading the file. If no error occurs, this is ERROR_NONE
     */
    Error error;
};

void PrintRow(const char* label, long bytes, unsigned long long elapsed, unsigned long long cpuFrequency)
{
    double ms = CPUTimeToMs(elapsed, cpuFrequency);
    double throughput = (1000 * bytes) / (1024*1024*1024*ms);
    printf("%s: %llu [%0.2f ms]    [%.3f GB/s]\n", label, elapsed, ms, throughput);
}

void PrintResults(const char* name, unsigned long bytes, TestRunResults *results, TestRunConfiguration *configuration)
{
    printf("                                    \n");
    
    printf("-- %s --\n", name);
    PrintRow("MIN", bytes, results->min, configuration->cpuFrequency);
    PrintRow("AVG", bytes, results->total / results->numberOfRuns, configuration->cpuFrequency);
    PrintRow("MAX", bytes, results->max, configuration->cpuFrequency);
}

long GetFileSize(FILE *file)
{
    if (fseek(file, 0, SEEK_END) != 0)
    {
        return 0;
    }

    long fileSize = ftell(file);
    if (fileSize < 0)
    {
        fclose(file);
        return 0;
    }

    // Return to the beginning.
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        fclose(file);
        return 0;
    }

    return fileSize;
}

void ReadFile(Buffer *b, TestRunConfiguration *configuration, TestRunResults *results)
{
    FILE *file = fopen(configuration->file, "rb");
    b->count = (unsigned long)GetFileSize(file);
    b->buffer = (char*)malloc(b->count * sizeof(char));

    unsigned long long start = GetCPUTime();
    int result = fread(b->buffer, sizeof(char), b->count, file);
    unsigned long long end = GetCPUTime();

    // File read error because result == 0
    if (!result)
    {
        printf("ERROR::Could not read file.\n");
        results->error = ERROR_FILE_READ;
    }

    // Read bytes != expected byte count
    if (result != b->count)
    {
        printf("ERROR::Byte count mismatch.\n");
        results->error = ERROR_SIZE_MISMATCH;
    }

    // No errors occurred, record results
    if (result == b->count)
    {
        unsigned long long elapsed = end - start;
        if (results->min > elapsed)
        {
            results->min = elapsed;
            configuration->timer = GetOSTimeMS();
        }
        results->total += elapsed;
        results->max = results->max < elapsed ? elapsed : results->max;
        results->numberOfRuns++;
    }

    free(b->buffer);
    fclose(file);
}

void TestFRead(TestRunConfiguration *configuration)
{
    TestRunResults results = {
        .min = __INT_MAX__
    };

    Buffer b = {};

    configuration->timer = GetOSTimeMS();
    while (GetOSTimeMS() - configuration->timer < RUNTIME)
    {
        ReadFile(&b, configuration, &results);
    }

    PrintResults("fread", b.count, &results, configuration);
}


int main(int argc, const char* argv[]) 
{
    if (argc < 2)
    {
        printf("ERROR::Invalid argument. File path argument is required.\n");
        return 1;
    }

    const char* file = argv[1];

    TestRunConfiguration configuration = {
        .file = file,
        .timer = 10000,
        .displayNewMin = false,
        .cpuFrequency = EstimateCPUTimerFrequency()
    };

    TestFRead(&configuration);
    TestFRead(&configuration);

    return 0;
}