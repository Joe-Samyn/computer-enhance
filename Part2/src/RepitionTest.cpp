
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

enum Measurements
{
    MIN_RUNTIME,
    MAX_RUNTIME,
    MIN_PAGEFAULT_COUNT,
    MAX_PAGEFAULT_COUNT,
    TOTAL_PAGEFAULT_COUNT,
    TOTAL_RUNTIME,
    TOTAL_RUN_COUNT,
    ERROR_NO,

    Measurements_Count
};

struct TestResults
{
    unsigned long long results[Measurements_Count];
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


void PrintRow(const char* label, long bytes, unsigned long long elapsed, unsigned long long pfCount, unsigned long long cpuFrequency)
{
    double ms = CPUTimeToMs(elapsed, cpuFrequency);
    double throughput = (1000.0 * bytes) / (1024*1024*1024*ms);
    printf("%s: %llu [%0.2f ms]    [%.3f GB/s]    [%d Page Faults]\n", label, elapsed, ms, throughput, pfCount);
}

void PrintResults(const char* name, TestResults *testResults, TestRunConfiguration *configuration)
{
    printf("                                    \n");
    
    printf("-- %s --\n", name);
    printf("File Size: %llu bytes\n", configuration->fileSize);
    PrintRow("MIN", configuration->fileSize, testResults->results[MIN_RUNTIME], testResults->results[MIN_PAGEFAULT_COUNT], configuration->cpuFrequency);
    PrintRow("AVG", configuration->fileSize, testResults->results[TOTAL_RUNTIME] / testResults->results[TOTAL_RUN_COUNT], testResults->results[TOTAL_PAGEFAULT_COUNT] / testResults->results[TOTAL_RUN_COUNT], configuration->cpuFrequency);
    PrintRow("MAX", configuration->fileSize, testResults->results[MAX_RUNTIME], testResults->results[MAX_PAGEFAULT_COUNT], configuration->cpuFrequency);
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

void ReadFileWithAlloc(TestRunConfiguration *configuration, TestResults *testResults)
{
    FILE *file = fopen(configuration->file, "rb");
    unsigned long long elapsed = GetCPUTime();
    unsigned long long pageFaults = GetPageFaultCount();
    Buffer b = {
        .buffer = (char*)malloc(configuration->fileSize * sizeof(char)),
        .count = (unsigned long long)GetFileSize(configuration->file)
    };

    int result = fread(b.buffer, sizeof(char), b.count, file);
    elapsed = GetCPUTime() - elapsed;
    pageFaults = GetPageFaultCount() - pageFaults;

    // File read error because result == 0
    if (!result)
    {
        printf("ERROR::Could not read file.\n");
        testResults->results[ERROR_NO] = ERROR_FILE_READ;
    }

    // Read bytes != expected byte count
    if (result != b.count)
    {
        printf("ERROR::Byte count mismatch.\n");
        testResults->results[ERROR_NO] = ERROR_SIZE_MISMATCH;
    }

    // No errors occurred, record results
    if (result == b.count)
    {
        if (testResults->results[MIN_RUNTIME] > elapsed)
        {
            testResults->results[MIN_RUNTIME] = elapsed;
            configuration->timer = GetOSTimeMS();
        }

        if (testResults->results[MAX_RUNTIME] < elapsed)
        {
            testResults->results[MAX_RUNTIME] = elapsed;
            testResults->results[MAX_PAGEFAULT_COUNT] = pageFaults;
        }
        testResults->results[TOTAL_RUNTIME] += elapsed;
        testResults->results[TOTAL_PAGEFAULT_COUNT] += pageFaults;
        testResults->results[TOTAL_RUN_COUNT]++;

        fclose(file);
    }

    free(b.buffer);
}

void ReadFileWithoutAlloc(Buffer* b, TestRunConfiguration* configuration, TestResults* testResults)
{
    FILE* file = fopen(configuration->file, "rb");
    unsigned long long elapsed = GetCPUTime();
    unsigned long long pageFaults = GetPageFaultCount();
    int result = fread(b->buffer, sizeof(char), b->count, file);
    elapsed = GetCPUTime() - elapsed;
    pageFaults = GetPageFaultCount() - pageFaults;

    // File read error because result == 0
    if (!result)
    {
        printf("ERROR::Could not read file.\n");
        testResults->results[ERROR_NO] = ERROR_FILE_READ;
    }

    // Read bytes != expected byte count
    if (result != b->count)
    {
        printf("ERROR::Byte count mismatch.\n");
        testResults->results[ERROR_NO] = ERROR_SIZE_MISMATCH;
    }

    // No errors occurred, record results
    if (result == b->count)
    {
        if (testResults->results[MIN_RUNTIME] > elapsed)
        {
            testResults->results[MIN_RUNTIME] = elapsed;
            configuration->timer = GetOSTimeMS();
        }

        if (testResults->results[MAX_RUNTIME] < elapsed)
        {
            testResults->results[MAX_RUNTIME] = elapsed;
            testResults->results[MAX_PAGEFAULT_COUNT] = pageFaults;
        }
        testResults->results[TOTAL_RUNTIME] += elapsed;
        testResults->results[TOTAL_PAGEFAULT_COUNT] += pageFaults;
        testResults->results[TOTAL_RUN_COUNT]++;

        fclose(file);
    }
}

void TestFReadWithAlloc(TestRunConfiguration *configuration)
{
    TestResults testResults = {};
    testResults.results[MIN_RUNTIME] = INT_MAX;
    testResults.results[MAX_RUNTIME] = 0;

    configuration->timer = GetOSTimeMS();
    while (GetOSTimeMS() - configuration->timer < RUNTIME)
    {
        ReadFileWithAlloc(configuration, &testResults);
    }

    PrintResults("fread - Alloc", &testResults, configuration);
}

void TestFReadNoAlloc(TestRunConfiguration* configuration, Buffer *buffer)
{
    TestResults testResults = {};
    testResults.results[MIN_RUNTIME] = INT_MAX;
    testResults.results[MAX_RUNTIME] = 0;


    configuration->timer = GetOSTimeMS();
    while (GetOSTimeMS() - configuration->timer < RUNTIME)
    {
        ReadFileWithoutAlloc(buffer, configuration, &testResults);
    }

    PrintResults("fread - No Alloc", &testResults, configuration);

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

    InitializeMemoryTooling();

    while(true)
    {
        TestFReadWithAlloc(&configuration);
        TestFReadNoAlloc(&configuration, &b);
    }

    return 0;
}
