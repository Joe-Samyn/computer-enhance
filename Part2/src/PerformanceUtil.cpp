
#include "PerformanceUtil.h"

#include <cstdio>
#include <x86intrin.h>

#if _WIN32

unsigned long long GetOSTime()
{
    printf("ERROR::Function not implemented.\n");
}

unsigned long long GetOSFrequency()
{
    printf("ERROR::Function not implemented.\n");
}

unsigned long long GetCPUTime()
{
    printf("ERROR::Function not implemented.\n");
}

#else 
#include <time.h>

/*
    NOTE: On Linux, we just return 1,000,000,000 to convert to nanoseconds as this is the units used by Linux. 
*/
unsigned long long GetOSFrequency()
{
    return 1000000000; 
}

unsigned long long GetOSTime()
{
    struct timespec ts;
    int timeResult = clock_gettime(CLOCK_MONOTONIC, &ts);
    
    // TODO: validate we successfully recieved time

    long timestamp = GetOSFrequency()*ts.tv_sec + ts.tv_nsec;
    return timestamp;
}

unsigned long long GetCPUTime()
{
    return __rdtsc();
}

#endif