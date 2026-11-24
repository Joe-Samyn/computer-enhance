
#include "PerformanceUtil.cpp"

#include <stdio.h>
#include <cstdlib>
#include <climits>
#include <sys/stat.h>

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
    unsigned long long count;
};

struct TestRunConfiguration {
    
    /**
     * Path and name of the file to read for tests
     */
    const char* file;

    /**
     * @brief Size of file in bytes
     */
    long long fileSize;

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
    double throughput = (1000.0 * bytes) / (1024*1024*1024*ms);
    printf("%s: %llu [%0.2f ms]    [%.3f GB/s]\n", label, elapsed, ms, throughput);
}

void PrintResults(const char* name, TestRunResults *results, TestRunConfiguration *configuration)
{
    printf("                                    \n");
    
    printf("-- %s --\n", name);
    PrintRow("MIN", configuration->fileSize, results->min, configuration->cpuFrequency);
    PrintRow("AVG", configuration->fileSize, results->total / results->numberOfRuns, configuration->cpuFrequency);
    PrintRow("MAX", configuration->fileSize, results->max, configuration->cpuFrequency);
}

long long GetFileSize(const char* file)
{
    struct __stat64 info;

    if (_stat64(file, &info) != 0)
    {
        perror("_stat64");
        return -1;
    }

    return info.st_size;
}

void ReadFileWithAlloc(TestRunConfiguration *configuration, TestRunResults *results)
{
    FILE *file = fopen(configuration->file, "rb");
    unsigned long long start = GetCPUTime();
    Buffer b = {
        .buffer = (char*)malloc(configuration->fileSize * sizeof(char)),
        .count = (unsigned long long)GetFileSize(configuration->file)
    };

    int result = fread(b.buffer, sizeof(char), b.count, file);
    unsigned long long end = GetCPUTime();

    // File read error because result == 0
    if (!result)
    {
        printf("ERROR::Could not read file.\n");
        results->error = ERROR_FILE_READ;
    }

    // Read bytes != expected byte count
    if (result != b.count)
    {
        printf("ERROR::Byte count mismatch.\n");
        results->error = ERROR_SIZE_MISMATCH;
    }

    // No errors occurred, record results
    if (result == b.count)
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

        fclose(file);
    }

    free(b.buffer);
}

void ReadFileWithoutAlloc(Buffer* b, TestRunConfiguration* configuration, TestRunResults* results)
{
    FILE* file = fopen(configuration->file, "rb");
    unsigned long long start = GetCPUTime();
    int result = fread(b->buffer, sizeof(char), b->count, file);
    unsigned long long end = GetCPUTime();

    // File read error because result == 0
    if (!result)
    {
        perror(configuration->file);
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
        fclose(file);
    }
}

void TestFReadWithAlloc(TestRunConfiguration *configuration)
{
    TestRunResults results = {
        .min = INT_MAX
    };

    configuration->timer = GetOSTimeMS();
    while (GetOSTimeMS() - configuration->timer < RUNTIME)
    {
        ReadFileWithAlloc(configuration, &results);
    }

    PrintResults("fread - Alloc", &results, configuration);
}

void TestFReadNoAlloc(TestRunConfiguration* configuration, Buffer *buffer)
{
    TestRunResults results = {
        .min = INT_MAX
    };


    configuration->timer = GetOSTimeMS();
    while (GetOSTimeMS() - configuration->timer < RUNTIME)
    {
        ReadFileWithoutAlloc(buffer, configuration, &results);
    }

    PrintResults("fread - No Alloc", &results, configuration);

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
        .fileSize = GetFileSize(file),
        .timer = 10000,
        .displayNewMin = false,
        .cpuFrequency = EstimateCPUTimerFrequency()
    };

    Buffer b = {};
    b.buffer = (char*)malloc(sizeof(char) * configuration.fileSize);
    b.count = configuration.fileSize;

    while(true)
    {
        TestFReadWithAlloc(&configuration);
        TestFReadNoAlloc(&configuration, &b);
    }

    return 0;
}
